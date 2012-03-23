<?php

/**
 * @file
 * Include file for integration with the node system.
 */

/**
 * Drupal _nodeapi() hook.
 */
function spam_nodeapi(&$node, $op) {
  switch ($op) {
    case 'update':
      if (!spam_bypass_filters()) {
        spam_content_update($node, 'node');
        $spam = spam_content_is_spam($node, 'node', $node->nid);
        if (isset($spam['is_spam']) && $spam['is_spam']) {
          $node->is_spam = TRUE;
        }
      }
      break;
    case 'insert':
      if (!spam_bypass_filters()) {
        spam_content_insert($node, 'node');
        $spam = spam_content_is_spam($node, 'node', $node->nid);
        if (isset($spam['is_spam']) && $spam['is_spam']) {
          $node->is_spam = TRUE;
        }
      }
      break;
    case 'delete':
      spam_content_delete($node, 'node');
      break;
    case 'view':
      $node->is_spam = FALSE;
      if (spam_score_is_spam(_spam_content_node_score($node->nid))) {
        drupal_add_css(drupal_get_path('module', 'spam') .'/content/spam-node.css');
        $node->is_spam = TRUE;
      }

  }
}

/**
 * Cache the node id to be sure it's available when we need it.
 */
function _spam_content_node_nid($id = NULL) {
  static $nid = 0;

  if (isset($id) && is_numeric($id)) {
    $nid = $id;
  }

  return $nid;
}

/**
 * Retrieve spam score, caching in memory for repeated use.
 */
function _spam_content_node_score($nid) {
  static $scores = array();

  if (!isset($scores[$nid])) {
    $scores[$nid] = db_result(db_query("SELECT score FROM {spam_tracker} WHERE content_type = 'node' AND content_id = '%s'", $nid));
  }

  return $scores[$nid];
}

/**
 * Spam module _spamapi() hook.
 */
function node_spamapi($op, $arg1 = NULL, $arg2 = NULL, $arg3 = NULL) {
  switch ($op) {
    case 'content_module':
      // Register with the spam api as a content type module.
      return 'node';

    case 'content_id':
      if (is_object($arg1)) {
        $arg1 = (array)$arg1;
      }
      if (isset($arg1['nid'])) {
        return _spam_content_node_nid($arg1['nid']);
      }
      else {
        return 0;
      }

    case 'content_types':
      // Register all node types with the spam module.
      $types = array();
      foreach (node_get_types() as $type) {
        $types[] = array(
          'name' => $type->type,
          'module' => $type->module,
          'title' => t($type->name),
          'description' => t($type->description),
          'default_value' => 0,
        );
      }
      return $types;

    case 'filter_content_type':
      if (is_array($arg1)) {
        $arg1 = (object)$arg1;
      }
      return (variable_get("spam_filter_$arg1->type", 0));

    case 'filter_fields':
      // Tell spam filter which fields should be scanned for spam.
      $fields['main'] = array('title', 'body');
      // TODO: other fields (CCK)
      return $fields;

    case 'feedback_form':
      $form = array();
      if (is_numeric($form['nid'])) {
        $form['nid'] = array(
          '#type' => 'textfield',
          '#title' => t('Node ID'),
          '#value' => $arg1['nid'],
          '#disabled' => TRUE,
        );
      }
      // fall through...
    case 'error_form':
      if (!is_array($form)) {
        $form = array();
      }
      $form['node'] = array(
        '#type' => 'fieldset',
        '#title' => $type,
      );
      $form['node']['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#value' => $arg1['title'],
        '#disabled' => TRUE,
      );
      $form['node']['body'] = array(
        '#type' => 'textarea',
        '#title' => t('Body'),
        '#value' => $arg1['body'],
        '#disabled' => TRUE,
      );
      $form['node']['author'] = array(
        '#type' => 'markup',
        '#prefix' => '<div><strong>' . t('Author') . ':</strong></div>',
        '#value' => theme('username', user_load(array('uid' => $arg1['uid']))),
      );
      // TODO: CCK fields
      return $form;

    case 'load':
      return node_load($arg1);

    case 'title':
      return db_result(db_query('SELECT title FROM {node} WHERE nid = %d', $arg1));

    case 'status':
      $status = db_result(db_query('SELECT status FROM {node} WHERE nid = %d', $arg1));
      if ($status == 1) {
        return SPAM_PUBLISHED;
      }
      else {
        return SPAM_NOT_PUBLISHED;
      }

    case 'edit_link':
      return "node/$arg1/edit";

    case 'link':
      if (is_object($arg1) && isset($arg1->nid)) {
        return spam_links('node', $arg1->nid, $arg1);
      }
      break;

    case 'redirect':
      return drupal_goto("node/$arg1");

    case 'overview_filter_join':
      return 'INNER JOIN {node} n ON t.content_id = CAST(n.nid AS CHAR(32))';

    case 'overview_filter_where':
      switch ($arg1) {
        case 'title':
          return "n.title LIKE '%%%s%%'";
        case 'status':
          return "n.status != %d";
      }

    case 'publish':
      if (is_numeric($arg1)) {
        $arg1 = node_load(array('nid' => $arg1));
        node_publish_action($arg1);
        node_save($arg1);
      }
      break;

    case 'unpublish':
      if (is_numeric($arg1)) {
        $arg1 = node_load($arg1);
        node_unpublish_action($arg1);
        node_save($arg1);
      }
      break;

  }
}

/**
 * Preprocess function to add some class for comments marked as spam.
 */
function spam_preprocess_node(&$vars) {
  $node = $vars['node'];
  if ($node->is_spam) {
    $vars['content'] = '<div class="node-spam-content">' . $vars['content'] . '</div>';
  }
}

/**
 * Form alter gets its own function so we can reference &$form without causing
 * errors in PHP4 installations.  (If we use spamapi, we have to set a default,
 * which PHP4 doesn't support.)
 */
function node_spamapi_form_alter(&$form, &$form_state, $form_id) {
  if (strpos($form_id, '_node_form')) {
    $form['#validate'][] = 'node_spam_scan';
  }
}

/**
 * Scan node content before it is posted into the database.
 */
function node_spam_scan($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Save')) {
    $node = $form['#post'];
    $node['type'] = $form['type']['#value'];
    $_SESSION['spam_form'] = $form;
    spam_scan($node, 'node');
  }
  // spam_form is used if we catch spam in spam_scan, we can now free it
  if (isset($_SESSION['spam_form'])) {
    unset($_SESSION['spam_form']);
  }
}

/**
* Implementation of a Drupal action.
* Mark node as spam.
*/
function spam_mark_node_as_spam_action(&$object, $context = array()) {
  // get the nid from the object
  if (isset($object->nid)) {
    $nid = $object->nid;
  }
  elseif (isset($context['nid'])) {
    $nid = $context['nid'];
  }
  // make sure we have a node record
  if ($nid) {
    spam_mark_as_spam('node', $nid);
    // record a message noting the action taken
    watchdog('action', 'Marked node %nid as spam.', array('%nid' => $nid) );
  }
}

/**
* Implementation of a Drupal action.
* Mark node as not spam.
*/
function spam_mark_node_as_not_spam_action(&$object, $context = array()) {
  // get the nid from the object
  if (isset($object->nid)) {
    $nid = $object->nid;
  }
  elseif (isset($context['nid'])) {
    $nid = $context['nid'];
  }
  // make sure we have a node record
  if ($nid) {
    spam_mark_as_not_spam('node', $nid);
    // record a message noting the action taken
    watchdog('action', 'Marked node %nid as not spam.', array('%nid' => $nid) );
  }
}

<?php

/**
 * @file
 * Include file for integration with the user system.
 */

/**
 *
 */
function spam_user($type, &$edit, &$user, $category = NULL) {
  switch ($type) {
    case 'update':
      $user = spam_content_user_profile_update($user);
      return spam_content_update($user, 'user');
    case 'insert':
      $user = spam_content_user_profile_update($user);
      return spam_content_insert($user, 'user');
    case 'delete':
      return spam_content_delete($user, 'user');
    case 'view':
      $links = '';
      if (user_spamapi('filter_content_type', $user) && user_access('administer spam')) {
        foreach (spam_links('user', $user->uid, $user) as $link) {
          if ($link['href']) {
            $links .= l($link['title'], $link['href'], array('query' => array_merge($link['query'], array('destination' => 'user/'. $user->uid)))) .' ';
          }
          else {
            $links .= $link['title'] .' ';
          }
        }
        $status = db_result(db_query('SELECT status FROM {users} WHERE uid = %d', $user->uid));
        $status_text = t('User status: %status', array('%status' => $status ? t('not blocked') : t('blocked')));
        $user->content['summary']['spam_status'] = array(
          '#type' => 'user_profile_item',
          '#weight' => '10',
          '#title' => 'Spam status',
          '#value' => $links . ' ' . $status_text,
        );
      }
      break;
  }
}

/**
 * Cache the user id to be sure it's available when we need it.
 */
function _spam_content_user_uid($id) {
  static $uid = 0;

  if (isset($id) && is_numeric($id)) {
    $uid = $id;
  }

  return $uid;
}

/**
 * User module _spamapi() hook.
 */
function user_spamapi($op, $arg1 = NULL, $arg2 = NULL, $arg3 = NULL) {
  switch ($op) {
    case 'content_module':
      // Register with the spam api as a content type module.
      return 'user';

    case 'content_id':
      if (is_object($arg1)) {
        $arg1 = (array)$arg1;
      }
      return _spam_content_user_uid($arg1['uid']);

    case 'content_types':
      // Register the "user" content type with the spam module.
      return array(array(
        'name' => 'users',
        'module' => 'user',
        'title' => t('Users'),
        'description' => t('Check this box to filter users for spam.'),
        'default_value' => 0,
      ));

    case 'process_form':
      // Hook to scan user before it is inserted into the database.
      if ($arg1 == 'user_register' && is_array($arg2)) {
        $user = $arg2['#post'];
        if (is_array($user) && $user['op'] == t('Create new account')) {
          $_SESSION['spam_form'] = $arg2;
          spam_scan($user, 'user');
        }
        // spam_form is used if we catch spam in spam_scan, we can now free it
        if (isset($_SESSION['spam_form'])) {
          unset($_SESSION['spam_form']);
        }
      }
      break;

    case 'filter_content_type':
      return (variable_get('spam_filter_users', 0));

    case 'filter_fields':
      // Be sure we're always working with an array.
      if (is_object($arg1)) {
        $arg1 = (array)$arg1;
      }
      // Determine uid so we can cache fields in static.
      if (isset($arg1['uid'])) {
        $uid = $arg1['uid'];
      }
      else {
        $uid = 0;
      }
      // Only figure out filter_fields once per user per page load.
      static $fields = array();
      if (!isset($fields[$uid])) {
        $fields[$uid]['main'] = array('name', 'mail');
        $fields[$uid]['other'] = array();
        if (isset($arg1['signature'])) {
          $fields[$uid]['other'][] = 'signature';
        }
        if (db_table_exists('profile_fields')) {
          $result = db_query('SELECT name FROM {profile_fields}');
          while ($profile = db_fetch_object($result)) {
            if (isset($arg1[$profile->name])) {
              $fields[$uid]['other'][] = $profile->name;
            }
          }
        }
      }
      return $fields[$uid];

    case 'redirect':
      if (is_numeric($arg1)) {
        return drupal_goto("user/$arg1");
      }
      break;

    case 'load':
      if (is_numeric($arg1)) {
        return user_load(array('uid' => $arg1));
      }
      break;

    case 'title':
      return db_result(db_query('SELECT name FROM {users} WHERE uid = %d', $arg1));

    case 'edit_link':
      return "user/$arg1/edit";

    case 'status':
      $status = db_result(db_query('SELECT status FROM {users} WHERE uid = %d', $arg1));
      if ($status == 1) {
        return SPAM_PUBLISHED;
      }
      else {
        return SPAM_NOT_PUBLISHED;
      }

    case 'overview_filter_join':
      return 'INNER JOIN {users} u ON t.content_id = CAST(u.uid AS CHAR(32))';

    case 'overview_filter_where':
      switch ($arg1) {
        case 'title':
          return "u.name LIKE '%%%s%%'";
        case 'status':
          return "u.status != %d";
      }

    case 'feedback_filter_join_field':
      return 'u.uid';

    case 'publish':
      // TODO: When un/publish user, should probably also un/publish content
      if (is_numeric($arg1)) {
        module_invoke('user', 'user_operations_unblock', array($arg1));
      }
      break;

    case 'unpublish':
      // TODO: When un/publish user, should probably also un/publish content
      if (is_numeric($arg1)) {
        module_invoke('user', 'user_operations_block', array($arg1));
      }
      break;

    case 'feedback_form':
      $form = array();
      if (is_numeric($form['uid'])) {
        $form['uid'] = array(
          '#type' => 'textfield',
          '#title' => t('User ID'),
          '#value' => $arg1['uid'],
          '#disabled' => TRUE,
        );
      }
      // fall through...
    case 'error_form':
      if (!is_array($form)) {
        $form = array();
      }
      $form['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Username'),
        '#value' => $arg1['name'],
        '#disabled' => TRUE,
      );
      $form['mail'] = array(
        '#type' => 'textfield',
        '#title' => t('Email address'),
        '#value' => $arg1['mail'],
        '#disabled' => TRUE,
      );
      // TODO: Show profile fields.
      return $form;
  }
}

/**
 * Be sure we're scanning the latest profile data.
 */
function spam_content_user_profile_update($user) {
  if (module_exists('profile')) {
    profile_load_profile($user);
  }
  return $user;
}

/**
 * Form alter gets its own function so we can reference &$form without causing
 * errors in PHP4 installations.  (If we use spamapi, we have to set a default,
 * which PHP4 doesn't support.)
 */
function user_spamapi_form_alter(&$form, &$form_state, $form_id) {
  // Scan users before they are inserted into the database
  if ($form_id == 'user_register' && is_array($form)) {
    $form['#validate'][] = 'user_spam_scan';
  }
}

/**
 * Scan user before it is created in the database.
 */
function user_spam_scan($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Create new account')) {
    $account = $form['#post'];
    $_SESSION['spam_form'] = $form;
    spam_scan($account, 'user');
  }
  // spam_form is used if we catch spam in spam_scan, we can now free it
  if (isset($_SESSION['spam_form'])) {
    unset($_SESSION['spam_form']);
  }
}

/**
* Implementation of a Drupal action.
* Mark user as spam.
*/
function spam_mark_user_as_spam_action(&$object, $context = array()) {
  // get the uid from the object
  if (isset($object->uid)) {
    $uid = $object->uid;
  }
  elseif (isset($context['uid'])) {
    $uid = $context['uid'];
  }
  // make sure we have a user record
  if ($uid) {
    spam_mark_as_spam('user', $uid);
    // record a message noting the action taken
    watchdog('action', 'Marked user %uid as not spam.', array('%uid' => $uid) );
  }
}

/**
* Implementation of a Drupal action.
* Mark user as not spam.
*/
function spam_mark_user_as_not_spam_action(&$object, $context = array()) {
  // get the uid from the object
  if (isset($object->uid)) {
    $uid = $object->uid;
  }
  elseif (isset($context['uid'])) {
    $uid = $context['uid'];
  }
  // make sure we have a comment record
  if ($uid) {
    spam_mark_as_not_spam('user', $uid);
    // record a message noting the action taken
    watchdog('action', 'Marked user %uid as not spam.', array('%uid' => $uid) );
  }
}

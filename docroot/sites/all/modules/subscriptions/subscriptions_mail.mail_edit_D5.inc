<?php
// $Id: subscriptions_mail.mail_edit_D5.inc,v 1.1 2008/12/25 12:15:34 salvis Exp $

/**
 * @file
 * Subscriptions module mail gateway helper functions for upgrading mail_edit D5 to D6.
 */

/**
 * Implementation of hook_form_alter().
 *
 * Adds to the General Settings part at admin/settings/subscriptions.
 */
function _subscriptions_mail_form_mail_edit_list_form_alter(&$form, $form_state) {
  $form['subscriptions'] = array(
      '#type' => 'fieldset',
      '#title' => t('!Subscriptions module', array('!Subscriptions' => 'Subscriptions')),
      '#weight' => -5,
  );
  $form['subscriptions']['explain'] = array(
    '#type'          => 'item',
    '#value'         => t('You have !Subscriptions mail templates that were created under Drupal 5. Please import or remove these templates.', array('!Subscriptions' => 'Subscriptions')),
  );
  $lang_query = "SELECT DISTINCT(language) FROM {mail_edit} ORDER BY language ASC";
  $lang_result = db_query($lang_query);
  $mail_langs = array();//fetch all current translations
  while($row = db_fetch_object($lang_result)) {
    $mail_langs[$row->language] = $row->language;
  }
  foreach (language_list() as $lang_key => $language) {
    $mail_langs[$lang_key] = $lang_key;
  }//<!-- fetch all current translations
  
  if (count($mail_langs) > 1) {
    $form['subscriptions']['languages'] = array(
      '#type'          => 'select',
      '#title'         => t('Language(s)'),
      '#default_value' => array(),
      '#options'       => $mail_langs,
      '#description'   => t('Select all the language(s) into which to import the Drupal 5 !Subscriptions mail templates.<br />Note: After successful importing, the old templates are removed automatically.', array('!Subscriptions' => 'Subscriptions')),
      '#multiple'      => TRUE,
    );
  }
  else {
    $form['subscriptions']['language'] = array(
      '#type'          => 'value',
      '#value'         => current($mail_langs),
    );
  }
  $form['subscriptions']['import'] = array(
    '#type'          => 'submit',
    '#value'         => t('Import'),
  );
  $form['subscriptions']['remove'] = array(
    '#type'          => 'submit',
    '#value'         => t('Remove'),
  );
  $form['#validate'][] = 'subscriptions_mail_mail_edit_list_form_validate';
  $form['#submit'][] = 'subscriptions_mail_mail_edit_list_form_submit';
}

function subscriptions_mail_mail_edit_list_form_validate($form, &$form_state) {
  if ($form_state['values']['op'] == $form_state['values']['import']) {
    if (!isset($form_state['values']['language']) && empty($form_state['values']['languages'])) {
      $form['subscriptions']['languages']['#error'] = TRUE;
      form_set_error('languages', t('Select at least one language to import into.'));
    }
  }
  return $form;
}

function subscriptions_mail_mail_edit_list_form_submit($form, &$form_state) {
  $languages = (isset($form_state['values']['language']) ? array($form_state['values']['language']) : $form_state['values']['languages']);
  
  switch ($form_state['values']['op']) {
      
    case $form_state['values']['remove']:
      if (db_table_exists('subscriptions_mail_edit')) {
        db_query("DELETE FROM {subscriptions_mail_edit}");
      }
      if (db_table_exists('mail_edit_d5')) {
        db_query("DELETE FROM {mail_edit_d5} WHERE mailkey LIKE 'subscriptions-%'");
      }
      break;
  
    case $form_state['values']['import']:    
      // Import digest records:
      $old_mailkey = 'subscriptions-digest';
      $new_mailkey = 'subscriptions_mail_digest';
      $result = db_query("SELECT * FROM {mail_edit_registry} WHERE id = '%s'", $new_mailkey);
      if ($registry = db_fetch_array($result)) {
        if (_subscriptions_mail_mail_edit_import($old_mailkey, $new_mailkey, $registry['description'], $languages)) {
          $digest_item = db_result(db_query("SELECT item_body FROM {subscriptions_mail_edit} WHERE mailkey = '%s'", $old_mailkey .'-item'));
          $digest_separator = db_result(db_query("SELECT item_body FROM {subscriptions_mail_edit} WHERE mailkey = '%s'", $old_mailkey .'-separator'));
          $digest_comment = db_result(db_query("SELECT item_body FROM {subscriptions_mail_edit} WHERE mailkey = '%s'", $old_mailkey .'-item-comment'));
          foreach ($languages as $language) {
              db_query("INSERT INTO {mail_edit} (description, subject, id, body, language) VALUES ('%s', '%s', '%s', '%s', '%s')", '', $digest_separator, $new_mailkey .'+item', $digest_item, $language);
              db_query("INSERT INTO {mail_edit} (description, subject, id, body, language) VALUES ('%s', '%s', '%s', '%s', '%s')", '', '', $new_mailkey .'+comment', $digest_comment, $language);
          }
          db_query("DELETE FROM {subscriptions_mail_edit} WHERE mailkey LIKE '%s'", $new_mailkey .'-%');
        }
      }
      
      // Import node/nid (+ comment) records:
      $result = db_query("SELECT * FROM {mail_edit_registry} WHERE id = '%s'", $new_mailkey);
      if ($registry = db_fetch_array($result)) {
        if (_subscriptions_mail_mail_edit_import('subscriptions-node-nid', 'subscriptions_content_node-nid', $registry['description'], $languages)) {
          $comment_item = db_result(db_query("SELECT item_body FROM {subscriptions_mail_edit} WHERE mailkey = 'subscriptions-comment-item'"));
          $comment_separator = db_result(db_query("SELECT item_body FROM {subscriptions_mail_edit} WHERE mailkey = 'subscriptions-comment-separator'"));
          foreach ($languages as $language) {
            db_query("INSERT INTO {mail_edit} (description, subject, id, body, language) VALUES ('%s', '%s', '%s', '%s', '%s')", '', $comment_separator, 'subscriptions_content_node-nid+comment', $comment_item, $language);
          }
          db_query("DELETE FROM {subscriptions_mail_edit} WHERE mailkey LIKE 'subscriptions-comment-%'");
        }
      }
      
      // Import node/tid records:
      $result = db_query("SELECT * FROM {mail_edit_registry} WHERE id = '%s'", $new_mailkey);
      if ($registry = db_fetch_array($result)) {
        _subscriptions_mail_mail_edit_import('subscriptions-node-tid', 'subscriptions_taxonomy_node-tid', $registry['description'], $languages);
      }
      
      // Import node-type-* records:
      $old_mailkey = 'subscriptions-node-type-';
      $new_mailkey = 'subscriptions_content_node-type-';
      $result = db_query("SELECT * FROM {mail_edit_registry} WHERE id LIKE '%s'", $new_mailkey .'%');
      while ($registry = db_fetch_array($result)) {
        $type = substr($registry['id'], strlen($new_mailkey));
        _subscriptions_mail_mail_edit_import($old_mailkey . $type, $new_mailkey . $type, $registry['description'], $languages);
      }
      break;
  }

  $dummy = array();
  if (db_table_exists('subscriptions_mail_edit') && db_result(db_query("SELECT COUNT(*) FROM {subscriptions_mail_edit}")) == 0) {
    db_drop_table($dummy, 'subscriptions_mail_edit');
  }
  if (db_table_exists('mail_edit_d5') && db_result(db_query("SELECT COUNT(*) FROM {mail_edit_d5}")) == 0) {
    db_drop_table($dummy, 'mail_edit_d5');
  }
}

function _subscriptions_mail_mail_edit_import($old_mailkey, $new_mailkey, $description, $languages) {
  $success = FALSE;
  if ($row = db_fetch_array(db_query("SELECT * FROM {mail_edit_d5} WHERE mailkey = '%s'", $old_mailkey))) {
    if (empty($row['description'])) {
      $row['description'] = filter_xss($description, array());
    }
    $success = TRUE;
    foreach ($languages as $language) {
      $variables = array('%key' => $new_mailkey, '%language' => $language);
      if (db_result(db_query("SELECT COUNT(*) FROM {mail_edit} WHERE id = '%s' AND language = '%s'", $new_mailkey, $language))) {
        drupal_set_message(t("%language translation of %key cannot be imported because it's already defined.", $variables), 'warning');
        $success = FALSE;
      }
      elseif (db_query("INSERT INTO {mail_edit} (description, subject, id, body, language) VALUES ('%s', '%s', '%s', '%s', '%s')", $row['description'], $row['subject'], $new_mailkey, $row['body'], $language) && db_affected_rows()) {
        drupal_set_message(t('%language translation of %key has been imported.', $variables));
      }
      else {
        drupal_set_message(t('%language translation of %key failed to import.', $variables), 'error');
        $success = FALSE;
      }
    }
    if ($success) {
      db_query("DELETE FROM {mail_edit_d5} WHERE mailkey = '%s'", $old_mailkey);
    }
  }
  return $success;
}
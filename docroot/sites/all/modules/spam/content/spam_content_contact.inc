<?php

/**
 * @file
 * Include file for integration with the contact form.
 */


/**
 * Spam module _spamapi() hook.
 */
function contact_spamapi($op, $arg1 = NULL, $arg2 = NULL, $arg3 = NULL) {
  switch ($op) {
    case 'content_module':
      // Register with the spam api as a content type module.
      return 'contact';

    case 'content_id':
      static $id = 0;

      if ($id === 0) {
        // contact messages do not have real IDs so we fake one.
        $id = drupal_get_token(md5(uniqid(mt_rand(), true)));
      }

      return $id;

    case 'content_types':
      $types = array();
      $types[] = array(
        'name' => 'contact_mail_user',
        'module' => 'contact',
        'title' => t('Personal contact form'),
        'description' => t('Allow users to contact each other by e-mail via their personal contact form.'),
        'default_value' => 0,
      );
      $types[] = array(
        'name' => 'contact_mail_page',
        'module' => 'contact',
        'title' => t('Site-wide contact form'),
        'description' => t('Allow users to contact predefined addressees through the site-wide contact form.'),
        'default_value' => 0,
      );

      return $types;

    case 'filter_content_type':
      // need to return the variable value of ie spam_filter_contact_user or spam_filter_contact_page
      return variable_get('spam_filter_'. $arg1['type'], 0);

    case 'filter_fields':
      // Tell spam filter which fields should be scanned for spam.
      $fields['main'] = array('subject', 'message');
      return $fields;

    case 'feedback_form':
      $form = array();
      // fall through...
    case 'error_form':
      if (!is_array($form)) {
        $form = array();
      }
      $form['contact'] = array(
        '#type' => 'fieldset',
        '#title' => $type,
      );
      $form['contact']['subject'] = array(
        '#type' => 'textfield',
        '#title' => t('Subject'),
        '#value' => $arg1['subject'],
        '#disabled' => TRUE,
      );
      $form['contact']['message'] = array(
        '#type' => 'textarea',
        '#title' => t('Message'),
        '#value' => $arg1['message'],
        '#disabled' => TRUE,
      );
      return $form;

    case 'feedback_approved':
      // the mail was already sent by the "publish" action
      return TRUE;
    case 'publish':
      global $language, $user;
      // we "publish" the contact message, ie we send it.
      $contact_message = db_fetch_object(db_query("SELECT * FROM {spam_filters_errors} WHERE content_id = '%s'", $arg1));
      $content = unserialize($contact_message->content);
      $type = $content['type'];
      $form_state = unserialize($contact_message->form);

      $account = user_load($contact_message->uid);

      // We need to switch languages and user
      $current_language = $language->language;
      $language->language = $contact_message->language;

      $current_user = $user;
      $old_state = session_save_session();
      session_save_session(FALSE);
      $user = $account;
      require_once drupal_get_path('module', 'contact') . '/contact.pages.inc';

      switch ($type) {
        case 'contact_mail_page':
          contact_mail_page_submit(array(), $form_state);
          break;
        case 'contact_mail_user':
          contact_mail_user_submit(array(), $form_state);
          break;
      }

      $language->language = $current_language;
      $user = $current_user;
      session_save_session($old_state);

      // The contact module increments the flood counter for the current IP
      // address. We try to delete this again since it would affect the admin
      // publishing contact mails, not the spammer.
      db_query("DELETE FROM {flood} WHERE event = 'contact' AND hostname = '%s' AND timestamp = %d", ip_address(), time());
      return TRUE;
      break;

    case 'load':
      return '';
      
    case 'title':
      if($name = db_result(db_query("SELECT u.name FROM {spam_filters_errors} sfe LEFT JOIN users u ON u.uid = sfe.uid  WHERE sfe.content_id = '%s'", $arg1))) {
        return t('Contact mail from !account', array('!account' => $name));
      }
      else {
        return t('Contact mail from !account or no feedback.', array('!account' => variable_get('anonymous', t('Anonymous'))));
      }
      break;

    case 'status':
      //db_result(db_query("SELECT u.name FROM {spam_filters_errors} sfe WHERE sfe.content_id = '%s'", $arg1));
      return '';

    case 'edit_link':
      if($uid = db_result(db_query("SELECT u.uid FROM {spam_filters_errors} sfe LEFT JOIN users u ON u.uid = sfe.uid  WHERE sfe.content_id = '%s'", $arg1))) {
        return "uid/$uid/edit";
      }
      else {
        return FALSE;
      }
      break;

    case 'link':
      break;

    case 'hostname':
      return db_result(db_query("SELECT hostname FROM {spam_filters_errors} WHERE content_id = '%s'", $arg1));

    case 'redirect':
      return drupal_goto("contact");

    case 'overview_filter_join':
      // This doesn't work for contact mails
      break;

    case 'overview_filter_where':
      // This doesn't work for contact mails
      break;

    case 'unpublish':
      // for contact mails "unpublish" is not well defined, we treat it as "hold".
    case 'hold':
      spam_content_insert($arg2, 'contact');
      // We auto-generate an entry for the error page
      spam_feedback_insert(t('This feedback is automatically generated.'));
      if ($op == 'hold') {
        // We fake the message from contact.module
        drupal_set_message(t('Your message has been sent.'));
      }
      // unpublish
      else {
        drupal_set_message(t('Your message will be reviewed by a site administrator.'));
      }
      // We redirect to the fp to discard the actual mail.
      drupal_goto();
      break;
  }
}

/**
 * Form alter gets its own function so we can reference &$form without causing
 * errors in PHP4 installations.  (If we use spamapi, we have to set a default,
 * which PHP4 doesn't support.)
 */
function contact_spamapi_form_alter(&$form, &$form_state, $form_id) {
  switch ($form_id) {
    case 'contact_mail_user':
      if (empty($form['#validate'])) {
      	$form['#validate'] = array();
      }
      $form['#validate'] = array('contact_spam_scan') + $form['#validate'];
      break;
    case 'contact_mail_page':
      if (empty($form['#validate'])) {
      	$form['#validate'] = array();
      }
      $form['#validate'] = array('contact_spam_scan') + $form['#validate'];
      break;
  }
}

/**
 * Scan contact form content before it is sent to users.
 */
function contact_spam_scan($form, &$form_state) {
  $contact = $form['#post'];
  $contact['type'] = $form['form_id']['#value'];
  $_SESSION['spam_form'] = $form_state;
  spam_scan($contact, 'contact');
  // spam_form is used if we catch spam in spam_scan, we can now free it
  if (isset($_SESSION['spam_form'])) {
    unset($_SESSION['spam_form']);
  }
}


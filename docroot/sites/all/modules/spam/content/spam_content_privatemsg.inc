<?php

/**
 * @file
 * Include file for integration with the privatemsg module.
 */

//
// The following doesn't work at this time.
// I'm commenting it out so it doesn't generate any more side effects and often
// people don't think of deleting files (tarballs have their limits.)
// We'll see whether this works, if you have a problem with private messages,
// delete the file altogether. It will most probably be deleted in the next
// version since at this point the code doesn't work and no one is offering
// to fix it.
//

// /**
//  * Spam module _spamapi() hook.
//  */
// function privatemsg_spamapi($op, $arg1 = NULL, $arg2 = NULL, $arg3 = NULL) {
//   switch ($op) {
//     case 'content_module':
//       // Register with the spam api as a content type module.
//       return 'privatemsg';
// 
//     case 'content_id':
//       static $id = 0;
// 
//       if ($id === 0) {
//         // private messages do not have real IDs so we fake one.
//         $id = drupal_get_token(md5(uniqid(mt_rand(), true)));
//       }
// 
//       return $id;
// 
//     case 'content_types':
//       $types = array();
//       $types[] = array(
//         'name' => 'privatemsg',
//         'module' => 'privatemsg',
//         'title' => t('Privatemsg'),
//         'description' => t("Filter private messages."),
//         'default_value' => 0,
//       );
// 
//       return $types;
// 
//     case 'filter_content_type':
//       return variable_get('spam_filter_privatemsg', FALSE);
// 
//     case 'filter_fields':
//       $fields['main'] = array('subject', 'body');
//       return $fields;
// 
//     case 'feedback_form':
//       $form = array();
//       // fall through...
//     case 'error_form':
//       if (!is_array($form)) {
//         $form = array();
//       }
//       $form['privatemsg'] = array(
//         '#type' => 'fieldset',
//         '#title' => $type,
//       );
//       $form['privatemsg']['recipient'] = array(
//         '#type' => 'textfield',
//         '#title' => t('Recipient'),
//         '#value' => $arg1['recipient'],
//         '#disabled' => TRUE,
//       );
//       $form['privatemsg']['subject'] = array(
//         '#type' => 'textfield',
//         '#title' => t('Subject'),
//         '#value' => $arg1['subject'],
//         '#disabled' => TRUE,
//       );
//       $form['privatemsg']['body'] = array(
//         '#type' => 'textarea',
//         '#title' => t('Message'),
//         '#value' => $arg1['body'],
//         '#disabled' => TRUE,
//       );
//       return $form;
// 
//     case 'mark_as_not_spam':
//       break;
//     case 'feedback_approved':
//       // the mail was already sent by the "publish" action
//       return TRUE;
//     case 'publish':
//       global $language, $user;
//       // we "publish" the private message, ie we send it.
//       $message = db_fetch_object(db_query("SELECT * FROM {spam_filters_errors} WHERE content_id = '%s'", $arg1));
//       $content = unserialize($message->content);
//       $type = $content['type'];
//       $form_state = unserialize($message->form);
// 
//       $account = $form_state['values']['author'];
// 
//       // We need to switch languages and user
//       $current_language = $language->language;
//       $language->language = $message->language;
// 
//       $current_user = $user;
//       $old_state = session_save_session();
//       session_save_session(FALSE);
//       $user = $account;
//       pm_send(array(), $form_state);
// 
//       $language->language = $current_language;
//       $user = $current_user;
//       session_save_session($old_state);
// 
//       return TRUE;
//       break;
// 
//     case 'load':
//       break;
//     case 'title':
//       if($name = db_result(db_query("SELECT u.name FROM {spam_filters_errors} sfe LEFT JOIN users u ON u.uid = sfe.uid  WHERE sfe.content_id = '%s'", $arg1))) {
//         return t('Private message from !account', array('!account' => $name));
//       }
//       else {
//         return t('Private message from !account or no feedback.', array('!account' => variable_get('anonymous', t('Anonymous'))));
//       }
//       break;
// 
//     case 'status':
//     case 'edit_link':
//       if($uid = db_result(db_query("SELECT u.uid FROM {spam_filters_errors} sfe LEFT JOIN users u ON u.uid = sfe.uid  WHERE sfe.content_id = '%s'", $arg1))) {
//         return "uid/$uid/edit";
//       }
//       else {
//         return FALSE;
//       }
//       break;
// 
//     case 'link':
//     case 'hostname':
//       return db_result(db_query("SELECT hostname FROM {spam_filters_errors} WHERE content_id = '%s'", $arg1));
// 
//     case 'redirect':
//     case 'overview_filter_join':
//     case 'overview_filter_where':
//     case 'unpublish':
//       // for private messages "unpublish" is not well defined, we treat it as "hold".
//     case 'hold':
//       $content = unserialize($_SESSION['spam_content']);
//       spam_content_insert($content, 'privatemsg');
//       // We auto-generate an entry for the error page
//       spam_feedback_insert(t('This feedback is automatically generated.'));
//       // We fake the message from privatemsg.module
//       drupal_set_message(t('A message has been sent.'));
//       // We redirect to the fp to discard the actual mail.
//       drupal_goto();
//       break;
//   }
// }
// 
// /**
//  * Form alter gets its own function so we can reference &$form without causing
//  * errors in PHP4 installations.  (If we use spamapi, we have to set a default,
//  * which PHP4 doesn't support.)
//  */
// function privatemsg_spamapi_form_alter(&$form, &$form_state, $form_id) {
//   switch ($form_id) {
//     case 'privatemsg_new':
//       switch (variable_get('spam_visitor_action', SPAM_ACTION_PREVENT)) {
//         case SPAM_ACTION_PREVENT_SILENT:
//         case SPAM_ACTION_PREVENT:
//           /*
//           $form['privatemsg']['submit']['#validate'] = array('privatemsg_spam_scan') + $form['privatemsg']['submit']['#validate'];
//           break;
//           */
//         case SPAM_ACTION_HOLD:
//         case SPAM_ACTION_UNPUBLISH:
//           $form['privatemsg']['submit']['#submit'] = array('privatemsg_spam_scan') + $form['privatemsg']['submit']['#submit'];
//           break;
//       }
//       break;
//   }
// }
// 
// /**
//  * Scan privatemsg form content before it is sent to users.
//  */
// function privatemsg_spam_scan($form, &$form_state) {
//   $privatemsg = $form['#post'];
//   $privatemsg['type'] = $form['form_id']['#value'];
//   $_SESSION['spam_form'] = $form_state;
//   spam_scan($privatemsg, 'privatemsg');
//   // spam_form is used if we catch spam in spam_scan, we can now free it
//   if (isset($_SESSION['spam_form'])) {
//     unset($_SESSION['spam_form']);
//   }
// }


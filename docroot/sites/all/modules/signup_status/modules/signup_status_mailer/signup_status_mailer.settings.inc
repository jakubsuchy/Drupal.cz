<?php
// $Id: signup_status_mailer.settings.inc,v 1.7 2009/08/23 16:02:32 dww Exp $

/**
 * @file
 * Code for the various settings pages to define e-mail templates.
 */


/**
 * Form builder for signup_status_mailer settings.
 */
function signup_status_mailer_settings($form_state, $node = 0) {
  drupal_add_js(drupal_get_path('module', 'signup_status_mailer') . '/signup_status_mailer.settings.js');
  $codes = signup_status_codes(array(), TRUE);
  $form = array('#tree' => TRUE);
  $site_settings = signup_status_mailer_get_settings(0);
  if (empty($node)) {
    $nid = 0;
  }
  else {
    $nid = $node->nid;
    $node_settings = signup_status_mailer_get_settings($nid);
  }

  // Add some anchor links for browsers without JavaScript enabled.
  foreach ($codes as $cid => $status) {
    $anchors[] = l(t("Skip to settings for status '!status'", array('!status' => $status['name'])), $_GET['q'], array('fragment' => 'status-settings-' . drupal_strtolower(filter_xss($status['name']))));
  }
  $form['no-js-help'] = array(
    '#prefix' => '<div class="js-hide">',
    '#suffix' => '</div>',
    '#value' => theme('item_list', $anchors),
  );

  if (!module_exists('token') && empty($node)) {
    $form['token_help'] = array(
      '#type' => 'markup',
      '#value' => t('It is highly recommended that you install the <a href="@token_url">Token module</a> so that these messsages can include small placeholder strings that are dynamically converted to the appropriate values for the user signing up, the content they sign up for, and your current site configuration.', array('@token_url' => 'http://drupal.org/project/token')),
    );
  }

  if (empty($node)) {
    // If it's the site-wide settings form, add a fieldset for the admin
    // notifications.
    $form['admin_notifications'] = array(
      '#type' => 'fieldset',
      '#title' => t('Administrator notifications for signup status changes'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $notify_emails = variable_get('signup_status_mailer_admin_notify_emails', array());
    $form['admin_notifications']['notify_emails'] = array(
      '#type' => 'textarea',
      '#title' => t('E-mail addresses to notify for any signup status changes'),
      '#rows' => 4,
      '#cols' => 50,
      '#default_value' => implode("\n", $notify_emails),
      '#description' => t('Whenever a signup status is set or changed, your site can notify a list of users via e-mail. Put each address on a separate line. If blank, no e-mails will be sent.'),
    );
    $form['admin_notifications']['subject'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail subject'),
      '#default_value' => variable_get('signup_status_mailer_admin_notify_subject', ''),
    );
    $form['admin_notifications']['body'] = array(
      '#type' => 'textarea',
      '#title' => t('E-mail body'),
      '#cols' => 50,
      '#rows' => 6,
      '#default_value' => variable_get('signup_status_mailer_admin_notify_body', ''),
    );
    // Add a validation callback to make sure the e-mails are valid.
    $form['#validate'][] = 'signup_status_mailer_settings_validate';
  }

  foreach ($codes as $cid => $status) {
    $site_setting = isset($site_settings[$cid]) ? $site_settings[$cid] : array();
    $node_setting = isset($node_settings[$cid]) ? $node_settings[$cid] : array();
    $form["signup_status_mailer_{$cid}"] = _signup_status_mailer_settings_form($status, $node, $site_setting, $node_setting);
  }

  if (module_exists('token')) {
    $form['token_help'] = array(
      '#type' => 'fieldset',
      '#title' => t('Replacement patterns for all message templates'),
      '#description' =>  t("Since these tokens will be used in plain text e-mail, it is better to use the '-raw' versions of any tokens that provide them."),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['token_help']['help'] = array(
      '#type' => 'markup',
      '#value' => theme('token_help', array('node', 'user', 'signup', 'global')),
    );
  }

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $nid,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['#submit'][] = 'signup_status_mailer_settings_submit';

  return $form;
}

/**
 * Private helper to generate a mailer settings fieldset for a nid/cid pair.
 *
 * @param $status
 *   Array of values describing the Signup status to create settings for.
 * @param $node
 *   The fully-loaded node object we're generating settings for, or NULL if
 *   we're rendering the site-wide settings page.
 * @param $site_setting
 *   Array of the current site-wide defaults for this status.
 * @param $node_setting
 *   Array of the current per-node settings for this status, or NULL if we're
 *   generating the site-wide settings page.
 *
 * @return
 *   FAPI array with the appropriate form elements for the given nid/cid pair.
 */
function _signup_status_mailer_settings_form($status, $node, $site_setting = array(), $node_setting = array()) {

  $form = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attributes' => array('id' => 'status-settings-' . drupal_strtolower(filter_xss($status['name']))),
  );
  if (empty($node)) {
    // Site-wide defaults.
    $form['#title'] = t("Default notification settings for status '@status'", array('@status' => $status['name']));

    // On the site-wide settings, the notification options are always the same.
    $notification_options = array(
      'none' => t('Disabled'),
      'default' => t('Enabled'),
    );
  }
  else {
    // Per-node settings.
    $form['#title'] = t("Notification settings for status '@status'", array('@status' => $status['name']));
  }

  // Save the status CID as a value in the form.
  $form['cid'] = array(
    '#type' => 'value',
    '#value' => $status['cid'],
  );

  foreach (array('add', 'update') as $action) {
    $form[$action] = array(
      '#prefix' => '<div class="signup-status-mailer-action">',
      '#suffix' => '</div>',
    );

    if (!empty($node)) {
      // Figure out the appropriate options for the notifications setting.
      $notification_options = array(
        'none' => t('Disabled'),
        'custom' => t('Enabled (custom)'),
      );
      if (!empty($site_setting[$action]['notification']) && $site_setting[$action]['notification'] != 'none') {
        $current_default = 'default-enabled';
        $notification_options['default-enabled'] = t("Use default (currently 'Enabled')");
      }
      else {
        $current_default = 'default-disabled';
        $notification_options['default-disabled'] = t("Use default (currently 'Disabled')");
      }
    }

    $form[$action]['notification'] = array(
      '#type' => 'select',
      '#options' => $notification_options,
      '#attributes' => array('class' => 'signup_status_mailer_notification_select'),
    );
    if ($action == 'add') {
      $form[$action]['notification']['#title'] = t("E-mail notifications when the status '@status' is first set", array('@status' => $status['name']));
    }
    elseif ($action == 'update') {
      $form[$action]['notification']['#title'] = t("E-mail notifications when an existing status is changed to '@status'", array('@status' => $status['name']));
    }

    $form[$action]['email'] = array(
      '#weight' => 1,
    );
    $form[$action]['email']['subject'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail subject'),
    );
    $form[$action]['email']['body'] = array(
      '#type' => 'textarea',
      '#title' => t('E-mail body'),
      '#cols' => 50,
      '#rows' => 6,
    );

    // Now, figure out the default values to use for all of this.
    if (!empty($node)) {
      // Per-node settings
      if (isset($node_setting[$action]['notification']) && $node_setting[$action]['notification'] != 'default') {
        $default_notification = $node_setting[$action]['notification'];
      }
      else {
        $default_notification = $current_default;
      }

      // If we're on a node settings page, and we're configured to use the
      // site defaults, include them as disabled form elements for info, with
      // help text explaining how to change it.
      if ($default_notification == $current_default) {
        $default_subject = isset($site_setting[$action]['subject']) ? $site_setting[$action]['subject'] : '';
        $default_body = isset($site_setting[$action]['body']) ? $site_setting[$action]['body'] : '';
        $form[$action]['email']['subject']['#disabled'] = TRUE;
        $form[$action]['email']['body']['#disabled'] = TRUE;
      }
      else {
        // Using per-node values, if any.
        $default_subject = isset($node_setting[$action]['subject']) ? $node_setting[$action]['subject'] : '';
        $default_body = isset($node_setting[$action]['body']) ? $node_setting[$action]['body'] : '';
      }

      if (user_access(SIGNUP_STATUS_MAILER_ADMIN_PERM)) {
        if ($current_default == 'default-disabled') {
          $description = t('This message is configured to use the site-wide default, which is currently set to not send an e-mail. To send a message and specify the subject and body just for this %node_type, select "Enabled (custom)" to override the defaults. To send a message and define the subject and body for all status notifications, visit the <a href="@settings_url">site-wide Signup status mailer settings page</a>.', array('%node_type' => node_get_types('name', $node->type), '@settings_url' => url('admin/settings/signup_status/mailer', array('query' => drupal_get_destination()))));
        }
        else {
          $description = t('This message is configured to use the site-wide default text for this status. To modify the subject or body just for this %node_type, select "Enabled (custom)" to override the defaults. To change the subject or body for all status notifications, visit the <a href="@settings_url">site-wide Signup status mailer settings page</a>.', array('%node_type' => node_get_types('name', $node->type), '@settings_url' => url('admin/settings/signup_status/mailer', array('query' => drupal_get_destination()))));
        }
      }
      else {
        if ($current_default == 'default-disabled') {
          $description = t('This message is configured to use the site-wide default, which is currently set to not send an e-mail. To send a message and specify the subject and body, select "Enabled (custom)" to override the defaults for this %node_type.', array('%node_type' => node_get_types('name', $node->type)));
        }
        else {
          $description = t('This message is configured to use the site-wide default text for this status. To modify the subject or body below, select "Enabled (custom)" to override the defaults for this %node_type.', array('%node_type' => node_get_types('name', $node->type)));
        }
      }
      $description_classes = 'notification-description description';
      if ($default_notification == 'none' || $default_notification == 'custom') {
        $description_classes .= ' js-hide';
      }
      $form[$action]['notification-description'] = array(
        '#type' => 'markup',
        '#value' => '<div class="'. $description_classes . '">' . $description . '</div>',
      );
    }
    else {
      // Site-wide settings
      if (isset($site_setting[$action]['notification'])) {
        $default_notification = $site_setting[$action]['notification'];
      }
      else {
        $default_notification = 'none';
      }
      $default_subject = isset($site_setting[$action]['subject']) ? $site_setting[$action]['subject'] : '';
      $default_body = isset($site_setting[$action]['body']) ? $site_setting[$action]['body'] : '';
    }

    $email_classes = 'email-settings';
    // If the notification for this status/action is currently configured not
    // to send any notification, hide the email fields on page load.
    if ($default_notification == 'none' || $default_notification == 'default-disabled') {
      $email_classes .= ' js-hide';
    }
    $form[$action]['email']['#prefix'] = '<div class="'. $email_classes .'">';
    $form[$action]['email']['#suffix'] = '</div>';

    // Finally load our defaults into the form.
    $form[$action]['notification']['#default_value'] = $default_notification;
    $form[$action]['email']['subject']['#default_value'] = $default_subject;
    $form[$action]['email']['body']['#default_value'] = $default_body;
  }

  return $form;
}

/**
 * Validate callback for a signup_status_mailer settings form.
 *
 * This is only invoked for the site-wide defaults settings form to validate
 * the administrator notification e-mail settings. If there are any invalid
 * email addresses, the form element is marked with a form error. If they're
 * all valid, the array of valid addresses is stashed in $form_state so that
 * the submit handler can just save those directly instead of re-computing.
 *
 * This function is based heavily on update_settings_validate() from core
 * (also written by Derek "dww" Wright).
 *
 * @see signup_status_mailer_settings()
 * @see signup_status_mailer_settings_submit()
 * @see update_settings_validate()
 */
function signup_status_mailer_settings_validate($form, &$form_state) {
  if (!empty($form_state['values']['admin_notifications']['notify_emails'])) {
    $valid = array();
    $invalid = array();
    foreach (explode("\n", trim($form_state['values']['admin_notifications']['notify_emails'])) as $email) {
      $email = trim($email);
      if (!empty($email)) {
        if (valid_email_address($email)) {
          $valid[] = $email;
        }
        else {
          $invalid[] = $email;
        }
      }
    }
    if (empty($invalid)) {
      $form_state['signup_status_mailer_admin_notify_emails'] = $valid;
    }
    elseif (count($invalid) == 1) {
      form_set_error('admin_notifications][notify_emails', t('%email is not a valid e-mail address.', array('%email' => reset($invalid))));
    }
    else {
      form_set_error('admin_notifications][notify_emails', t('%emails are not valid e-mail addresses.', array('%emails' => implode(', ', $invalid))));
    }
  }
}

/**
 * Submit callback for a signup_status_mailer settings form.
 *
 * Saves the notification and message template settings for each status and
 * action in the form.
 *
 * If this is the site-wide defaults form, the administrator notification
 * settings are also saved. In that case, an array of valid e-mail addresses
 * will already be stashed in $form_state since those are already computed
 * during validation and there's no need to recompute them.
 *
 * @see signup_status_mailer_settings_validate()
 */
function signup_status_mailer_settings_submit($form, $form_state) {
  foreach ($form_state['values'] as $key => $status_values) {
    if (substr($key, 0, 21) == 'signup_status_mailer_') {
      foreach (array('add', 'update') as $action) {
        if (!empty($status_values[$action])) {
          $values = $status_values[$action];
          $values['action'] = $action;
          if (strpos($values['notification'], 'default-')) {
            $values['notification'] = 'default';
          }
          $values['cid'] = $status_values['cid'];
          $values['nid'] = $form_state['values']['nid'];
          if ($values['nid'] == 0) {
            // For the site-wide defaults, we always want to write whatever
            // text they gave us to the database, even if it's not currently
            // configured to be used.
            $save_text = TRUE;
          }
          else {
            // Only write the text to the DB if we're saving a custom setting.
            $save_text = ($values['notification'] == 'custom');
          }
          _signup_status_mailer_save_settings($values, $save_text);
        }
      }
    }
  }
  // See if the admin notification settings were defined.
  if (!empty($form_state['values']['admin_notifications'])) {
    if (empty($form_state['signup_status_mailer_admin_notify_emails'])) {
      variable_del('signup_status_mailer_admin_notify_emails');
    }
    else {
      variable_set('signup_status_mailer_admin_notify_emails', $form_state['signup_status_mailer_admin_notify_emails']);
    }
    foreach (array('subject', 'body') as $setting) {
      if (!empty($form_state['values']['admin_notifications'][$setting])) {
        variable_set("signup_status_mailer_admin_notify_$setting", $form_state['values']['admin_notifications'][$setting]);
      }
      else {
        variable_del("signup_status_mailer_admin_notify_$setting");
      }
    }
  }
}

/**
 * Private helper to update a record in {signup_satus_mailer_settings}
 *
 * @param $values
 *   Array of values to save.
 * @param $include_text
 *   Boolean indicating if the e-mail text should be written to the DB or not.
 *
 * @return
 *   TRUE if a new record was inserted, FALSE if we updated an existing record.
 */
function _signup_status_mailer_save_settings($values, $include_text = TRUE) {
  $is_new = FALSE;
  if ($include_text) {
    db_query("UPDATE {signup_status_mailer_settings} SET notification = '%s', body = '%s', subject = '%s' WHERE nid = %d AND cid = %d AND action = '%s'", $values['notification'], $values['email']['body'], $values['email']['subject'], $values['nid'], $values['cid'], $values['action']);
    if (!db_affected_rows()) {
      db_query("INSERT INTO {signup_status_mailer_settings} (nid, cid, action, notification, body, subject) VALUES (%d, %d, '%s', '%s', '%s', '%s')", $values['nid'], $values['cid'], $values['action'], $values['notification'], $values['email']['body'], $values['email']['subject']);
      $is_new = TRUE;
    }
  }
  else {
    db_query("UPDATE {signup_status_mailer_settings} SET notification = '%s' WHERE nid = %d AND cid = %d AND action = '%s'", $values['notification'], $values['nid'], $values['cid'], $values['action']);
    if (!db_affected_rows()) {
      db_query("INSERT INTO {signup_status_mailer_settings} (nid, cid, action, notification) VALUES (%d, %d, '%s', '%s')", $values['nid'], $values['cid'], $values['action'], $values['notification']);
      $is_new = TRUE;
    }
  }
  return $is_new;
}


<?php


/**
 * @file
 * contains all admin pages, settings, and validate.
 *
 * This file will only be parsed when processing admin menu options.
 * See http://drupal.org/node/146172
 */


/**
 * Settings form for menu callback
 */
function autologout_admin_settings() {
  _autologout_debug("autologout_settings()");

  if (!user_access('administer autologout')) {
    drupal_access_denied();
    return;
  }

  if (module_exists('jstimer')) {
    if (!module_exists('jst_timer')) {
      drupal_set_message(t('The "Widget: timer" module must also be enabled for the dynamic countdown to work in the automated logout block.'), 'error');
    }

    if (variable_get('jstimer_js_load_option', 0) != 1) {
      drupal_set_message(t('The Javascript timer module\'s "Javascript load options" setting should be set to "Every page" for the dynamic countdown to work in the automated logout block.'), 'error');
    }
  }

  $form = array();
  $form['autologout'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Automated Logout settings'),
    '#tree'  => TRUE
  );
  $form['autologout']['markup1'] = array(
    '#type'  => 'markup',
    '#value' => theme('autologout_generic', 1),
  );
  $form['autologout']['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable module'),
    '#default_value' => _autologout_local_settings('enabled'),
    '#description' => t('Uncheck this and save settings to disable Automated Logout')
  );
  $form['autologout']['timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Timeout value in seconds'),
    '#default_value' => _autologout_local_settings('timeout'),
    '#size' => 10,
    '#maxlength' => 12,
    '#description' => t('The length of inactivity time, in seconds, before Automated Logout.  Must be 60 seconds or greater.')
  );
  $form['autologout']['use_watchdog'] = array(
   '#type' => 'checkbox',
   '#title' => t('Enable watchdog Automated Logout logging'),
   '#default_value' => _autologout_local_settings('use_watchdog'),
   '#description' => t('Enable logging of automatically logged out users')
  );
  $form['autologout']['markup2'] = array(
    '#type' => 'markup',
    '#value' => theme('autologout_generic', 0),
  );

  foreach (user_roles(TRUE) as $role) {
    $form['autologout'][$role] = array(
      '#type' => 'select',
      '#title' => $role,
      '#options' => array('0' => t('Enforce'), '1' => t('Exclude'), '2' => t('By user')),
      '#default_value' => _autologout_local_settings($role)
    );
  }

  $markup3_items = array(
    t('Enforce : all users in this role will be auto-logged out'),
    t('Exclude : all users in this role are excluded from auto-logout functionality'),
    t('By user : all users in this role can select to switch off this functionality')
  );
  $markup3 = theme('item_list', $markup3_items, t('Policy description')) . t('Please note: If a user is found to be in a role that allows disabling this feature, this overrides any enforcement');
  $form['autologout']['markup3'] = array(
   '#type' => 'markup',
   '#value' => $markup3
  );

  $form['autologout']['logout_message']  = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#title' => t('Display a message at logout'),
    '#default_value' => _autologout_local_settings('logout_message'),
    '#size' => 40,
    '#description' => t('If you want to display a message to the users when they are logged out.  This setting can contain html if needed.')
  );

  $form['autologout']['redirect_url']  = array(
    '#type' => 'textfield',
    '#title' => t('Redirect URL at logout'),
    '#default_value' => _autologout_local_settings('redirect_url'),
    '#size' => 40,
    '#description' => t('If you want to redirect users to another page when they are logged out. (default: autologout/logout)')
  );

  $form['autologout']['refresh_delta'] = array(
    '#type' => 'textfield',
    '#title' => t('Browser refresh delay'),
    '#default_value' => _autologout_local_settings('refresh_delta'),
    '#size' => 10,
    '#maxlength' => 12,
    '#description' => t("The length of time, in seconds, after a timeout that a browser refresh is forced. Setting this to -1 (negative number) disables the browser refresh facility entirely."),
  );

  $form['block_settings'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Autologout warning block')
  );

  $form['block_settings']['block_title']  = array(
    '#type' => 'textfield',
    '#title' => t('Block title'),
    '#default_value' => _autologout_local_settings('block_title'),
    '#size' => 40,
    '#description' => t('The title of the logout warning block.')
  );

  if (module_exists('jstimer') && module_exists('jst_timer')) {
    $form['block_settings']['autologout_jstimer_format']  = array(
      '#title' => t('Javascript timer output format string'),
      '#type' => 'textarea',
      '#rows' => 2,
      '#default_value' => variable_get('autologout_jstimer_format', '%hours%:%mins%:%secs%'),
      '#description' => t('Change the display of the dynamic timer.  Available replacement values are: %day%, %month%, %year%, %dow%, %moy%, %years%, %ydays%, %days%, %hours%, %mins%, and %secs%.')
    );
  }

  $form['autologout_one_session'] = array(
    '#type'  => 'fieldset',
    '#title' => t('One Session settings'),
    '#tree'  => TRUE,
    '#description' => t('One session will provide enforcement for "one session" per user. If a user logs in when another session for that user is active the other session will be deactivated. The user will be notified and the event logged.'),
  );
  $values = variable_get('autologout_one_session', 0);

  foreach (user_roles(TRUE) as $role) {
    $this_value = $values[$role];

    $form['autologout_one_session'][$role] = array(
      '#type' => 'select',
      '#title' => $role,
      '#options' => array('1' => t('Enforce'), '0' => t('Exclude')),
      '#default_value' => (isset($values[$role]) ? $values[$role] : 0),
    );
  }
  $markup4_items = array(
    t('Enforce : all users in this role will be only allowed one session.'),
    t('Exclude : all users in this role are excluded from one session functionality.'),
  );
  $markup4 = theme('item_list', $markup4_items, t('Policy description')) . t('Please note: If a user is found to be in a role that allows disabling this feature, this overrides any enforcement');
  $form['autologout_one_session']['markup4'] = array(
   '#type' => 'markup',
   '#value' => $markup4,
  );
  return system_settings_form($form);
}


function autologout_admin_settings_validate($form, $form_state) {
  if ( $form_state['values']['autologout']['timeout'] < 60 ) {
    form_set_error('autologout][timeout', t('You must specify a timeout value 60 seconds or greater'));
  }
}

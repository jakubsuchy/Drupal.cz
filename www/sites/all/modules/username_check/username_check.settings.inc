<?php
// $Id: username_check.settings.inc,v 1.4 2009/01/22 14:10:03 duke Exp $

/**
 * @file
 * Admin page callbacks for the username_check module.
 */

/**
 * Menu callback; displays the username_check module settings page.
 */
function username_check_settings() {
  $form = array();

  $form['username_check_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Checking mode'),
    '#options' => array(
      'auto'   => t('Automatic - executes when user leaves username field or upon timer'),
      'manual' => t('Manual - executes upon clicking a button')),
    '#default_value' => variable_get('username_check_mode', 'auto'),
  );

  $form['username_check_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Timer threshold'),
    '#description' => t('Threshold in seconds (ex: 0.5, 1). Only for automatic mode.'),
    '#default_value' => variable_get('username_check_delay', 1),
  );

  return system_settings_form($form);
}
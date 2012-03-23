<?php
// $Id: signup_status_notifications.admin.inc,v 1.1 2009/01/30 18:40:47 miglius Exp $

/**
 * @file
 * Module admin page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// signup status notifications settings

/**
 * Implements the settings page.
 *
 * @return
 *   The form structure.
 */
function signup_status_notifications_admin_settings() {
  $options = array();
  foreach (signup_status_codes() as $cid => $code) {
    $options[$cid] = $code['name'];
  }
  $form['notifications'] = array('#type' => 'fieldset', '#title' => t('Notification settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['notifications']['signup_status_notifications_statuses'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Status codes'),
    '#description' => t('Select the signup status codes that will trigger an automatic subscription to node update notifications on signup.'),
    '#options' => $options,
    '#default_value' => variable_get('signup_status_notifications_statuses', array()),
  );
  $form['notifications']['signup_status_notifications_delete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete subscription on the sign up cancel'),
    '#description' => t('Should a subscription to a node be deleted if the sign up is canceled.'),
    '#default_value' => SIGNUP_STATUS_NOTIFICATIONS_DELETE,
  );

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['reset'] = array(
    '#type'  => 'submit',
    '#value' => t('Reset to defaults'),
  );
  return $form;
}

/**
 * Submit hook for the settings form.
 */
function signup_status_notifications_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  $values = $form_state['values'];
  switch ($op) {
    case t('Save configuration'):
      variable_set('signup_status_notifications_statuses', array_filter($values['signup_status_notifications_statuses']));
      variable_set('signup_status_notifications_delete', $values['signup_status_notifications_delete']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case t('Reset to defaults'):
      variable_del('signup_status_notifications_statuses');
      variable_del('signup_status_notifications_delete');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}


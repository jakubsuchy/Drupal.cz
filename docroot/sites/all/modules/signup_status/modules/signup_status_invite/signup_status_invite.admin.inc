<?php
// $Id: signup_status_invite.admin.inc,v 1.1 2009/01/30 18:44:47 miglius Exp $

/**
 * @file
 * Module admin page callbacks.
 */

//////////////////////////////////////////////////////////////////////////////
// signup status invite settings

/**
 * Implements the settings page.
 *
 * @return
 *   The form structure.
 */
function signup_status_invite_admin_settings() {
  $options = array();
  if (SIGNUP_STATUS_INVITE_STATUS == 0) {
    $options[0] = t('-- Select code --');
  }
  foreach (signup_status_codes() as $cid => $code) {
    $options[$cid] = $code['name'];
  }
  $form['general'] = array('#type' => 'fieldset', '#title' => t('General settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['general']['signup_status_invite_status'] = array(
    '#type' => 'select',
    '#title' => t('Status code'),
    '#description' => t('Select a status code invited users will be signuped as.'),
    '#options' => $options,
    '#default_value' => SIGNUP_STATUS_INVITE_STATUS,
  );

  $form['invite'] = array('#type' => 'fieldset', '#title' => t('Invite email'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['invite']['signup_status_invite_mail_invite_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject of invite e-mail'),
    '#maxlength' => 180,
    '#default_value' => _signup_status_invite_mail_text('invite_subject'),
    '#description' => t('Customize the subject of your invite e-mail message.') .' '. t('Available variables are:') .' !site, !name, !mailto, !date, !title, !signup_url.',
  );
  $form['invite']['signup_status_invite_mail_invite_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body of invite e-mail'),
    '#rows' => 15,
    '#default_value' => _signup_status_invite_mail_text('invite_body'),
    '#description' => t('Customize the body of your invite e-mail message.') .' '. t('Available variables are:') .' !site, !name, !mailto, !date, !title, !signup_url.',
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
 * Validate hook for the settings form.
 */
function signup_status_invite_admin_settings_validate($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case t('Save configuration'):
      $values = $form_state['values'];
      if ($values['signup_status_invite_status'] == 0) {
        form_set_error('signup_status_invite_status', t('Select a status code.'));
      }
      break;
  }
}

/**
 * Submit hook for the settings form.
 */
function signup_status_invite_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case t('Save configuration'):
      $values = $form_state['values'];
      variable_set('signup_status_invite_status', $values['signup_status_invite_status']);
      variable_set('signup_status_invite_mail_invite_subject', $values['signup_status_invite_mail_invite_subject']);
      variable_set('signup_status_invite_mail_invite_body', $values['signup_status_invite_mail_invite_body']);
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case t('Reset to defaults'):
      $values = $form_state['values'];
      variable_del('signup_status_invite_status');
      variable_del('signup_status_invite_mail_invite_subject');
      variable_del('signup_status_invite_mail_invite_body');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}


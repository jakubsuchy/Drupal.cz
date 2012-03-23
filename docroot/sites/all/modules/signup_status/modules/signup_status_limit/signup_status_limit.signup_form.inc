<?php
// $Id: signup_status_limit.signup_form.inc,v 1.2 2009/09/18 19:55:07 dww Exp $

/**
 * @file
 * Code to alter signup forms (new and edit) to enforce per-status limits.
 */


/**
 * Alter the signup form to enforce per-status limits.
 *
 * @see signup_status_limit_alter_signup_status_element()
 */
function signup_status_limit_alter_signup_form(&$form, $form_state) {
  $node = node_load($form['nid']['#value']);
  // Only try to alter the signup form under the following conditions:
  // a) The node is configured to use per-status signup limits
  // b) There are any status options visible on the form
  // c) There's no "Username" field (e.g. at node/N/signups/add) -- admins
  //    aren't subject to status limits.
  if (!empty($node->signup_status_limit_type) && $node->signup_status_limit_type == 'status' && !empty($form['collapse']['signup_status']) && empty($form['collapse']['signup_user_form']['signup_username'])) {
    $form['#validate'][] = 'signup_status_limit_validate_signup_form';
    signup_status_limit_alter_signup_status_element($form['collapse']['signup_status'], $form_state, $node);
    // See if there are any valid signup status options left, and if not,
    // alter the form to just print a message telling the user that signups
    // are full.
    if (count($form['collapse']['signup_status']['#options']) <= 1) {
      foreach (element_children($form['collapse']) as $key) {
        unset($form['collapse'][$key]);
      }
      $form['collapse']['error'] = array(
        '#type' => 'item',
        '#value' => t('Signups are currently full for this %node_type.', array('%node_type' => node_get_types('name', $node->type))),
      );
    }
  }
}

/**
 * Alter the signup edit form to enforce per-status limits.
 *
 * @see signup_status_limit_alter_signup_status_element()
 */
function signup_status_limit_alter_signup_edit_form(&$form, $form_state) {
  $node = node_load($form['#signup']->nid);
  if (!empty($node->signup_status_limit_type) && $node->signup_status_limit_type == 'status' && !empty($form['elements']['signup_status'])) {
    signup_status_limit_alter_signup_status_element($form['elements']['signup_status'], $form_state, $node);
    $form['#validate'][] = 'signup_status_limit_validate_signup_form';
  }
}

/**
 * Helper function to alter the status element to enforce per-status limits.
 *
 * @param $element
 *   Reference to the form subarray that defines the signup status element.
 * @param $node
 *   The node that the signup form is being rendered for.
 *
 * @see signup_status_limit_alter_signup_form()
 * @see signup_status_limit_alter_signup_edit_form()
 */
function signup_status_limit_alter_signup_status_element(&$element, $form_state, $node) {
  if (!empty($element['#default_value'])) {
    $current_status = $element['#default_value'];
  }
  // We also want to see if a value has been submitted via form_state, so that
  // we don't unset an option that was valid when the form was first rendered,
  // even if it is now full. Doing so would generate the "An illegal choice
  // has been detected..." message, which would be confusing to users.
  elseif (!empty($form_state['post']['signup_status'])) {
    $current_status = $form_state['post']['signup_status'];
  }
  else {
    $current_status = 0;
  }

  foreach ($element['#options'] as $key => $value) {
    if ($key == $current_status) {
      // Ignore the status that the user is currently using. If they're trying
      // to signup using a now-full status, we'll catch it during validation.
      continue;
    }
    $limit = !empty($node->signup_status_limit[$key]) ? $node->signup_status_limit[$key] : array();
    if (!empty($limit['limit']) && $limit['total'] >= $limit['limit']) {
      unset($element['#options'][$key]);
    }
  }
}

function signup_status_limit_validate_signup_form(&$form, &$form_state) {
  if (empty($form_state['values']['signup_status'])) {
    drupal_set_message(t('The selected signup status is currently full (the limit was reached).'), 'error');
  }
  else {
    $status = $form_state['values']['signup_status'];
    $node = node_load($form_state['values']['nid']);
    $limit = !empty($node->signup_status_limit[$status]) ? $node->signup_status_limit[$status] : array();
    if (!empty($limit['limit']) && $limit['total'] >= $limit['limit']) {
      form_set_error('signup_status', t('The selected signup status is currently full (the limit was reached). Please select a different status.'));
    }
  }
}


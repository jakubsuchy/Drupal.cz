<?php
// $Id: signup_status_limit.settings.inc,v 1.3 2009/09/17 00:08:09 dww Exp $

/**
 * @file
 * Code to alter signup settings forms to add per-status limits.
 */


/**
 * Alter the form for the per-node signup settings page.
 */
function signup_status_limit_alter_signup_node_settings_form(&$form) {
  // Ensure the buttons are at the bottom of the page.
  $form['buttons']['#weight'] = 50;

  // Add our own validation and submit handlers.
  $form['#validate'][] = 'signup_status_limit_node_settings_validate';
  $form['#submit'][] = 'signup_status_limit_node_settings_submit';

  // Call our shared helper to actually alter the form elements. In this case,
  // everything lives in the root of the $form array, so we can pass the whole
  // thing.
  signup_status_limit_alter_signup_node_settings(&$form, $form['#node']);
}

/**
 * Alter the form for the side-wide signup settings page.
 */
function signup_status_limit_alter_signup_site_settings_form(&$form) {
  // Add our own validation and submit handlers.
  $form['#validate'][] = 'signup_status_limit_node_settings_validate';
  $form['#submit'][] = 'signup_status_limit_node_settings_submit';

  // By default, this form doesn't define a #node, so we need to add one.
  $query = db_query("SELECT nid, close_signup_limit as signup_close_signup_limit FROM {signup} WHERE nid = %d", 0);
  $node = db_fetch_object($query);
  // Add the signup_status_limit properties to this pseudo-node object using
  // our nodeapi 'load' function since it's exactly what we want.
  signup_status_limit_nodeapi_load($node);
  $form['#node'] = $node;
  
  // Call our shared helper to actually alter the form elements. In this case,
  // everything we're altering has been moved inside a fieldset, so we in
  // thing. Since we're passing a subtree of the $form array, the helper
  // wouldn't be able to find $form['#node'] so we pass the object directly
  // (although the submit handler is going to still be able to use (and will
  // need) $form['#node']).
  signup_status_limit_alter_signup_node_settings($form['node_defaults']['signup_node_settings_form'], $node);
}

/**
 * Alter the form elements for node-specific signup settings.
 *
 * This is a shared helper function since all of this logic is the same for
 * both the per-node settings pages at node/N/signups/settings and the
 * site-wide defaults at admin/settings/signup, although some of the other
 * form altering details are different in the two cases.
 *
 * @param $form
 *   The form array (or subtree) containing the elements to alter.
 * @param $node
 *   An object containing the existing signup-related settings saved to the
 *   DB. This can either be a real $node object or a pseudo object with the
 *   same layout in the case of the site-wide defaults page (where settings
 *   are stored to various tables using nid 0).
 *
 * @see signup_status_limit_alter_signup_node_settings_form()
 * @see signup_status_limit_alter_signup_site_settings_form()
 */
function signup_status_limit_alter_signup_node_settings(&$form, $node) {
  drupal_add_js(drupal_get_path('module', 'signup_status_limit') . '/signup_status_limit.settings.js');

  // Figure out the current defaults.
  $limit_type = db_result(db_query("SELECT limit_type FROM {signup_status_limit_node_setting} WHERE nid = %d", $node->nid));
  if (empty($limit_type)) {
    $limit_type = 'global';
  }
  $global_limit = $node->signup_close_signup_limit;

  $form['signup_status_limit_type'] = array(
    '#title' => t('Signup limit type'),
    '#type' => 'radios',
    '#default_value' => $limit_type,
    '#options' => array(
      'global' => t('Limit on total signups'),
      'status' => t('Separate limits for each signup status'),
    ),
    '#description' => '<div class="js-hide">' . t('Signups can either be limited by the total number of signups, or there can be separate limits for each signup status. Depending on which of these options is selected, one of the following sets of text boxes will be used to define the actual limits.') . '</div>',
    '#prefix' => '<div class="signup-limit-radios">',
    '#suffix' => '</div>',
    '#weight' => 5,
  );

  // Move the global limit field lower on the form, give it a better title,
  // and provide the necessary divs for the JS to work properly.
  $form['signup_close_signup_limit']['#weight'] = 6;
  $form['signup_close_signup_limit']['#title'] = t('Limit on total signups');
  $classes = 'signup_global_limit';
  if ($limit_type != 'global') {
    $classes .= ' js-hide';
  }
  $form['signup_close_signup_limit']['#prefix'] = '<div class="' . $classes . '">';
  $form['signup_close_signup_limit']['#suffix'] = '</div>';

  // Add the per-status limits table.
  $classes = 'signup_status_limit_node_limits';
  if ($limit_type != 'status') {
    $classes .= ' js-hide';
  }
  if (user_access(SIGNUP_STATUS_MANAGE_PERMISSION)) {
    $description = t('For each !signup_status defined on your site, you can control how many signups are allowed at that status. If set to 0, there is no limit for that status.', array('!signup_status' => l(t('Signup status'), 'admin/settings/signup_status')));
  }
  else {
    $description = t('For each Signup status defined on your site, you can control how many signups are allowed at that status. If set to 0, there is no limit for that status.');
  }
  $form['signup_status_limit_node_limits'] = array(
    '#theme' => empty($node->nid) ? 'signup_status_limit_site_limits' : 'signup_status_limit_node_limits',
    '#tree' => TRUE,
    '#type' => 'item',
    '#weight' => 7,
    '#prefix' => '<div class="' . $classes . '">',
    '#suffix' => '</div>',
    '#description' => $description,
  );
  foreach (signup_status_codes() as $cid => $code) {
    $form['signup_status_limit_node_limits'][$cid]['name'] = array(
      '#type' => 'markup',
      '#value' => check_plain($code['name']),
    );
    $form['signup_status_limit_node_limits'][$cid]['limit'] = array(
      '#type' => 'textfield',
      '#size' => 4,
      '#default_value' => isset($node->signup_status_limit[$cid]['limit']) ? $node->signup_status_limit[$cid]['limit'] : 0,
    );
    if (!empty($node->nid)) {
      // On the per-node form, we also want to display the current totals.
      $form['signup_status_limit_node_limits'][$cid]['total'] = array(
        '#type' => 'markup',
        '#value' => isset($node->signup_status_limit[$cid]['total']) ? $node->signup_status_limit[$cid]['total'] : 0,
      );
    }
  }
}

/**
 * Validate an altered signup settings form to verify limit settings.
 */
function signup_status_limit_node_settings_validate($form, $form_state) {
  foreach ($form_state['values']['signup_status_limit_node_limits'] as $cid => $value) {
    if (!is_numeric($value['limit']) || $value['limit'] < 0) {
      form_set_error("signup_status_limit_node_limits][$cid][limit", t('The limit must be a positive number'));
    }
  }
}

/**
 * Submit callback for altered signup settings form to handle limit settings.
 */
function signup_status_limit_node_settings_submit($form, $form_state) {
  $op = isset($form_state['values']['op']) ? $form_state['values']['op'] : '';

  // Regardless of the operation, submitting this form should clear any
  // existing per-status limits saved to the DB.  If we're saving a new
  // configuration, we want to write all the new values with a clean slate.
  db_query("DELETE FROM {signup_status_limit_node_limit} WHERE nid = %d", $form['#node']->nid);

  if ($op == t('Reset to defaults')) {
    // Ignore what's in the form, and save 'global' as the default.
    if (empty($form['#node']->nid)) {
      // Reset the global settings.
      $limit_type = 'global';
      // There are no default per-status limits, so leave $limits empty.
    }
    else {
      // Reset a node's settings to the current site-wide defaults.
      $limit_type = db_result(db_query("SELECT limit_type FROM {signup_status_limit_node_setting} WHERE nid = %d", 0));
      $query = db_query("SELECT cid, status_limit FROM {signup_status_limit_node_limit} WHERE nid = %d", 0);
      while ($limit = db_fetch_object($query)) {
        $limits[$limit->cid] = $limit->status_limit;
      }
    }
  }
  else {
    // Saving a new configuration from the form values.
    $limit_type = $form_state['values']['signup_status_limit_type'];

    // If the limit type is anything other than 'global', clear out the
    // current global limit.
    if ($limit_type != 'global') {
      db_query("UPDATE {signup} SET close_signup_limit = %d WHERE nid = %d", 0, $form['#node']->nid);
    }

    // Save all the new per-status limits defined in the form.
    foreach ($form_state['values']['signup_status_limit_node_limits'] as $cid => $value) {
      $limits[$cid] = $value['limit'];
    }
  }

  // Either way, we need to record the appropriate new settings.
  if (!empty($limits)) {
    foreach ($limits as $cid => $limit) {
      db_query("INSERT INTO {signup_status_limit_node_limit} (nid, cid, status_limit) VALUES (%d, %d, %d)", $form['#node']->nid, $cid, $limit);
    }
  }
  db_query("UPDATE {signup_status_limit_node_setting} SET limit_type = '%s' WHERE nid = %d", $limit_type, $form['#node']->nid);
  if (!db_affected_rows()) {
    // There might not be a record in {signup_status_limit_node_setting},
    // e.g. for signup-enabled nodes that existed before the
    // signup_status_limit module was enabled.
    db_query("INSERT INTO {signup_status_limit_node_setting} (nid, limit_type) VALUES (%d, '%s')", $form['#node']->nid, $limit_type);
  }

}

/**
 * Render the table of per-status limit settings on the per-node form.
 */
function theme_signup_status_limit_node_limits($form) {
  $header = array(
    t('Signup status'),
    t('Limit'),
    t('Current total'),
  );
  $rows = array();
  foreach (element_children($form) as $key) {
    $rows[] = array(
      drupal_render($form[$key]['name']),
      drupal_render($form[$key]['limit']),
      drupal_render($form[$key]['total']),
    );
  }
  return theme('table', $header, $rows);
}

/**
 * Render the table of per-status limit settings on the site-wide form.
 */
function theme_signup_status_limit_site_limits($form) {
  $header = array(
    t('Signup status'),
    t('Limit'),
  );
  $rows = array();
  foreach (element_children($form) as $key) {
    $rows[] = array(
      drupal_render($form[$key]['name']),
      drupal_render($form[$key]['limit']),
    );
  }
  return theme('table', $header, $rows);
}


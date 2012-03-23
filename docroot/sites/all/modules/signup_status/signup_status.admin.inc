<?php
// $Id: signup_status.admin.inc,v 1.7 2009/09/21 03:44:30 dww Exp $

/**
 * @file
 * Code related to the administrative (settings) pages for Signup Status.
 */

/**
 * Render the site-wide signup status settings form.
 */
function theme_signup_status_admin_settings_form($form) {
  drupal_add_tabledrag('signup-status-admin-settings-table', 'order', 'self', 'signup-status-weight');
  $header = array(
    array('data' => t('Name !required', array('!required' => '<span class="form-required" title="' . t('This field is required.') . '">*</span>'))),
    array('data' => t('Description')),
    array('data' => t('Modify signup count')),
    array('data' => t('Show on form')),
    array('data' => t('Weight')),
    array('data' => t('Operations'))
  );
  foreach (element_children($form['status']) as $key) {
    $row = array();
    foreach (array('name', 'description', 'mod_signup_count', 'show_on_form', 'weight', 'delete') as $element) {
      // Since we're rendering these in a table, remove any #title attributes.
      if (!empty($form['status'][$key][$element]['#title'])) {
        unset($form['status'][$key][$element]['#title']);
      }
      $row[] = drupal_render($form['status'][$key][$element]);
    }
    $rows[] = array(
      'class' => 'draggable',
      'data' => $row,
    );
  }
  $rows[] = array(
    'class' => 'draggable',
    'data' => array(
      drupal_render($form['status_add']['name']),
      drupal_render($form['status_add']['description']),
      drupal_render($form['status_add']['mod_signup_count']),
      drupal_render($form['status_add']['show_on_form']),
      drupal_render($form['status_add']['weight']),
      NULL,
    ),
  );
  $output = theme('table', $header, $rows, array('id' => 'signup-status-admin-settings-table'));
  $output .= drupal_render($form);
  return $output;
}

/**
 * Build the form for the site-wide signup_status settings table.
 *
 * @param $form_state
 *   A form state array.
 *
 * @return
 *   The form structured array.
 */
function signup_status_admin_settings_form(&$form_state) {
  $signup_status_codes = signup_status_codes();

  // Form elements for existing status codes.
  $form['status']['#tree'] = TRUE;
  if (!empty($signup_status_codes)) {
    foreach ($signup_status_codes as $cid => $status) {
      $form['status'][$cid]['name'] = array(
        '#title' => t('Name'),
        '#type' => 'textfield',
        '#default_value' => $status['name'],
        '#maxlength' => 128,
        '#size' => 20,
        '#required' => TRUE,
      );
      $form['status'][$cid]['weight'] = array(
        '#title' => t('Weight'),
        '#type' => 'weight',
        '#default_value' => $status['weight'],
        '#delta' => 15,
        '#attributes' => array('class' => 'signup-status-weight'),
      );
      $form['status'][$cid]['description'] = array(
        '#title' => t('Description'),
        '#type' => 'textfield',
        '#default_value' => $status['description'],
        '#maxlength' => 128,
        '#size' => 30,
      );
      $form['status'][$cid]['mod_signup_count'] = array(
        '#title' => t('Modify signup count'),
        '#type' => 'checkbox',
        '#default_value' => $status['mod_signup_count'],
      );
      $form['status'][$cid]['show_on_form'] = array(
        '#title' => t('Show on form'),
        '#type' => 'checkbox',
        '#default_value' => $status['show_on_form'],
      );
      $form['status'][$cid]['delete'] = array(
        '#type' => 'markup',
        '#value' => l(t('delete'), 'admin/settings/signup_status/delete/'. $cid),
      );
    }
  }

  // Form elements for adding a new status.
  $form['status_add']['#tree'] = TRUE;
  $form['status_add']['name'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#maxlength' => 128,
  );
  $form['status_add']['weight'] = array(
    '#type' => 'weight',
    '#default_value' => 0,
    '#delta' => 15,
    '#attributes' => array('class' => 'signup-status-weight'),
  );
  $form['status_add']['description'] = array(
    '#type' => 'textfield',
    '#maxlength' => 128,
    '#size' => 30,
  );
  $form['status_add']['mod_signup_count'] = array(
    '#type' => 'checkbox',
  );
  $form['status_add']['show_on_form'] = array(
    '#type' => 'checkbox',
  );

  $form['help'] = array(
    '#type' => 'item',
    '#description' => t("This table defines the signup status options available on this site. If the 'Modify signup count' box is checked, signups in that status will be counted towards the signup limit (if any). <strong>Note that changing this value once signups exist will update those signups and potentially open or close signups on the affected event(s).</strong> If the 'Show on form' box is checked, users will have the option of selecting the status when they signup or edit an existing signup. The order of statusus in this table determines the ordering choices on the signup form (if any are shown). The blank row at the bottom can be used to add a new status."),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Validation callback for the signup status code settings form.
 */
function signup_status_admin_settings_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $names = array();
  if (!empty($values['status'])) {
    foreach ($values['status'] as $cid => $status) {
      $name = $status['name'];
      if (!empty($names[$name])) {
        form_set_error('status][' . $cid . '][name', t('A status code with the name %name is already in use.', array('%name' => $name)));
      }
      else {
        $names[$name] = $cid;
      }
    }
  }
  if (!empty($values['status_add']['name'])) {
    $new_name = $values['status_add']['name'];
    if (!empty($names[$new_name])) {
      form_set_error('status_add][name', t('A status code with the name %name is already in use.', array('%name' => $new_name)));
    }
  }
  else {
    // Ensure that the name is defined if the user set anything else for the
    // new status code row.
    if (!empty($values['status_add']['description']) ||
        !empty($values['status_add']['mod_signup_count']) ||
        !empty($values['status_add']['show_on_form'])) {
      form_set_error('status_add][name', t('Name is required for a new status.'));
    }
  }
}

/**
 * Submit callback for the signup status code settings form.
 */
function signup_status_admin_settings_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  if (!empty($values['status_add']['name'])) {
    // Adding a new status.
    drupal_write_record('signup_status_codes', $values['status_add']);
    drupal_set_message(t('Status code %name has been added.', array('%name' => $values['status_add']['name'])));
    watchdog('signup_status', 'Status code %name has been added.', array('%name' => $values['status_add']['name']));
  }

  // See if we modified any existing status codes.
  if (!empty($values['status'])) {
    $count_changed_status_codes = array();
    $signup_status_codes = signup_status_codes();
    foreach ($values['status'] as $cid => $status) {
      $status['cid'] = $cid;
      $diff = array_diff_assoc($status, $signup_status_codes[$cid]);
      if (!empty($diff)) {
        if (isset($diff['mod_signup_count'])) {
          $count_changed_status_codes[] = $status;
        }
        drupal_write_record('signup_status_codes', $status, 'cid');
        drupal_set_message(t('Status code %name has been updated.', array('%name' => $status['name'])));
        watchdog('signup_status', 'Status code %name has been updated.', array('%name' => $status['name']));
      }
    }
    if (!empty($count_changed_status_codes)) {
      signup_status_admin_signup_count_change($count_changed_status_codes);
    }
  }
}

/**
 * Change the mod_signup_count and update signup totals via Batch API.
 *
 * @param $status_codes
 *   Array of signup status definitions (as returned by signup_status_codes())
 *   that have changed. The 'mod_signup_count' element in each subarray should
 *   hold the new value to change into.
 *
 * @see signup_status_codes()
 * @see signup_status_admin_signup_count_change_batch()
 * @see signup_status_admin_signup_count_change_finished()
 */
function signup_status_admin_signup_count_change($status_codes) {
  $title = t('Updating signup totals and checking limits for changes to if the signup status modifies the signup count');
  $batch = array(
    'operations' => array(
      array('signup_status_admin_signup_count_change_batch', array($status_codes)),
    ),
    'finished' => 'signup_status_admin_signup_count_change_finished',
    'title' => $title,
    'init_message' => $title,
    'progress_message' => t('Updating signup totals and checking limits...'),
    'error_message' => t('Error updating signup totals and checking limits.'),
    'file' => drupal_get_path('module', 'signup_status') . '/signup_status.admin.inc',
  );
  batch_set($batch);
}

/**
 * Batch worker to check signup totals based on changes mod_signup_count.
 *
 * @param $status_codes
 *   Array of signup status definitions (as returned by signup_status_codes())
 *   that have changed. The 'mod_signup_count' element in each subarray should
 *   hold the new value to change into.
 * @param $context
 *   Reference to a Batch API context array to track progress of the batch.
 *
 * @see signup_status_admin_signup_count_change()
 * @see signup_status_admin_signup_count_change_finished()
 */
function signup_status_admin_signup_count_change_batch($status_codes, &$context) {
  if (empty($context['sandbox']['nodes'])) {
    // Initialize the work to do -- remember all the nodes that have signups
    // in any effected status, since we need to modify the totals and check
    // limits.
    $status_cids = array();
    foreach ($status_codes as $code) {
      $status_cids[] = $code['cid'];
    }
    $placeholders = db_placeholders($status_cids);
    $query = db_query("SELECT nid FROM {signup_log} WHERE status IN ($placeholders) GROUP BY nid", $status_cids);
    while ($nid = db_result($query)) {
      $context['sandbox']['nodes'][] = $nid;
    }
    $context['finished'] = 0;
    $context['sandbox']['max'] = count($context['sandbox']['nodes']);
    $context['sandbox']['progress'] = 0;
    $context['message'] = t('Checking signup count and limits...');
    $context['results']['updated'] = 0;
    $context['results']['processed'] = 0;
    $context['results']['status_codes'] = $status_codes;

    // In case of a race condition, ensure there's something to process.
    if (empty($context['sandbox']['max'])) {
      $context['finished'] = 1;
      return;
    }
  }

  // If there's work to do, process a node.
  if (!empty($context['sandbox']['nodes'])) {
    $nid = array_pop($context['sandbox']['nodes']);
    // First update the signups for all affected status codes.
    foreach ($status_codes as $status) {
      db_query("UPDATE {signup_log} SET count_towards_limit = %d WHERE nid = %d AND status = %d", $status['mod_signup_count'], $nid, $status['cid']);
    }
    // Now, load the node, which will count and sum the effective signups.
    $node = node_load($nid);
    // Finally, check the limit now that the total has potentially changed.
    _signup_check_limit($node, 'total');
    $context['results']['updated']++;
    $context['message'] = t('Processed signups for %title', array('%title' => $node->title));
  }
  $context['sandbox']['progress']++;
  $context['results']['processed']++;
  $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
}

/**
 * Batch API callback once all signup totals have been modified.
 *
 * @see signup_status_admin_signup_count_change()
 * @see signup_status_admin_signup_count_change_batch()
 */
function signup_status_admin_signup_count_change_finished($success, $results, $operations) {
  if ($success) {
    if (!empty($results)) {
      drupal_set_message(format_plural($results['updated'], 'Updated signup totals on one node based on changes to signup counts.', 'Updated signup totals on @count nodes based on changes to signup counts.'));
      foreach ($results['status_codes'] as $status) {
        if ($status['mod_signup_count']) {
          drupal_set_message(t('Signup status %name now modifies the signup count.', array('%name' => $status['name'])));
        }
        else {
          drupal_set_message(t('Signup status %name now does not modify the signup count.', array('%name' => $status['name'])));
        }
      }
    }
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message(t('An error occurred while processing with arguments: @error', array('@error' => var_export($error_operation[0], TRUE))), 'error');
  }
}

/**
 * Implements the signup status code delete page.
 *
 * @param $form_state
 *   A form state array.
 * @param $cid
 *   A signup status ID.
 *
 * @return
 *   The form structure.
 */
function signup_status_admin_delete(&$form_state, $cid) {
  $codes = signup_status_codes();
  if (empty($codes[$cid])) {
    drupal_set_message(t('The status you are trying to delete does not exist.'), 'error');
    return drupal_goto('admin/settings/signup_status');
  }

  $name = $codes[$cid]['name'];
  $form['cid'] = array(
    '#type' => 'value',
    '#value' => $cid,
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $name,
  );

  $total = db_result(db_query('SELECT COUNT(sid) AS total FROM {signup_log} WHERE status = %d', $cid));
  if ($total > 0) {
    foreach ($codes as $id => $status) {
      $options[$id] = $status['name'];
    }
    $form['new_status'] = array(
      '#type' => 'select',
      '#title' => t('Reassign status'),
      '#default_value' => $cid,
      '#options' => $options,
      '#description' => t('There are !total existing signups with the status of @name. Please select a new status for these signups.', array('!total' => $total, '@name' => $name)),
    );
  }

  return confirm_form(
    $form,
    t('Are you sure you want to delete the status code %name?', array('%name' => $name)),
    'admin/settings/signup_status',
    t('This action cannot be undone.'),
    t('Delete status'),
    t('Cancel')
  );
}

/**
 * Validation callback for the signup status code delete page.
 */
function signup_status_admin_delete_validate($form, &$form_state) {
  if ($form_state['values']['new_status'] == $form_state['values']['cid']) {
    form_set_error('new_status', t('Choose a new signup status for existing signups of status %name.', array('%name' => $form_state['values']['name'])));
  }
}

/**
 * Submit callback for the signup status code delete page.
 */
function signup_status_admin_delete_submit($form, &$form_state) {
  $values = $form_state['values'];
  $codes = signup_status_codes();

  db_query("DELETE FROM {signup_status_codes} WHERE cid = %d", $values['cid']);
  drupal_set_message(t('Status code %name has been deleted.', array('%name' => $values['name'])));
  watchdog('signup_status', 'Status code %name has been deleted.', array('%name' => $values['name']));

  if (isset($values['new_status'])) {
    // We could update all of the signups to the new status in a single query,
    // but we need to invoke the hook that the status was changed, and that's
    // potentially expensive, so we do it via the Batch API.
    $old_status = $codes[$values['cid']];
    $new_status = $codes[$values['new_status']];
    signup_status_admin_reassign_status($old_status, $new_status);
  }
  $form_state['redirect'] ='admin/settings/signup_status';
}

/**
 * Reassign all signups from one status to another using the Batch API.
 *
 * @param $old_status
 *   Array defining the old status to assign from. This array must contain at
 *   least the 'cid' and 'name' keys, as returned by signup_status_codes().
 * @param $new_status
 *   Array defining the new status to assign to. This array must contain at
 *   least the 'cid' and 'name' keys, as returned by signup_status_codes().
 *
 * @see signup_status_codes()
 * @see signup_status_admin_reassign_status_batch()
 * @see signup_status_admin_reassign_status_finished()
 */
function signup_status_admin_reassign_status($old_status, $new_status) {
  $batch = array(
    'operations' => array(
      array('signup_status_admin_reassign_status_batch', array($old_status, $new_status)),
    ),
    'finished' => 'signup_status_admin_reassign_status_finished',
    'redirect' => 'admin/settings/signup_status',
    'title' => $title,
    'init_message' => $title,
    'progress_message' => t('Reassigning signups from %old_status status to %new_status status...', array('%old_status' => $old_status['name'], '%new_status' => $new_status['name'])),
    'error_message' => t('Error reassigning signups from %old_status status to %new_status status...', array('%old_status' => $old_status['name'], '%new_status' => $new_status['name'])),
    'file' => drupal_get_path('module', 'signup_status') . '/signup_status.admin.inc',
  );
  batch_set($batch);
}

/**
 * Batch worker to reassign signups from one status to another.
 *
 * @param $old_status
 *   Array defining the old status to assign from. This array must contain at
 *   least the 'cid' and 'name' keys, as returned by signup_status_codes().
 * @param $new_status
 *   Array defining the new status to assign to. This array must contain at
 *   least the 'cid' and 'name' keys, as returned by signup_status_codes().
 * @param $context
 *   Reference to a Batch API context array to track progress of the batch.
 *
 * @see signup_status_admin_reassign_status()
 * @see signup_status_admin_reassign_status_finished()
 */
function signup_status_admin_reassign_status_batch($old_status, $new_status, &$context) {
  if (empty($context['sandbox']['signups'])) {
    // Initialize the work to do -- remember all the signups we need to change
    // the status on. Also, if the new status and the old status have a
    // different value for 'mod_signup_count', we're going to need to process
    // all the unique nodes that have signups in the old status so that we can
    // properly update the effective total and check signup limits.
    $mod_signup_count_change = $old_status['mod_signup_count'] != $new_status['mod_signup_count'];
    $query = db_query('SELECT sid, nid FROM {signup_log} WHERE status = %d', $old_status['cid']);
    while ($signup = db_fetch_object($query)) {
      $context['sandbox']['signups'][] = $signup->sid;
      if ($mod_signup_count_change) {
        // Remember all the unique nodes to update totals and check limits on.
        $context['sandbox']['nodes'][$signup->nid] = $signup->nid;
      }
    }
    $context['finished'] = 0;
    $context['sandbox']['max'] = count($context['sandbox']['signups']);
    if ($mod_signup_count_change) {
      // If mod_signup_count changed, we also need to process each node.
      $context['sandbox']['max'] += count($context['sandbox']['nodes']);
    }
    $context['sandbox']['progress'] = 0;
    $context['message'] = t('Reassigning signups from %old_status status to %new_status status...', array('%old_status' => $old_status['name'], '%new_status' => $new_status['name']));
    $context['results']['updated'] = 0;
    $context['results']['processed'] = 0;
    $context['results']['old_status'] = $old_status;
    $context['results']['new_status'] = $new_status;

    // In case of a race condition, ensure there's something to process.
    if (empty($context['sandbox']['max'])) {
      $context['finished'] = 1;
      return;
    }
  }

  // If there's work to do, process a signup.
  if (!empty($context['sandbox']['signups'])) {
    $sid = array_pop($context['sandbox']['signups']);
    $signup = signup_load_signup($sid);
    $signup->old_status = $signup->status;
    $signup->status = $new_status['cid'];
    db_query("UPDATE {signup_log} SET status = %d WHERE sid = %d", $signup->status, $signup->sid);
    _signup_status_change('update', $signup);
    $context['results']['updated']++;
  }
  // If we're done processing signups, see if we need to process any nodes.
  elseif (!empty($context['sandbox']['nodes'])) {
    $nid = array_pop($context['sandbox']['nodes']);
    // Update the count_towards_limit column for all affected signups.
    db_query("UPDATE {signup_log} SET count_towards_limit = %d WHERE nid = %d AND status = %d AND count_towards_limit = %d", $new_status['mod_signup_count'], $nid, $new_status['cid'], $old_status['mod_signup_count']);
    // Now, load the node, which will count and sum the effective signups.
    $node = node_load($nid);
    // Finally, check the limit now that the total has potentially changed.
    _signup_check_limit($node, 'total');
    $context['results']['updated']++;
    $context['message'] = t('Processed signups for %title', array('%title' => $node->title));
  }
  $context['sandbox']['progress']++;
  $context['results']['processed']++;
  $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
}

/**
 * Batch API callback once all signups have been reassigned to a new status.
 *
 * @see signup_status_admin_reassign_status()
 * @see signup_status_admin_reassign_status_batch()
 */
function signup_status_admin_reassign_status_finished($success, $results, $operations) {
  if ($success) {
    if (!empty($results)) {
      drupal_set_message(format_plural($results['updated'], 'Reassigned one signup from the %old_status status to the %new_status status.', 'Reassigned @count signups from the %old_status status to the %new_status status.', array('%old_status' => $results['old_status']['name'], '%new_status' => $results['new_status']['name'])));
    }
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message(t('An error occurred while processing with arguments: @error', array('@error' => var_export($error_operation[0], TRUE))), 'error');
  }
}


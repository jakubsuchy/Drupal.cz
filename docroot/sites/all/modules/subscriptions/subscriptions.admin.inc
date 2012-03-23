<?php
// $Id: subscriptions.admin.inc,v 1.17 2008/10/26 23:25:43 salvis Exp $

/**
 * @file
 * Subscriptions module (load-on-demand admin functions).
 */

/**
 * Site Settings form at admin/settings/subscriptions.
 * Other submodules contribute additional parts to this form.
 */
function subscriptions_settings_form(&$form_state) {
  global $user;
  $form = array();
  $tr = 't';

  $form['links'] = array(
    '#type' => 'fieldset',
    '#title' => t('Links'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => -100,
  );
  $form['links'][] = array(
    '#value' => '<div>'. l($tr('Permissions'), 'admin/user/permissions', array('fragment' => 'module-subscriptions')) .'</div>',
    '#weight' => -20,
    );
  if (module_exists('subscriptions_mail')) {
    if (module_exists('mail_edit')) {
      $form['links'][] = array(
        '#value' => '<div>'. l($tr('Mail templates'), 'admin/build/mail-edit') .'</div>',
        '#weight' => -10,
      );
    }
    else {
      $form['links'][] = array(
        '#value' => '<div>'. l('Mail Editor', 'http://drupal.org/project/mail_edit') .'</div>',
        '#weight' => -10,
      );
    }
  }
  $form['links'][] = array(
    '#value' => '<div>'. l(t('Run cron manually!'), 'admin/reports/status/run-cron') .'</div>',
    '#weight' => 0,
  );
  $form['links'][] = array(
    '#value' => '<div>'. l(t('My subscriptions'), 'user/'. $user->uid .'/subscriptions') .'</div>',
    '#weight' => 10,
  );


  //$form['sub_settings'] = array(
  //    '#type' => 'fieldset',
  //    '#title' => t('General settings'),
  //    '#weight' => -3,
  //);
  //$form['sub_settings']['subscriptions_testpost'] = array(
  //  '#type'          => 'checkbox',
  //  '#title'         => t('Test held posts prior to sending'),
  //  '#default_value' => variable_get('subscriptions_testpost', 0),
  //  '#description'   => t('Tests to see if a post about to be sent by cron is still active.  Adds a small amount of overhead.  Default is OFF.'),
  //);
  //$form['sub_settings']['subscriptions_link_teaser'] = array(
  //  '#type'          => 'checkbox',
  //  '#title'         => t('Show subscribe link with teaser'),
  //  '#default_value' => variable_get('subscriptions_link_teaser', 1),
  //  '#description'   => t('Uncheck to show link only in node view.'),
  //);

  if (variable_get('subscriptions_show_install_info', 1) && (
        !module_exists('subscriptions_ui') ||
        !module_exists('subscriptions_mail') ||
        !module_exists('subscriptions_content') ||
        !module_exists('subscriptions_taxonomy'))) {
    if (empty($_POST)) {
      include_once drupal_get_path('module', 'subscriptions') .'/subscriptions.install.inc';
      _subscriptions_install_information();
    }
    $form['subscriptions_show_install_info'] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Keep showing the installation reminder above.'),
      '#default_value' => variable_get('subscriptions_show_install_info', 1),
      '#description'   => t('Uncheck this box to permanently remove the reminder, if you have a reason for not enabling the standard selection of Subscriptions modules.'),
      '#weight'        => -101,
    );
  }

  return system_settings_form($form);
}

/* ******************************************************* */
/*  user screens: display, edit functions                  */
/* ******************************************************* */

/**
 * Theme subscriptions page controls table.
 *
 * @ingroup themeable
 */
function theme_subscriptions_form_table($element) {
  $output = '';
  $rows = array();
  drupal_add_js(drupal_get_path('module', 'subscriptions') .'/subscriptions_tableselect.js', 'module', 'header', FALSE, TRUE, FALSE);
  $columns['checkboxes']    = array('data' => '&nbsp;', 'width' => '1%', 'class' => 'subscriptions-table');
  $columns['labels']        = array('data' => '&nbsp;', 'width' => '18%');
  if (isset($element['send_interval']) && $element['send_interval']['#access']) {
    $columns['send_interval'] = array('data' => t('Send interval'), 'width' => '20%');
  }
  if (isset($element['send_updates']) && $element['send_updates']['#access']) {
    $columns['send_updates']  = array('data' => t('On&nbsp;updates'), 'width' => '10%');
  }
  if (isset($element['send_comments']) && $element['send_comments']['#access'] && module_exists('comment')) {
    $columns['send_comments'] = array('data' => t('On&nbsp;comments'), 'width' => '10%');
  }

  // check whether we have an Author column
  if (isset($element['author'])) {
    foreach (element_children($element['checkboxes']) as $key) {
      foreach (element_children($element['checkboxes'][$key]) as $key1) {
        if ($key1 != -1) {
          $tr = 't';
          $columns['author'] = array('data' => $tr('Author'), 'width' => '20%');
        }
      }
    }
  }
  $column_keys = array_keys($columns);
  unset($columns[end($column_keys)]['width']);  // let the last column grow

  foreach (element_children($element['checkboxes']) as $key) {
    foreach (element_children($element['checkboxes'][$key]) as $key1) {
      $row = array();
      foreach (array_keys($columns) as $colkey) {
        $row[] = drupal_render($element[$colkey][$key][$key1]);
      }
      $rows[] = $row;
    }
  }
  if ($rows) {
    $output .= theme('table', array_values($columns), $rows);
    //$output .= drupal_render($element);
  }
  return $output;
}


/**
 * Subscriptions page submit handler.
 *
 * @ingroup form
 */
function subscriptions_page_form_submit($form, &$form_state) {
  global $user;
  if (isset($form_state['values']['header']['role']['roles'])) {
    $rid0 = current($form_state['values']['header']['role']['roles']);
  }
  if ($form_state['values']['op'] == $form_state['values']['submit']) {
    if (empty($rid0)) {
      $form_state['values']['header']['role']['roles'][] = -$form_state['values']['uid'];  // will be negated again below
    }
    $module = $form_state['values']['module'];
    $field = $form_state['values']['field'];
    $access_key = (isset($form_state['values']['access_key']) ? $form_state['values']['access_key'] : $module);
    foreach ($form_state['values'] as $element_values) {
      if (isset($element_values[0])) {
        $element_values = $element_values[0];
      }
      if (isset($element_values['defaults']['send_interval'])) {
        $defaults = $element_values['defaults'];
        foreach ($element_values['checkboxes'] as $value => $bundle) {
          foreach ($bundle as $author_uid => $set_to_enabled) {
            if ($set_to_enabled && (
                !isset($defaults['checkboxes'][$value][$author_uid]) ||
                $element_values['send_interval'][$value][$author_uid] != $defaults['send_interval'][$value][$author_uid] ||
                empty($element_values['send_updates'][$value][$author_uid]) != empty($defaults['send_updates'][$value][$author_uid]) ||
                empty($element_values['send_comments'][$value][$author_uid]) != empty($defaults['send_comments'][$value][$author_uid]))) {
              $send_updates = (empty($element_values['send_updates'][$value][$author_uid]) ? 0 : 1);
              $send_comments = (empty($element_values['send_comments'][$value][$author_uid]) ? 0 : 1);
              foreach ($form_state['values']['header']['role']['roles'] as $uid) {
                subscriptions_write($access_key, $module, $field, $value, $author_uid, -$uid, $element_values['send_interval'][$value][$author_uid], $send_updates, $send_comments);
              }
              $changed = TRUE;
            }
            elseif (!$set_to_enabled && isset($defaults['checkboxes'][$value][$author_uid])) {
              // It was enabled but now it is not. Delete.
              foreach ($form_state['values']['header']['role']['roles'] as $uid) {
                db_query("DELETE FROM {subscriptions} WHERE module = '%s' AND field = '%s' AND value = '%s' AND author_uid = %d AND recipient_uid = %d", $module, $field, $value, $author_uid, -$uid);
              }
              $changed = TRUE;
            }
          }
        }
      }
    }
    if (!empty($changed)) {
      drupal_set_message(t('Your subscriptions were updated.'));
    }
  }
  if (!empty($rid0)) {
    $form_state['redirect'] = 'admin/settings/subscriptions/userdefaults/'. arg(4);
    if ($rid0 != DRUPAL_AUTHENTICATED_RID) {
      $form_state['redirect'] .= '/'. $rid0;
    }
  }
}

/**
 * Display subscribed content data on the user and site subscriptions pages.
 * @ TODO clean up all of these parts
 *
 * @ingroup form
 */
function subscriptions_page($account, $stype) {
  $stypes = subscriptions_types();
  if (!isset($stypes[$stype])) {
    drupal_not_found();
    return;
  }

  if (arg(0) == 'admin') {
    $form['header'] = array(
      '#type'          => 'fieldset',
      '#title'         => t('Admin Information'),
      '#attributes'    => array('class' => 'error'),
      '#collapsible'   => TRUE,
      '#collapsed'     => FALSE,
      '#weight'        => -2,
    );
    $form['header'][] = array(
      '#type'          => 'item',
      '#value'         => t('The selected subscriptions will be given to <b>newly created</b> users.'),
      );

    $tr = 't';
    $form['header']['role'] = array(
      '#type'        => 'fieldset',
      '#title'       => $tr('Roles'),
      '#collapsible' => TRUE,
      '#collapsed'   => !is_numeric(arg(5)),
      );
    $form['header']['role']['header'] = array(
      '#type'          => 'markup',
      '#value'         => '<div>'. t('If a user has one or more roles assigned at creation time, then she will receive the additional subscriptions defined for these roles.') .'</div>',
      '#weight'        => -5,
      );
    $form['header']['role']['roles'] = array(
      '#type'          => 'select',
      '#title'         => t('Role(s)'),
      '#default_value' => (is_numeric(arg(5))? arg(5) : DRUPAL_AUTHENTICATED_RID),
      '#options'       => user_roles(TRUE),
      '#description'   => t('Select the role to load or role(s) to save.'),
      '#multiple'      => TRUE,
      );
    $form['header']['role']['load'] = array(
      '#type'          => 'submit',
      '#value'         => t('Load'),
      );
  }
  else {
    $form['header'][] = array(
      '#value'         => t('Current subscriptions:'),
      '#weight'        => -2,
      );
  }

  $form['footer'] = array(
    '#type' => 'item',
    '#description' => t('The master checkboxes on the left turn the given subscription on or off. Depending on the setup of the site, you may have additional options for active subscriptions.'),
    '#weight' => 10,
    );

  $callback = $stypes[$stype]['page'];
  $output = function_exists($callback) ? $callback($account, $form) : '';
  //drupal_add_link(array(
  //  'rel' => 'alternate',
  //  'type' => 'application/rss+xml',
  //  'title' => t('!name Subscriptions', array('!name' => $user->name)),
  //  'href' => url('subscriptions/feed'),
  //));
  drupal_set_title(isset($account) ? check_plain($account->name) : t('Subscriptions'));
  return theme('box', '', $output); // . theme('xml_icon', url('subscriptions/feed'));
}

/**
 * Construct the overview page, which displays a summary of subscriptions per
 * type as well as the user settings at user/UID/subscriptions.
 * This form is also used for admin/settings/subscriptions/userdefaults.
 *
 * @params $account
 *   Must be a valid user account from user_load() or NULL (for the admin form).
 */
function subscriptions_page_user_overview($account) {
  // Build summary
  $uid = (isset($account) ? $account->uid : -DRUPAL_AUTHENTICATED_RID);
  $counts = array();
  $result = db_query("SELECT module, field, count(1) as number FROM {subscriptions} WHERE recipient_uid = %d GROUP BY module, field", $uid);
  while ($subs = db_fetch_object($result)) {
    if (!empty($subs->module)) {
      $counts[$subs->module][$subs->field] = $subs->number;
    }
  }
  // give other modules a chance to add their count(s)
  $more_counts = module_invoke_all('count_user_subscriptions', $counts, $uid);
  $counts = array_merge($counts, $more_counts);

  $tr = 't';
  $header = array(array('data' => $tr('Type'), 'width' => '20%'), t('Number'));
  $types = subscriptions_types();
  if ($uid < 0) {
    unset($types['node']);
  }
  uasort($types, '_subscriptions_cmp_by_weight');
  foreach ($types as $stype => $type) {
    $has_access = ($uid < 0 && user_access('administer site configuration')) || user_access($type['access'], $account);
    if ($has_access || user_access('administer user subscriptions')) {
      $rows[] = array(
        ($has_access ? l($tr($type['title']), ($uid < 0 ? "admin/settings/subscriptions/userdefaults/$stype" : "user/$account->uid/subscriptions/$stype")) : $type['title']),
        isset($counts[$type['fields'][0]][$type['fields'][1]]) ? $counts[$type['fields'][0]][$type['fields'][1]] : 0,
      );
    }
  }
  $output  = theme('table', $header, $rows);
  $output .= theme('item', array('#value' => '', '#description' => t('Note: The counts on this page may differ from the ones on the detail pages for various technical reasons.')));
  $output .= '<br />';

  $output .= drupal_get_form('subscriptions_user_settings_form', $account);
  drupal_set_title($uid < 0 ? t('Subscriptions') : check_plain($account->name));
  return $output;
}

/**
 * Returns the form definition for the settings part of the overview page.
 * This subform is also used for admin/settings/subscriptions/userdefaults.
 *
 * @params $account
 *   Must be a valid user account from user_load() or NULL (for the admin form).
 *
 * @ingroup form
 */
function subscriptions_user_settings_form(&$form_state, $account) {
  global $user;

  $uid = (isset($account) ? $account->uid : -DRUPAL_AUTHENTICATED_RID);
  $tr = 't';
  $form['settings'] = array(
    '#type'          => 'fieldset',
    '#title'         => $tr('Settings'),
    '#weight'        => 5,
    '#collapsible'   => isset($account),
    '#collapsed'     => isset($account),
    );
  if ($uid < 0) {
    $form['settings']['info'] = array(
      '#type'          => 'fieldset',
      '#title'         => $tr('Admin Information'),
      '#attributes'    => array('class' => 'error'),
      '#collapsible'   => TRUE,
      '#collapsed'     => FALSE,
      );
    $form['settings']['info'][] = array(
      '#type'          => 'item',
      '#value'         => '<div>'. t("The subtabs here under %User_defaults mirror those found at !My_account_Subscriptions, except that the fieldsets here come up already expanded. Please go there now and familiarize yourself with what's there!",
        array(
          '%User_defaults' => $tr('User defaults'),
          '!My_account_Subscriptions' => l($tr('My account') .'|'. $tr('Subscriptions'), 'user/'. $user->uid .'/subscriptions'),
          '%Settings' => $tr('Settings'),
        )
      ) .'</div>',
    );
    $form['settings']['info'][] = array(
      '#type'          => 'item',
      '#value'         => '<div>'. t("The default settings that you define here are used for those users that haven't (yet) saved their own settings.",
        array(
          '%User_defaults' => $tr('User defaults'),
          '!My_account_Subscriptions' => l($tr('My account') .'|'. $tr('Subscriptions'), 'user/'. $user->uid .'/subscriptions'),
          '%Settings' => $tr('Settings'),
        )
      ) .'</div>',
    );
  }
  $form['settings']['autosub'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Auto-subscribe'),
    '#access'        => !isset($account) || user_access('subscribe to content', $account),
  );
  $form['settings']['autosub']['autosub_post'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Auto-subscribe to new content'),
    '#default_value' => _subscriptions_get_setting('autosub_on_post', $account),
    '#description'   => t('Automatically subscribes you to items that you create.'),
  );
  $form['settings']['autosub']['autosub_update'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Auto-subscribe to updated content'),
    '#default_value' => _subscriptions_get_setting('autosub_on_update', $account),
    '#description'   => t('Automatically subscribes you to items that you update (if you have that permission).'),
  );
  $form['settings']['autosub']['autosub_comment'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Auto-subscribe to comments'),
    '#default_value' => _subscriptions_get_setting('autosub_on_comment', $account),
    '#description'   => t('Automatically subscribes you to items that you comment on (or where you update a comment, if you have that permission).'),
    '#access'        => module_exists('comment') && (!isset($account) || user_access('post comments', $account) || user_access('post comments without approval', $account)),
  );
  $form['settings']['sendself'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Notify poster of own posts'),
    '#default_value' => _subscriptions_get_setting('send_self', $account),
    '#description'   => t("Sends you notifications about your own posts (if you're subscribed)."),
  );
  $form['settings']['digest'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Digest mode'),
    '#default_value' => _subscriptions_get_setting('digest', $account),
    '#description'   => t("Merges your notifications into a single email, sent at the interval you specify."),
  );
  $form['settings']['preferences'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Preferences'),
    '#access'        => FALSE,
  );
  $form['settings']['preferences']['send_interval'] = array(
    '#title'         => t('Send interval'),
    '#type'          => 'select',
    '#options'       => _subscriptions_send_intervals(),
    '#default_value' => _subscriptions_get_setting('send_interval', $account),
    '#description'   => t('The frequency of notifications; it may take longer, but notifications will not be sent more frequently than indicated.'),
  );
  $form['settings']['preferences']['send_updates'] = array(
    '#title'         => t('On updates'),
    '#type'          => 'checkbox',
    '#default_value' => _subscriptions_get_setting('send_updates', $account),
    '#description'   => t('Sends a notification when an item is updated.'),
  );
  $form['settings']['preferences']['send_comments'] = array(
    '#title'         => t('On comments'),
    '#type'          => 'checkbox',
    '#default_value' => _subscriptions_get_setting('send_comments', $account),
    '#description'   => t('Sends a notification when an item receives a comment or reply.'),
  );
  $form['settings']['preferences']['description'] = array(
    '#value'       => t('Any changes you make here will only apply to newly created subscriptions.'),
  );
  $form['settings']['visibility'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Visibility of controls'),
    '#collapsible'   => isset($account),
    '#collapsed'     => isset($account),
    '#access'        => FALSE,
  );
  $form['settings']['visibility'][] = array(
    '#type'          => 'item',
    '#description'   => t('The control(s) mentioned below appear(s) as one or more columns<br /> a) in the %Subscribe subform of node pages and<br /> b) on the sibling subtabs of this %Overview page.<br /> After you have set your %Preferences right above, you may want to hide the corresponding column(s), if you never change those values anyway.', array('%Subscribe' => $tr('Subscribe'), '%Overview' => $tr('Overview'), '%Preferences' => $tr('Preferences'))),
  );
  $visibility_options = array(t('Visible'), t('Hidden'));
  if (!isset($account)) {
    $visibility_options[] = t('Permanently hidden, except for the %preferences', array('%preferences' => 'Preferences'));
    $visibility_options[] = t('Completely inaccessible to the user');
    $form['settings']['visibility']['info'] = array(
      '#type'          => 'fieldset',
      '#title'         => $tr('Admin Information'),
      '#attributes'    => array('class' => 'error'),
      '#collapsible'   => TRUE,
      '#collapsed'     => FALSE,
      );
    $form['settings']['visibility']['info'][] = array(
      '#type'          => 'item',
      '#value'         =>
        t("Beyond defining the %Visible/%Hidden defaults for your users, you can",
          array(
            '%Visible' => $tr('Visible'),
            '%Hidden' => $tr('Hidden'),
          )) .
        '<ul><li>'. t("permanently hide the columns (and the corresponding visibility controls), so that the users cannot make them visible anymore (they can still change their %Preferences though), or",
          array(
            '%Preferences' => $tr('Preferences'),
          )) .'</li>'.
        '<li>'. t("you can even hide the %Preferences, so that what you've set right above is cast in stone.",
          array(
            '%Preferences' => $tr('Preferences'),
          )) .'</li></ul>',
    );
  }
  foreach (array('interval', 'updates', 'comments') as $parm ) {
    $site_setting = _subscriptions_get_setting('send_'. $parm .'_visible', 0);
    $pref_access = !isset($account) || $site_setting >= -1;
    $control_access = !isset($account) || $site_setting >= 0;
    $form['settings']['visibility']['send_'. $parm .'_visible'] = array(
      '#title'         => ($parm == 'interval' ? t('Send interval') :
                          ($parm == 'updates' ? t('On updates') : t('On comments'))),
      '#type'          => 'radios',
      '#options'       => $visibility_options,
      '#default_value' => 1-_subscriptions_get_setting('send_'. $parm .'_visible', $account),
      '#access'        => $control_access,
    );
    if ($control_access) {
      $form['settings']['visibility']['#access'] = TRUE;
    }
    $form['settings']['preferences']['send_'. $parm]['#access'] = $pref_access;
    if ($pref_access) {
      $form['settings']['preferences']['#access'] = TRUE;
    }
  }
  $form['settings']['save'] = array(
    '#type'          => 'submit',
    '#value'         => t('Save'),
  );
  if ($uid > 0 && !_subscriptions_get_setting('uses_defaults', $account)) {
    $form['settings']['reset'] = array(
      '#type'        => 'submit',
      '#value'       => t('Reset to site defaults'),
    );
  }
  $form['settings']['#submit'][] = 'subscriptions_user_options_submit';
  return $form;
}

/**
 * User settings subform submit handler.
 *
 * @ingroup form
 */
function subscriptions_user_settings_form_submit($form, &$form_state) {
  if (isset($form_state['values']['reset']) && $form_state['values']['op'] == $form_state['values']['reset']) {
    if (($uid = arg(1))> 0) {
      db_query("UPDATE {subscriptions_user} SET digest = -1, send_interval = -1, send_updates = -1, send_comments = -1, send_interval_visible = -1, send_updates_visible = -1, send_comments_visible = -1, autosub_on_post = -1, autosub_on_update = -1, autosub_on_comment = -1, send_self = -1 WHERE uid = %d", $uid);
      drupal_set_message(t('The site defaults were restored.'));
    }
  }
  elseif ($form_state['values']['op'] == $form_state['values']['save']) {
    if (arg(0) == 'user') {
      $uid = arg(1);
    }
    else {
      $uid = -DRUPAL_AUTHENTICATED_RID;
    }
    $send_interval_visible = 1-$form_state['values']['send_interval_visible'];
    $send_updates_visible = 1-$form_state['values']['send_updates_visible'];
    $send_comments_visible = 1-$form_state['values']['send_comments_visible'];
    db_query("
      UPDATE {subscriptions_user}
      SET digest = %d, send_interval = %d, send_updates = %d, send_comments = %d,
          send_interval_visible = %d, send_updates_visible = %d, send_comments_visible = %d,
          autosub_on_post = %d, autosub_on_update = %d, autosub_on_comment = %d,
          send_self = %d
      WHERE uid = %d",
      $form_state['values']['digest'],
      ($uid > 0 && $send_interval_visible == -2 ? -1 : $form_state['values']['send_interval']),
      ($uid > 0 && $send_updates_visible == -2 ? -1 : $form_state['values']['send_updates']),
      ($uid > 0 && $send_comments_visible == -2 ? -1 : $form_state['values']['send_comments']),
      $send_interval_visible, $send_updates_visible, $send_comments_visible,
      $form_state['values']['autosub_post'],
      $form_state['values']['autosub_update'],
      $form_state['values']['autosub_comment'],
      $form_state['values']['sendself'],
      $uid);
    drupal_set_message(t('The changes have been saved.'));
  }
}

/**
 * Provide the form definition for admin/settings/subscriptions/intervals.
 *
 * Called by _subscriptions_menu() in subscriptions.module.
 *
 * @ingroup form
 *
 * @see _subscriptions_menu()
 */
function subscriptions_intervals(&$form_state) {
  $default = '';
  foreach (_subscriptions_send_intervals() as $interval => $text) {
    $default .= "$interval|$text\n";
  }
  $form['intervals'] = array(
    '#type' => 'textarea',
    '#cols' => 40,
    '#default_value' => $default,
    '#description' => t('Every line has the format of seconds|human readable text'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Sending intervals settings form submit handler.
 *
 * @ingroup form
 */
function subscriptions_intervals_submit($form, &$form_state) {
  $intervals = array();
  foreach (explode("\n", $form_state['values']['intervals']) as $line) {
    if ($line) {
      $v = explode('|', $line);
      $intervals[$v[0]] = $v[1];
    }
  }
  variable_set('subscriptions_send_intervals', $intervals);
}

/**
 * Provide the form definition for adding subscriptions via
 * subscriptions/add/... link.
 *
 * Callback of _subscriptions_menu() in subscriptions.module.
 *
 * @param $stype
 *   Subscription type.
 * @param $sid
 *   Subscription parameter (depends on type).
 * @param $author_uid
 *   User ID for author-specific subscriptions or NULL/-1 for all authors.
 *
 * @ingroup forms
 * @see _subscriptions_menu()
 */
function subscriptions_add_form(&$form_state, $stype, $sid, $author_uid = NULL) {
  global $user;
  $form['sid'] = array('#type' => 'value', '#value' => $sid);
  $form['uid'] = array('#type' => 'value', '#value' => $user->uid);
  $form['stype'] = array('#type' => 'value', '#value' => $stype);
  $form['author_uid'] = array('#type' => 'value', '#value' => $author_uid);
  $form['send_interval'] = array(
    '#title' => t('Send interval'),
    '#type' => 'select',
    '#options' => _subscriptions_send_intervals(),
    '#default_value' => _subscriptions_get_setting('send_interval', $user),
    '#access' => _subscriptions_get_setting('send_interval_visible', $user),
  );
  $form['updates'] = array(
    '#type' => 'checkbox',
    '#title' => 'On updates',
    '#default_value' => _subscriptions_get_setting('send_updates', $user),
    '#access' => _subscriptions_get_setting('send_updates_visible', $user),
  );
  $form['comments'] = array(
    '#type' => 'checkbox',
    '#title' => 'On comments',
    '#default_value' => _subscriptions_get_setting('send_comments', $user),
    '#access' => _subscriptions_get_setting('send_comments_visible', $user),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Subscribe'),
  );
  return $form;
}

/**
 * Add Subscription form submit handler.
 *
 * @ingroup form
 */
function subscriptions_add_form_submit($form, &$form_state) {
  if ($a = module_invoke_all('subscriptions', 'stype', $form_state['values']['stype'], $form_state['values']['sid'], (isset($form_state['values']['author_uid']) ? $form_state['values']['author_uid'] : -1))) {
    list($module, $field, $value, $author_uid) = $a;
    $uid = $form_state['values']['uid'];
    subscriptions_write_subscription($module, $field, $value, $author_uid, $uid, $form_state['values']['send_interval'], $form_state['values']['updates'], $form_state['values']['comments']);
    drupal_set_message(t('Your subscription was activated.'));
    $form_state['redirect'] = '<front>';
  }
  else {
    drupal_set_message(t('Your subscription could not be activated.'));
  }
}

/**
 * Provide the form definition for deleting subscriptions via
 * subscriptions/del/... link.
 *
 * Callback of _subscriptions_menu() in subscriptions.module.
 *
 * @param $form_state
 *   FAPI form state.
 * @param $stype
 *   Subscription type.
 * @param $sid
 *   Subscription parameter (depends on type).
 * @param $author_uid
 *   User ID for author-specific subscriptions or -1/NULL for all authors.
 *
 * @ingroup forms
 * @see _subscriptions_menu()
 */
function subscriptions_del_form(&$form_state, $stype, $sid, $author_uid = NULL) {
  global $user;

  if ($a = module_invoke_all('subscriptions', 'stype', $stype, $sid, (isset($author_uid) ? $author_uid : -1))) {
    list($module, $field, $value, $author_uid) = $a;
    return subscriptions_delete_form($form_state, $module, $field, $value, $author_uid, $user->uid);
  }
  drupal_set_message(t('Your subscription could not be deactivated.'));
}

/**
 * Returns a list of send intervals.
 */
function _subscriptions_send_intervals() {
  return variable_get('subscriptions_send_intervals',
    array(
      1 => t('As soon as possible'),
      900 => t('Every 15 minutes'),
      3600 => t('Hourly'),
      10800 => t('Every three hours'),
      86400 => t('Daily'),
    )
  );
}

/**
 * Helper function for the submodules to build the table(s) on their
 * user/UID/subscriptions/... pages.
 *
 * @ingroup form
 */
function subscriptions_form_helper(&$form, &$defaults, $author_uid, $key, $title, $subscription) {
  static $intervals;
  if (!isset($intervals)) {
    $intervals = _subscriptions_send_intervals();
  }
  if (isset($subscription['author_uid']) && ($author_uid == $subscription['author_uid'])) {
    $defaults['checkboxes'][$key][$author_uid] = 1;
  }
  $defaults['send_interval'][$key][$author_uid] = $subscription['send_interval'];
  if ($subscription['send_comments']) {
    $defaults['send_comments'][$key][$author_uid] = 1;
  }
  if ($subscription['send_updates']) {
    $defaults['send_updates'][$key][$author_uid] = 1;
  }
  if (isset($subscription['author_uid']) && $subscription['author_uid'] > 0) {
    $author_value = check_plain(db_result(db_query('SELECT name FROM {users} WHERE uid = %d', $subscription['author_uid'])));
  }
  elseif ($author_uid == 0) {
    $author_value = variable_get('anonymous', '???');
  }
  else {
    $author_value = '';
  }

  $form['labels'][$key][$author_uid] = array(
    '#type' => 'item',
    '#value' => $title,
  );
  $form['author'][$key][$author_uid] = array(
    '#value' => $author_value,
  );
  $form['send_interval'][$key][$author_uid] = array(
    '#type' => 'select',
    '#options' => $intervals,
    '#default_value' => $defaults['send_interval'][$key][$author_uid],
  );
  foreach (array('checkboxes', 'send_comments', 'send_updates') as $index => $name) {
    $form[$name][$key][$author_uid] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($defaults[$name][$key][$author_uid]),
    );
  }
}

/**
 * Helper function for the submodules to hide invisible or empty columns from
 * their page form table(s).
 *
 * @ingroup form
 */
function subscriptions_form_column_filter(&$form_table, $uid) {
  foreach (array('send_interval', 'send_comments', 'send_updates', 'author') as $key) {
    if (isset($form_table[$key])) {
      $form_table[$key]['#access'] = ($uid <= 0 || $key == 'author' || _subscriptions_get_setting($key .'_visible', $uid) > 0);
    }
  }
}

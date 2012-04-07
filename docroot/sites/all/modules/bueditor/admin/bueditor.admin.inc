<?php

/**
 * @file
 * Serves administration pages of BUEditor.
 */

include_once './' . drupal_get_path('module', 'bueditor') . '/bueditor.inc';

/**
 * Admin main page.
 */
function bueditor_admin() {
  bueditor_eop();
  $editors = bueditor_editors('all');
  $token = drupal_get_token(date('ymd'));
  $header = array(t('Editor name'), array('data' => t('Operations'), 'colspan' => 2));
  $rows = array();
  foreach ($editors as $eid => $editor) {
    $rows[] = array($editor->name, l(t('Edit'), 'admin/settings/bueditor/'. $eid) .' | '. l(t('Delete'), 'admin/settings/bueditor/'. $eid .'/delete', array('attributes' => array('class' => 'eop-delete'))) .' | '. l(t('Copy'), 'admin/settings/bueditor', array('attributes' => array('class' => 'eop-copy', 'name' => 'Copy of '. $editor->name), 'query' => array('eop' => 'copy', 'eid' => $eid, 'token' => $token))) .' | '. l(t('Export'), 'admin/settings/bueditor', array('attributes' => array('class' => 'eop-export'), 'query' => array('eop' => 'export', 'eid' => $eid, 'token' => $token))));
  }
  $rows[] = array('', l(t('Add new editor'), 'admin/settings/bueditor/new') .' | '. l(t('Import editor'), 'admin/settings/bueditor/import') .' | '. l(t('Add the default editor'), 'admin/settings/bueditor', array('attributes' => array('class' => 'eop-add-default', 'name' => 'default editor'), 'query' => array('eop' => 'add_default', 'token' => $token))));
  $output = '<h2 class="title">'. t('Available editors') .'</h2>';
  $output .= theme('table', $header, $rows, array('id' => 'available-editors-list'));
  $output .= drupal_get_form('bueditor_admin_form');
  return $output;
}

/**
 * Admin form.
 */
function bueditor_admin_form($form_state) {
  $form['roles'] = array('#tree' => TRUE);
  $roles = bueditor_sorted_roles();
  $form['#weighted'] = count($roles) > 3;
  //user#1
  if ($GLOBALS['user']->uid == 1) {
    $u1 = array('name' => t('user #1'), 'weight' => t('n/a'), 'editor' => variable_get('bueditor_user1', 1), 'alt' => variable_get('bueditor_user1_alt', 0));
    $form['roles']['u1'] = bueditor_role_form($u1, $form['#weighted'], 1);
  }
  //other roles
  foreach ($roles as $rid => $role) {
    $core = $rid == DRUPAL_ANONYMOUS_RID || $rid == DRUPAL_AUTHENTICATED_RID;
    $form['roles'][$rid] = bueditor_role_form($role, $form['#weighted'], $core);
  }
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));
  $form['#submit'][] = 'bueditor_admin_submit';
  $form['#theme'] = 'bueditor_admin';
  return $form;
}

/**
 * Admin form theming.
 */
function bueditor_admin_theme($form = array()) {
  $header = array(t('User role'), t('Assigned editor'), t('Alternative editor'));
  $keys = array('name', 'editor', 'alt');
  $info = '';
  if ($form['#weighted']) {
    $header[] = t('Weight');
    $keys[] = 'weight';
    $info = '<br />'. t('For users who have <strong>multiple roles</strong>, the <strong>weight</strong> property will determine the assigned editor. Lighter roles that are placed upper will take the precedence. So, an administrator role should be placed over other roles by having a smaller weight, ie. -10.');
  }

  $rows = array();
  foreach (element_children($form['roles']) as $rid) {
    $cells = array();
    foreach ($keys as $key) {
      $cells[] = drupal_render($form['roles'][$rid][$key]);
    }
    $rows[] = $cells;
  }

  $output = '<h2 class="title">'. t('Role-editor assignments') .'</h2>';
  $output .= theme('table', $header, $rows, array('id' => 'roles-editors-list'));
  $output .= '<div class="form-item"><div class="description">'. t('Assign editors to user roles.') .'<br />'. t('Alternative editor makes it possible to use different editors for different textareas or different editors on diffrent pages. You just have to configure visibility settings for each editor.') . $info .'</div></div>';
  $output .= drupal_render($form);
  return $output;
}

/**
 * Admin form submission.
 */
function bueditor_admin_submit($form, &$form_state) {
  $roles = $form_state['values']['roles'];
  $roles[DRUPAL_ANONYMOUS_RID]['weight'] = 12;
  $roles[DRUPAL_AUTHENTICATED_RID]['weight'] = 11;
  if ($GLOBALS['user']->uid == 1 && isset($roles['u1'])) {
    variable_set('bueditor_user1', $roles['u1']['editor']);
    variable_set('bueditor_user1_alt', $roles['u1']['alt']);
    unset($roles['u1']);
  }
  if (count($roles) > 3) {
    uasort($roles, 'bueditor_rolesort');
  }
  variable_set('bueditor_roles', $roles);
  drupal_set_message(t('The changes have been saved.'));
}

/**
 * Role-editor form
 */
function bueditor_role_form($role, $weight = TRUE, $core = TRUE) {
  $form['name'] = array(
    '#type' => 'markup',
    '#value' => $role['name'],
  );
  if ($weight) {
    $form['weight'] = $core ? array(
      '#type' => 'markup',
      '#value' => $role['weight'],
    ) : array(
      '#type' => 'weight',
      '#default_value' => $role['weight'],
    );
  }
  $form['editor'] = array(
    '#type' => 'select',
    '#options' => bueditor_editor_options(),
    '#default_value' => $role['editor'],
  );
  $form['alt'] = array(
    '#type' => 'select',
    '#options' => bueditor_editor_options(),
    '#default_value' => $role['alt'],
  );
  return $form;
}

/**
 * Editor options.
 */
function bueditor_editor_options() {
  static $options;
  if (!isset($options)) {
    $options = array(0 => t('none'));
    foreach (bueditor_editors('all') as $eid => $editor) {
      $options[$eid] = $editor->name;
    }
  }
  return $options;
}

/**
 * Button form
 */
function bueditor_button_form($button = NULL) {
  $button = is_object($button) ? $button : bueditor_button_defaults();
  $form = array();
  $form['title'] = array(
    '#type' => 'textfield',
    '#default_value' => $button->title,
    '#size' => 14,
    '#attributes' => array('class' => 'input-title'),
  );
  $form['content'] = array(
    '#type' => 'textarea',
    '#default_value' => $button->content,
    '#rows' => 1,
    '#attributes' => array('class' => 'input-content'),
  );
  $form['icon'] = array(
    '#type' => 'textfield',
    '#default_value' => $button->icon,
    '#size' => 3,
    '#attributes' => array('class' => 'input-icon'),
  );
  $form['accesskey'] = array(
    '#type' => 'textfield',
    '#default_value' => $button->accesskey,
    '#size' => 2,
    '#maxlength' => 1,
    '#attributes' => array('class' => 'input-key'),
  );
  $form['weight'] = array(
    '#type' => 'textfield',
    '#default_value' => $button->weight,
    '#size' => 3,
    '#attributes' => array('class' => 'input-weight'),
  );
  return $form;
}

/**
 * Editor form.
 */
function bueditor_editor_form($form_state, $editor = NULL) {
  $editor = is_object($editor) ? $editor : bueditor_editor_defaults();
  $editor->eid = isset($editor->eid) ? $editor->eid : '';

  $form = array(
    '#cache' => TRUE,
    '#tree' => TRUE,
    '#theme' => 'bueditor_editor',
    '#attributes' => array('enctype' => 'multipart/form-data'),
  );
  $form['editor']['eid'] = array(
    '#type' => 'hidden',
    '#value' => $editor->eid
  );
  $form['editor']['name'] = array('#type' => 'textfield',
    '#title' => t('Editor name'),
    '#maxlength' => 255,
    '#default_value' => $editor->name,
    '#required' => TRUE,
  );
  $form['editor']['pages'] = array('#type' => 'textarea',
    '#title' => t('Show the editor on specific pages'),
    '#default_value' => $editor->pages,
    '#description' => t('Enter one page per line as Drupal paths. The * character is a wildcard.'),
  );
  $form['editor']['excludes'] = array('#type' => 'textarea',
    '#title' => t('Hide the editor for specific textareas'),
    '#default_value' => $editor->excludes,
    '#description' => t('Enter one textarea ID per line. The * character is a wildcard.'),
  );
  $form['editor']['iconpath'] = array('#type' => 'textfield',
    '#title' => t('Directory of editor icons'),
    '#maxlength' => 255,
    '#default_value' => $editor->iconpath,
    '#description' => t('Web accessible directory path where editor icons reside.') .' '. t('Placeholders that you can use are; %BUEDITOR (bueditor path), %FILES (drupal files path), and %THEME (current theme\'s path).'),
  );
  $form['editor']['librarypath'] = array('#type' => 'textarea',
    '#title' => t('Editor specific library files'),
    '#default_value' => $editor->librarypath,
    '#description' => t('Web accessible javascript(.js) or style sheet(.css) file paths to be included with the editor. Enter one file path per line.') .' '. t('Placeholders that you can use are; %BUEDITOR (bueditor path), %FILES (drupal files path), and %THEME (current theme\'s path).'),
  );
  $form['editor']['spriteon'] = array('#type' => 'checkbox',
    '#title' => t('Enable CSS sprites'),
    '#default_value' => $editor->spriteon,
    '#description' => t('<a href="http://www.alistapart.com/articles/sprites">What are CSS sprites?</a>. The sprites will be stored in %dir.', array('%dir' => bueditor_sprites_dir())),
  );

  //buttons
  foreach (bueditor_buttons($editor->eid) as $bid => $button) {
    $form['buttons'][$bid] = bueditor_button_form($button);
    $form['checks'][$bid] = array('#type' => 'checkbox');
  }

  //imported/copied buttons are previewed. not saved yet.
  if (isset($_SESSION['bueimport']) && $import = $_SESSION['bueimport']) {
    $import = is_array($import) ? $import : bueditor_import_csv_buttons($import);
    if (empty($import)) {
      drupal_set_message(t('There is no button to import.'), 'warning');
    }
    else {
      foreach ($import as $bid => $button) {
        $form['buttons']['new'. $bid] = bueditor_button_form($button);
      }
      drupal_set_message(t('New buttons are ready to be saved.'));
    }
    unset($_SESSION['bueimport']);
  }

  //there is always two new buttons.
  $form['buttons']['new'] = $form['buttons']['new_'] = bueditor_button_form();

  if ($editor->eid) {
    //actions for selected buttons
    $form += bueditor_selaction_form();
    //button import
    $form += bueditor_button_import_form();
    //demo
    $form['demo']['demo'] = array(
      '#type' => 'textarea',
      '#title' => t('Demo'),
      '#rows' => 10,
      '#value' => 'DEMO',
      '#id' => 'editor-demo',
      '#name' => 'body',
    );
    $form['demo']['format'] = filter_form();
  }
    //configuration submit
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'), '#submit' => array('bueditor_editor_submit'));
  return $form;
}

/**
 * Editor form theming.
 */
function bueditor_editor_theme($form = array()) {
  $path = drupal_get_path('module', 'bueditor');
  $eid = $form['editor']['eid']['#value'];
  $header = array(
    array('data' => t('Title'), 'class' => 'th-title'),
    array('data' => t('Content'), 'class' => 'th-content'),
    array('data' => t('Icon'), 'class' => 'th-icon'),
    array('data' => t('Key'), 'class' => 'th-key'),
    array('data' => t('Weight'), 'class' => 'th-weight'),
    theme('table_select_header_cell'),
  );
  $rows = array();
  foreach (element_children($form['buttons']) as $bid) {
    $new = !is_numeric($bid);
    $cells = array();
    $cells[] = drupal_render($form['buttons'][$bid]['title']);
    $cells[] = drupal_render($form['buttons'][$bid]['content']);
    $cells[] = drupal_render($form['buttons'][$bid]['icon']);
    $cells[] = drupal_render($form['buttons'][$bid]['accesskey']);
    $cells[] = drupal_render($form['buttons'][$bid]['weight']);
    $cells[] = $new ? '<a>new</a>' : drupal_render($form['checks'][$bid]);
    $row = array('data' => $cells, 'class' => 'draggable');
    if ($new) {
      $row['class'] .= ' new-button';
    }
    $rows[] = $row;
  }

  //set title
  if ($form['editor']['name']['#value']) {
    drupal_set_title(t('Settings for the editor %name', array('%name' => $form['editor']['name']['#value'])));
  }
  $name = theme('fieldset', array(
    '#title' => t('Editor name'),
    '#children' => drupal_render($form['editor']['name']),
    '#collapsible' => TRUE,
    '#collapsed' => $eid
  ));
  $visibility = theme('fieldset', array(
    '#title' => t('Visibility settings'),
    '#children' => drupal_render($form['editor']['pages']) . drupal_render($form['editor']['excludes']),
    '#collapsible' => TRUE,
    '#collapsed' => $eid
  ));
  $paths = theme('fieldset', array(
    '#title' => t('Editor paths'),
    '#children' => drupal_render($form['editor']['iconpath']) . drupal_render($form['editor']['spriteon']) . drupal_render($form['editor']['librarypath']),
    '#collapsible' => TRUE,
    '#collapsed' => $eid
  ));
  $buttons = theme('table', $header, $rows, array('class' => 'button-table', 'id' => 'button-table'));
  $selaction = $eid ? '<div id="sel-action-wrapper">'. drupal_render($form['selaction']) . drupal_render($form['copyto']) . drupal_render($form['go']) .'</div>' : '';
  $import = $eid ? theme('fieldset', array(
    '#title' => t('Import Buttons'),
    '#children' => t('<em>You can either upload a CSV file exported from BUEditor 6.x-1.x or earlier</em> !csvfield <em>OR paste the editor code containing the buttons</em> !codefield', array('!csvfield' => drupal_render($form['importcsv']), '!codefield' => drupal_render($form['importcode']))) . drupal_render($form['import']),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  )) : '';

  $timer = timer_read('page');
  $demo = '';
  if ($eid && !isset($_GET['nodemo']) && bueditor_settle($eid)) {
    $set['BUE']['preset']['editor-demo'] = "e$eid";
    $demo = drupal_render($form['demo']);
    $set['BUE']['demotime'] = round(timer_read('page') - $timer);
  }

  $set['BUE']['iconpath'] = bueditor_path_tr($form['editor']['iconpath']['#value']);
  $set['BUE']['iconlist'] = bueditor_icons($set['BUE']['iconpath']);
  drupal_add_css($path .'/admin/bueditor.edit.css');
  drupal_add_js($path .'/admin/bueditor.edit.js');
  drupal_add_js($set, 'setting');
  drupal_add_tabledrag('button-table', 'order', 'sibling', 'input-weight');

  $output = $name . $visibility . $paths;
  $output .= '<h2 class="title">'. t('Buttons') .'</h2>'. $buttons . $selaction;
  $output .= drupal_render($form['submit']) . $import;
  $output .= drupal_render($form) . $demo;
  return $output;
}

/**
 * Editor form submission.
 */
function bueditor_editor_submit($form, &$form_state) {
  $editor = (object) $form_state['values']['editor'];
  $buttons = array();
  foreach ($form_state['values']['buttons'] as $bid => $button) {
    if ($button['title']) {
      $button['bid'] = $bid;
      $button['content'] = str_replace("\r\n", "\n", $button['content']);
      $buttons[] = (object) $button;
    }
  }
  $old = $editor->eid;
  if ($editor = bueditor_write_editor($editor, $buttons)) {
    $form_state['redirect'] = 'admin/settings/bueditor/'. $editor->eid;
    $old ? drupal_set_message(t('The changes have been saved.')) : bueditor_message_added($editor->name);
  }
}

/**
 *  Selected buttons actions form.
 */
function bueditor_selaction_form() {
  $form['selaction'] = array('#type' => 'select', '#options' => array('' => t('... selecteds'), 'delete' => t('Delete'), 'export' => t('Export'), 'copyto' => t('Copy to editor')));
  $eoptions = array('new' => t('Add new editor')) + bueditor_editor_options();
  unset($eoptions[0]);
  $form['copyto'] = array('#type' => 'select', '#options' => $eoptions);
  $form['go'] = array('#type' => 'submit', '#value' => t('Go'), '#submit' => array('bueditor_selaction_submit'));
  return $form;
}

/**
 * Selected buttons actions form submission.
 */
function bueditor_selaction_submit($form, &$form_state) {
  $bids = array_keys(array_filter($form_state['values']['checks']));
  switch ($form_state['values']['selaction']) {
    case 'delete':
      bueditor_delete_buttons($bids);
      break;
    case 'export':
      bueditor_export_buttons($bids);
      break;
    case 'copyto':
      $_SESSION['bueimport'] = bueditor_buttons_by_id($bids);
      $form_state['redirect'] = 'admin/settings/bueditor/'. $form_state['values']['copyto'];
      break;
  }
  //don't forget to save editor specific changes.
  $editor = (object) $form_state['values']['editor'];
  $editor->eid && bueditor_write_editor($editor);
}

/**
 * Delete buttons.
 */
function bueditor_delete_buttons($bids = array()) {
  if (!empty($bids)) {
    foreach ($bids as $bid) {
      bueditor_delete_button($bid);
    }
    drupal_set_message(t('Selected buttons have been deleted.'));
  }
}
function bueditor_delete_button($bid) {
  db_query("DELETE FROM {bueditor_buttons} WHERE bid = %d", $bid);
}

/**
 * Export buttons using var_export. The genarated code can also be imported as an editor.
 */
function bueditor_export_buttons($bids = array()) {
  $editor = array();
  $editor['buttons'] = bueditor_exportable_buttons(bueditor_buttons_by_id($bids));
  if (!empty($editor['buttons'])) {
    bueditor_export_text(var_export($editor, TRUE), 'bueditor.buttons.txt');
  }
  drupal_set_message(t('There is no button to export.'), 'warning');
}

/**
 * Button import form.
 */
function bueditor_button_import_form() {
    $form['importcsv'] = array('#type' => 'file', '#title' => t('CSV file containing the buttons'));
    $form['importcode'] = array('#type' => 'textarea',
      '#title' => t('Editor code'),
      '#rows' => 10,
      '#description' => t('Enter previously exported editor code.') .' '. t('The code will be scanned for only buttons and all other data will be ignored.') .'<div class="warning">'. t('This code will be evaluated as PHP, therefore you must be sure that it is harmless and produces proper editor data.') .'</div>',
  );
    $form['import'] = array('#type' => 'submit', '#value' => t('Import'), '#submit' => array('bueditor_button_import_submit'));
  return $form;
}

/**
 * Button import form submission.
 */
function bueditor_button_import_submit($form, &$form_state) {
  if ($file = file_save_upload('importcsv', array('file_validate_extensions' => array('csv')), file_directory_temp())) {
    $_SESSION['bueimport'] = $file->filepath;
  }
  elseif ($code = $form_state['values']['importcode']) {
    if ($editor = bueditor_exec_editor_code($code)) {
      $_SESSION['bueimport'] = bueditor_grab_imported_buttons($editor);
    }
    else {
      drupal_set_message(t('The editor code did not produce proper editor data.'), 'error');
    }
  }
}

/**
 * Import buttons from a CSV file.
 */
function bueditor_import_csv_buttons($file) {
  $buttons = array();
  if (is_file($file) && $fp = fopen($file, 'r')) {
    $fields = fgetcsv($fp, 100000);
    if (in_array('title', $fields)) {
      while ($values = fgetcsv($fp, 100000)) {
        $button = array();
        for ($i = 0; isset($fields[$i]); $i++) {
          $button[$fields[$i]] = stripslashes($values[$i]);
        }
        $buttons[] = (object) $button;
      }
    }
  }
  return $buttons;
}

/**
 * Editor delete form.
 */
function bueditor_delete_form($form_state, $editor) {
  return confirm_form(array(), t('Are you sure you want to delete the editor @name?', array('@name' => $editor->name)), 'admin/settings/bueditor', t('All buttons and settings of this editor will be removed.') .' '. t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Editor delete form submission.
 */
function bueditor_delete_form_submit($form, &$form_state) {
  $editor = $form['#parameters'][2];
  bueditor_delete_editor($editor->eid);
  drupal_set_message(t('Editor %name has been deleted.', array('%name' => $editor->name)));
  $form_state['redirect'] = 'admin/settings/bueditor';
}

/**
 * Delete an editor.
 */
function bueditor_delete_editor($eid) {
  db_query("DELETE FROM {bueditor_buttons} WHERE eid = %d", $eid);
  db_query("DELETE FROM {bueditor_editors} WHERE eid = %d", $eid);
  //update roles
  $roles = variable_get('bueditor_roles', array());
  foreach ($roles as $rid => $role) {
    $roles[$rid]['editor'] = $eid == $role['editor'] ? 0 : $role['editor'];
    $roles[$rid]['alt'] = $eid == $role['alt'] ? 0 : $role['alt'];
  }
  variable_set('bueditor_roles', $roles);
}

/**
 * Editor import form.
 */
function bueditor_editor_import_form(&$form_state) {
  bueditor_set_quick_import();
  $form['name'] = array('#type' => 'textfield',
    '#title' => t('Editor name'),
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  $form['dirname'] = array('#type' => 'textfield',
    '#title' => t('Directory of editor files'),
    '#maxlength' => 255,
    '#field_prefix' => file_directory_path() .'/',
    '#description' => t('Define a directory to store the library files imported from the editor code. Icons will be stored in a directory named "icons" under this directory. If you do not want to import the files or if the editor uses existing ones you can leave this field blank.'),
  );
  $form['code'] = array('#type' => 'textarea',
    '#title' => t('Editor code (PHP)'),
    '#rows' => 20,
    '#required' => TRUE,
    '#description' => t('Enter previously exported editor code.') .'<div class="warning">'. t('This code will be evaluated as PHP, therefore you must be sure that it is harmless and produces proper editor data.') .'</div>',
  );
  $form['overwrite'] = array('#type' => 'select',
    '#title' => t('Overwrite'),
    '#options' => bueditor_editor_options(),
    '#description' => t('You can optionally select the editor you want to overwrite. Otherwise, a new one will be created.'),
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t('Import'));
  $form['#validate'][] = 'bueditor_editor_import_validate';
  $form['#submit'][] = 'bueditor_editor_import_submit';
  return $form;
}

/**
 * Editor import form validation.
 */
function bueditor_editor_import_validate($form, &$form_state) {
  if (!($editor = bueditor_exec_editor_code($form_state['values']['code']))) {
    return form_error($form['code'], t('The editor code did not produce proper editor data.'));
  }
  $dirname = $form_state['values']['dirname'];
  if ($dirname) {
    $editor->custom_path = file_directory_path() .'/'. $dirname;
    if (!file_check_directory($editor->custom_path, 1, 'dirname')) {
      return FALSE;
    }
  }
  $editor->name =  $form_state['values']['name'];
  $editor->eid = $editor->overwrite = empty($form_state['values']['overwrite']) ? NULL : $form_state['values']['overwrite'];
  $form_state['editor'] = $editor;
}

/**
 * Editor import form submission.
 */
function bueditor_editor_import_submit($form, &$form_state) {
  if ($editor = bueditor_save_import($form_state['editor'])) {
    bueditor_message_added($editor->name);
    $form_state['redirect'] = 'admin/settings/bueditor/'. $editor->eid;
  }
}

/**
 * Save imported editor data. Save icons and library files into specified editor path.
 */
function bueditor_save_import($editor) {
  //this will fill in the blanks.
  $default = bueditor_editor_defaults('Noname');

  //buttons
  $buttons = bueditor_grab_imported_buttons($editor);

  //handle custom paths for icons and library files.
  if (isset($editor->custom_path) && file_check_directory($editor->custom_path, 1)) {
    $cpath = array(
      'tokened' => bueditor_path_tr($editor->custom_path, 'reverse'),
      'icons' => $editor->custom_path .'/icons',
      'library' => $editor->custom_path
    );
    //custom icons
    if (isset($editor->icons) && !empty($editor->icons) && file_check_directory($cpath['icons'], 1)) {
      foreach ($editor->icons as $name => $base64) {
        $filepath = $cpath['icons'] .'/'. $name;
        if (!file_exists($filepath)) {
          file_put_contents($filepath, base64_decode($base64));
          if (!@getimagesize($filepath)) {
            unlink($filepath);
          }
        }
      }
      $editor->iconpath = $cpath['tokened'] .'/icons';
    }
  }

  //library files
  if (isset($editor->library)) {
    if (empty($editor->library)) {
      $editor->librarypath = '';
    }
    else {
      $files = array();
      foreach ($editor->library as $tokenpath  => $content) {
        $filepath = bueditor_path_tr($tokenpath);
        if (file_exists($filepath)) {
          $files[] = $tokenpath;
        }
        elseif (isset($cpath) && $content) {
          $filename = basename($filepath);
          file_put_contents($cpath['library'] .'/'. $filename, $content);
          $files[] = $cpath['tokened'] .'/'. $filename;
        }
      }
      $editor->librarypath = implode("\n", $files);
    }
  }

  //set defaults for unset properties
  foreach ($default as $key => $value) {
    if (!isset($editor->$key)) {
      $editor->$key = $value;
    }
  }

  //get rid of some unwanted or possibly big variables
  unset($editor->custom_path, $editor->buttons, $editor->library, $editor->icons);
  if (empty($editor->overwrite)) {
    unset($editor->eid);
  }
  // New buttons shall replace the old ones.
  elseif (!empty($editor->eid)) {
    db_query("DELETE FROM {bueditor_buttons} WHERE eid = %d", $editor->eid);
  }

  //save and return the editor
  return bueditor_write_editor($editor, $buttons);
}

/**
 * Grab buttons from an imported editor data.
 */
function bueditor_grab_imported_buttons($editor) {
  $buttons = array();
  if (!isset($editor->buttons) || !is_array($editor->buttons) || empty($editor->buttons)) {
    return $buttons;
  }
  foreach ($editor->buttons as $button) {
    if (isset($button['title']) && $button['title']) {
      $buttons[] = (object) $button;
    }
  }
  return $buttons;
}

/**
 * Prepare and execute if there is any valid editor operation that doesn't require form submission.
 */
function bueditor_eop() {
  if (!isset($_GET['eop']) || !isset($_GET['token'])) {
    //Add editor name prompt for copy and add_default operations.
    //Confirm editor deletion without going to the confirmation page.
    return drupal_add_js(drupal_get_path('module', 'bueditor') . '/admin/bueditor.eop.js');
  }
  if (drupal_valid_token($_GET['token'], date('ymd'))) {
    $editors = bueditor_editors('all');
    $eop = $_GET['eop'];
    $name = isset($_GET['name']) ? check_plain($_GET['name']) : '';
    $editor = isset($_GET['eid']) && isset($editors[$_GET['eid']]) ? $editors[$_GET['eid']] : NULL;
    if ($eop == 'add_default') {
      bueditor_eop_add_default($name);
    }
    elseif ($editor) {
      switch ($eop) {
        case 'copy': bueditor_eop_copy($editor, $name); break;
        case 'delete': bueditor_eop_delete($editor); break;
        case 'export': bueditor_eop_export($editor); break;
      }
    }
  }
  drupal_goto('admin/settings/bueditor');
}

/**
 * Editor add_default operation.
 */
function bueditor_eop_add_default($ename) {
  if ($editor = bueditor_import_by_name('default', $ename)) {
    bueditor_message_added($editor->name);
  }
  return $editor;
}

/**
 * Editor copy operation.
 */
function bueditor_eop_copy($editor, $name) {
  $editor = drupal_clone($editor);
  $buttons = bueditor_buttons($editor->eid);
  $editor->eid = NULL;
  $editor->name = $name ? $name : 'Copy of '. $editor->name;
  array_walk($buttons, create_function('&$button', '$button->bid = NULL;'));
  if ($editor = bueditor_write_editor($editor, $buttons)) {
    bueditor_message_added($editor->name);
  }
  return $editor;
}

/**
 * Editor delete operation. Mimics editor delete form submission.
 */
function bueditor_eop_delete($editor) {
  $form = $form_state = array();
  $form['#parameters'][2] = $editor;
  bueditor_delete_form_submit($form, $form_state);
  return $editor;
}

/**
 * Editor export operation. Export the editor as text.
 */
function bueditor_eop_export($editor) {
  bueditor_export_text(bueditor_varexport_editor($editor), str_replace(' ', '.', $editor->name) .'.bueditor.txt');
}

/**
 * Generate an importable editor string including icon and library files.
 */
function bueditor_varexport_editor($editor) {
  $editor = (array) $editor;
  $library = $editor['librarypath'];
  $iconpath = bueditor_path_tr($editor['iconpath']);
  $buepath = drupal_get_path('module', 'bueditor');

  //include buttons
  $editor['buttons'] = bueditor_exportable_buttons(bueditor_buttons($editor['eid']));

  //include icons if they are not in default icon directory.
  $editor['icons'] = array();
  if ($iconpath != $buepath .'/icons') {
    foreach (bueditor_icons($iconpath) as $name => $value) {
      $editor['icons'][$name] = base64_encode(file_get_contents($iconpath .'/'. $name));
    }
  }

  //include library files if they are not in default library directory.
  $editor['library'] = array();
  $buelib = $buepath .'/library';
  foreach (bueditor_get_library($library) as $key => $filepath) {
    $editor['library'][$key] = $buelib == dirname($filepath) ? '' : file_get_contents($filepath);
  }

  unset($editor['eid'], $editor['librarypath'], $editor['spritename']);
  return var_export($editor, TRUE);
}

/**
 * Make buttons exportable by converting them into arrays.
 */
function bueditor_exportable_buttons($buttons) {
  $new_buttons = array();
  if (is_array($buttons)) {
    foreach ($buttons as $bid => $button) {
      unset($button->bid, $button->eid);
      $new_buttons[] = (array) $button;
    }
  }
  return $new_buttons;
}

/**
 * Execute the given code to get editor data.
 */
function bueditor_exec_editor_code($code) {
  $editor = eval('return '. $code .';');
  if (is_array($editor)) {
    return (object) $editor;
  }
  return FALSE;
}

/**
 * Update/insert an editor.
 */
function bueditor_write_editor($editor, $buttons = array()) {
  if (isset($editor->spriteon) && $editor->spriteon) {
    include_once './' . drupal_get_path('module', 'bueditor') . '/admin/bueditor.sprite.inc';
    $editor->spritename = bueditor_sprite($editor, $buttons);
  }
  if (drupal_write_record('bueditor_editors', $editor, bueditor_isupdate($editor, 'eid'))) {
    foreach ($buttons as $button) {
      $button->eid = $editor->eid;
      drupal_write_record('bueditor_buttons', $button, bueditor_isupdate($button, 'bid'));
    }
    return $editor;
  }
  return FALSE;
}

/**
 * Load icons in the path
 */
function bueditor_icons($path) {
  $icons = file_scan_directory($path, '\.(png|gif|jpg)$', array('.', '..', 'CVS'), 0, 0, 'basename');
  array_walk($icons, create_function('&$value', '$value = 1;'));
  return $icons;
}

/**
 * Sort roles according to their weights.
 */
function bueditor_sorted_roles() {
  static $sorted;
  if (!isset($sorted)) {
    $sorted = array();
    $roles = user_roles();
    $broles = variable_get('bueditor_roles', array());
    $broles[DRUPAL_ANONYMOUS_RID]['weight'] = 12;
    $broles[DRUPAL_AUTHENTICATED_RID]['weight'] = 11;
    $keys = array('weight', 'editor', 'alt');
    foreach ($roles as $rid => $name) {
      $sorted[$rid]['name'] = $name;
      foreach ($keys as $key) {
        $sorted[$rid][$key] = empty($broles[$rid][$key]) ? 0 : (int) $broles[$rid][$key];
      }
    }
    uasort($sorted, 'bueditor_rolesort');
  }
  return $sorted;
}

/**
 * user sorting function for roles.
 */
function bueditor_rolesort($r1, $r2) {
  return $r1['weight']-$r2['weight'];
}

/**
 * Return update state for an object to be used with drupal_write_record.
 */
function bueditor_isupdate($obj, $key) {
  return isset($obj->$key) && is_numeric($obj->$key) && $obj->$key > 0 ? array($key) : array();
}

/**
 * Default values of a new editor.
 */
function bueditor_editor_defaults($name = '') {
  return (object) array(
    'name' => $name,
    'pages' => "node/*\ncomment/*",
    'excludes' => 'edit-log',
    'iconpath' => '%BUEDITOR/icons',
    'librarypath' => '%BUEDITOR/library/bue.min.default.js',
    'spriteon' => 0,
    'spritename' => '',
  );
}

/**
 * Default values of a new button.
 */
function bueditor_button_defaults($title = '') {
  return (object) array(
    'title' => $title,
    'content' => '',
    'icon' => '',
    'accesskey' => '',
    'weight' => 0
  );
}

/**
 * Load buttons by ids.
 */
function bueditor_buttons_by_id($bids = array()) {
  $sql = 'SELECT * FROM {bueditor_buttons} WHERE bid IN ('. db_placeholders($bids) .') ORDER BY weight, title';
  return empty($bids) ? array() : bueditor_query_buttons($sql, $bids);
}

/**
 * Sends the text to the client.
 */
function bueditor_export_text($text, $filename) {
  drupal_set_header('Content-type: text/plain; charset=utf-8');
  drupal_set_header('Content-Disposition: attachment; filename='. $filename);
  print $text;
  exit();
}

/**
 * Set editor addition message.
 */
function bueditor_message_added($name) {
  drupal_set_message(t('Editor %name has been added.', array('%name' => $name)));
}

/**
 * Allow quick import of predefined editors by ajax.
 */
function bueditor_set_quick_import() {
  $path = drupal_get_path('module', 'bueditor');
  $setting['BUE']['imp'] = array(
    'editors' => bueditor_importable_names(),
    'dir' => $path . '/import/',
  );
  drupal_add_js($path . '/admin/bueditor.import.js');
  drupal_add_js($setting, 'setting');
}

/**
 * Return the list of importable editor names.
 */
function bueditor_importable_names() {
  return array('default', 'bbcode', 'commenter', 'lab');
}

/**
 * Import an editor from 'bueditor/import' directory.
 */
function bueditor_import_by_name($name, $ename = '') {
  $file = drupal_get_path('module', 'bueditor') . '/import/' . $name . '.bueditor.txt';
  if ($code = file_get_contents($file)) {
    if ($editor = bueditor_exec_editor_code($code)) {
      if ($ename) {
        $editor->name = $ename;
      }
      return bueditor_save_import($editor);
    }
  }
  return FALSE;
}

/**
 * Import all from 'bueditor/import' directory.
 */
function bueditor_import_all() {
  foreach (bueditor_importable_names() as $name) {
    bueditor_import_by_name($name);
  }
}

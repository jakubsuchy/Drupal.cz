<?php

/**
 * @file
 * Implements commonly used functions for bueditor.
 */

/**
 * Editor(s).
 */
function bueditor_editors($eid = 0) {
  if (empty($eid)) {
    return FALSE;
  }
  static $editors = array(), $gotall = FALSE;
  if ($eid === 'all') {
    if (!$gotall) {
      $gotall = TRUE;
      $result = db_query("SELECT * FROM {bueditor_editors} ORDER BY name");
      while ($editor = db_fetch_object($result)) {
        $editors[$editor->eid] = $editor;
      }
    }
    return $editors;
  }
  elseif (!isset($editors[$eid]) && !$gotall && is_numeric($eid) && $eid > 0) {
    $editors[$eid] = db_fetch_object(db_query("SELECT * FROM {bueditor_editors} WHERE eid = %d", $eid));
  }
  return isset($editors[$eid]) ? $editors[$eid] : FALSE;
}

/**
 * All buttons of an editor.
 */
function bueditor_buttons($eid) {
  $sql = 'SELECT * FROM {bueditor_buttons} WHERE eid = %d ORDER BY weight, title';
  return $eid ? bueditor_query_buttons($sql, $eid) : array();
}

/**
 * Fetch buttons by an sql query.
 */
function bueditor_query_buttons($sql, $var = array()) {
  $buttons = array();
  $result = db_query($sql, $var);
  while ($button = db_fetch_object($result)) {
    $buttons[$button->bid] = $button;
  }
  return $buttons;
}

/**
 * Processed buttons. Evaluate php code for php buttons and translate titles prefixed with t:.
 */
function bueditor_processed_buttons($eid) {
  $buttons = array();
  foreach (bueditor_buttons($eid) as $bid => $button) {
    if (($content = $button->content) && substr($content, 0, 4) == 'php:') {
      if (!$content = drupal_eval('<?php '. substr($content, 4) .' ?>')) {
        //php returned false or nothing. include empty button in order not to break sprite index.
        if (substr($button->title, 0, 4) != 'tpl:' && preg_match('/\.(png|gif|jpg)$/', $button->icon)) {
          $buttons[] = array();
        }
        continue;
      }
    }
    $title = substr($button->title, 0, 2) == 't:' ? call_user_func('t', trim(substr($button->title, 2))) : $button->title;
    $buttons[] = array($title, $content, $button->icon, $button->accesskey);
  }
  return $buttons;
}

/**
 * Integrate the editor into textareas.
 */
function _bueditor_textarea($element) {
  static $editors, $textareas = array();

  if (isset($textareas[$element['#id']])) {
    return $element;
  }
  $textareas[$element['#id']] = TRUE;

  //get editors
  if (!isset($editors)) {
    $editors = bueditor_user_eids($GLOBALS['user']);
    $editors[0] = bueditor_check_page($_GET['q'], $editors[0]);
  }

  //if the first editor does not settle try the second.
  if (!bueditor_preset_textarea($element['#id'], $editors[0]) && $editors[1]) {
    if (!isset($editors[1]->eid)) {
      $editors[1] = bueditor_check_page($_GET['q'], $editors[1]);
    }
    bueditor_preset_textarea($element['#id'], $editors[1]);
  }

  return $element;
}

/**
 * Insert textarea id into preset of the editor.
 */
function bueditor_preset_textarea($tid, $editor) {
  if ($editor && !bueditor_check_match($editor->excludes, $tid)) {
    bueditor_settle($editor);
    $settings['BUE']['preset'][$tid] = 'e'. $editor->eid;
    drupal_add_js($settings, 'setting');
    return TRUE;
  }
  return FALSE;
}

/**
 * Include necessary js and css files for editor settlement.
 */
function _bueditor_settle($editor) {
  static $settled = array();
  if (is_numeric($editor)) {
    $editor = bueditor_editors($editor);
  }
  if ($editor && $editor->eid && !isset($settled[$editor->eid])) {
    $settings = array();
    $path = drupal_get_path('module', 'bueditor');
    if (empty($settled)) {//run once
      drupal_add_css($path .'/bueditor.css');
      drupal_add_js($path .'/bueditor.js');
      if (module_invoke('imce', 'access')) {
        $settings['BUE']['imceURL'] = url('imce');
      }
      module_invoke('ajax_markup', 'on');
    }
    $editor->iconpath = bueditor_path_tr($editor->iconpath);
    bueditor_add_library($editor->librarypath);//load library files.
    $settings['BUE']['templates']['e'. $editor->eid] = array(
      'iconpath' => base_path() . $editor->iconpath,
      'buttons' => bueditor_processed_buttons($editor->eid),
    );
    if ($editor->spriteon && $editor->spritename) {
      $sprite_path = bueditor_sprites_dir() . '/' . $editor->spritename;
      if (file_exists($sprite_path) && ($info = getimagesize($sprite_path))) {
        $settings['BUE']['templates']['e'. $editor->eid]['sprite'] = array(
          'url' => file_create_url($sprite_path),
          'unit' => $info[1],
          'x1' => base_path() . $path . '/icons/x1.png',
        );
      }
    }
    drupal_add_js($settings, 'setting');
    $settled[$editor->eid] = TRUE;
  }
  return $editor && isset($settled[$editor->eid]);
}

/**
 * Include js and css files from library.
 */
function bueditor_add_library($library) {
  foreach (bueditor_get_library($library) as $key => $file) {
    $ext = substr($file, -4);
    if ($ext == '.css') {
      drupal_add_css($file, 'theme');
    }
    elseif (substr($ext, 1) == '.js') {
      drupal_add_js($file, 'theme');
    }
  }
}

/**
 * Get an array of js and css files defined in editor library.
 */
function bueditor_get_library($library) {
  $files = array();
  foreach (preg_split("/\s+/", $library) as $path) {
    if ($path && is_file($path_tr = bueditor_path_tr($path))) {
      $files[$path] = $path_tr;
    }
  }
  return $files;
}

/**
 * Return the editor ids assigned to the user.
 */
function bueditor_user_eids($user) {
  //user #1
  if ($user->uid == 1) {
    return array(variable_get('bueditor_user1', 1), variable_get('bueditor_user1_alt', 0));
  }
  $roles = variable_get('bueditor_roles', array());
  //anonymous user
  if (empty($user->uid)) {
    $rid = DRUPAL_ANONYMOUS_RID;
    return isset($roles[$rid]) ? array($roles[$rid]['editor'], $roles[$rid]['alt']) : array('', '');
  }
  //other users
  foreach ($roles as $rid => $role) {
    if (isset($user->roles[$rid]) && ($role['editor'] || $role['alt'])) {
      return array($role['editor'], $role['alt']);
    }
  }
  return array('', '');
}

/**
 * Check if the editor is visible in the page.
 */
function bueditor_check_page($page, $editor) {
  $editor = is_numeric($editor) ? bueditor_editors($editor) : $editor;
  if ($editor) {
    if (drupal_match_path($page, $editor->pages)) {
      return $editor;
    }
    $alias = drupal_get_path_alias($page);
    if ($alias != $page && drupal_match_path($alias, $editor->pages)) {
      return $editor;
    }
    if (arg(0) == 'node' && arg(2) == 'edit') {
      $node = node_load(arg(1));
      if ($node && drupal_match_path('node/add/'.  str_replace('_', '-', $node->type), $editor->pages)) {
        return $editor;
      }
    }
  }

  return FALSE;
}

/**
 * Check matching lines of the needle in haystack.(page and textarea id)
 */
function bueditor_check_match($needle, $haystack) {
  if ($needle == '') {
    return FALSE;
  }
  $needle = '/^'. preg_replace("/\r\n?|\n/", '|', str_replace(array('*', '-', '/'), array('.*', '\\-', '\\/'), trim($needle))) .'$/';
  return preg_match($needle, $haystack);
}

/**
 * Translate editor paths.
 */
function bueditor_path_tr($path, $reverse = FALSE) {
  if (!$path) {
    return $path;
  }
  static $tokens;
  if (!isset($tokens)) {
    $tokens = array(
      '%BUEDITOR' => drupal_get_path('module', 'bueditor'),
      '%FILES' => file_directory_path(),
      '%THEME' => isset($GLOBALS['theme']) ? drupal_get_path('theme', $GLOBALS['theme']) : path_to_theme(),
    );
  }
  if ($reverse) {
    return strtr($path, array_flip($tokens));
  }
  $trpath = strtr($path, $tokens);
  //for themes missing icon or library directory, switch to default theme.
  if (!file_exists($trpath) && substr($path, 0, 6) == '%THEME') {
    $trpath = str_replace('%THEME', drupal_get_path('theme', variable_get('theme_default', 'garland')), $path);
  }
  return $trpath;
}

/**
 * Returns sprites directory.
 */
function bueditor_sprites_dir() {
  return file_directory_path() . '/' . variable_get('bueditor_sprites_dir', 'bueditor-sprites');
}
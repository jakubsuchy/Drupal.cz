<?php
// $Id: path_redirect.admin.inc,v 1.1.2.44 2009/06/27 22:24:27 davereid Exp $

/**
 * @file
 * Administrative page callbacks for the path_redirect module.
 */

/**
 * Render a list of redirects for the main admin page.
 */
function path_redirect_admin() {
  $multilanguage = (module_exists('locale') || db_result(db_query("SELECT rid FROM {path_redirect} WHERE language <> ''")));
  $languages = language_list();

  $header = array(
    array('data' => t('From'), 'field' => 'path', 'sort' => 'asc'),
    array('data' => t('To'), 'field' => 'redirect'),
    array('data' => t('Type'), 'field' => 'type'),
    array('data' => t('Operations'), 'colspan' => '2'),
  );
  if ($multilanguage) {
    array_splice($header, 3, 0, array(array('data' => t('Language'), 'field' => 'language')));
  }
  $rows = array();
  $limit = 50;

  $redirects = pager_query('SELECT rid, path, redirect, query, fragment, language, type FROM {path_redirect}'. tablesort_sql($header), $limit);
  while ($r = db_fetch_object($redirects)) {
    $redirect = url($r->redirect, array('query' => $r->query, 'fragment' => $r->fragment, 'absolute' => TRUE, 'alias' => TRUE));
    $row = array(
      'data' => array(
        // @todo: Revise the following messy, confusing line.
        l($r->path, preg_replace('/[\?\&].*/', '', $r->path), array('query' => strstr($r->path, '?') ? preg_replace('/.*\?/', '', $r->path) : NULL, 'language' => $r->language ? $languages[$r->language] : NULL)),
        // @todo: Fix sorting on the redirect field
        l($redirect, $redirect, array('external' => TRUE)),
        $r->type,
        l(t('Edit'), 'admin/build/path-redirect/edit/'. $r->rid, array('query' => drupal_get_destination())),
        l(t('Delete'), 'admin/build/path-redirect/delete/'. $r->rid, array('query' => drupal_get_destination())),
      ),
    );
    if ($multilanguage) {
      array_splice($row['data'], 3, 0, module_invoke('locale', 'language_name', $r->language));
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No redirects available.'), 'colspan' => $multilanguage ? 5 : 4));
  }

  $output = theme('table', $header, $rows, array('class' => 'path-redirects'));
  $output .= theme('pager', NULL, $limit);

  return $output;
}

/**
 * Callback for add and edit pages.
 *
 * @return
 *   A form for drupal_get_form.
 */
function path_redirect_edit($form_state, $rid = FALSE) {
  if ($rid) {
    $redirect = (array) path_redirect_load($rid);
    drupal_set_title(check_plain($redirect['path']));
    $output = path_redirect_edit_form($form_state, $redirect);
  }
  else {
    drupal_set_title(t('Add redirect'));
    $output = path_redirect_edit_form($form_state);
  }
  return $output;
}

function path_redirect_edit_form(&$form_state, $edit = array()) {
  // Merge default values.
  $edit += array(
    'rid' => NULL,
    'path' => isset($_GET['src']) ? urldecode($_GET['src']) : '',
    'redirect' => isset($_GET['dst']) ? urldecode($_GET['dst']) : '',
    'query' => NULL,
    'fragment' => NULL,
    'language' => isset($_GET['language']) ? urldecode($_GET['language']) : '',
    'type' => path_redirect_var('default_status'),
  );

  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('From'),
    '#description' => t("Enter an interal Drupal path or path alias to redirect (e.g. %example1 or %example2). Fragment anchors (e.g. %anchor) are <strong>not</strong> allowed.", array('%example1' => 'node/123', '%example2' => 'taxonomy/term/123', '%anchor' => '#anchor')),
    '#size' => 42,
    '#maxlength' => 255,
    '#default_value' => $edit['path'],
    '#required' => TRUE,
    '#field_prefix' => url('', array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
    '#autocomplete_path' => db_table_exists('watchdog') ? 'js/path_redirect/autocomplete_404' : '',
  );

  $form['redirect'] = array(
    '#type' => 'item',
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
    '#title' => t('To'),
    '#description' => '<div style="display:block">'. t('Enter an interal Drupal path, path alias, or complete external URL (like http://example.com/) to redirect to. Use %front to redirect to the front page. Enter (optional) queries after "?" and (optional) anchor after "#". Most redirects will not contain queries or fragment anchors.', array('%front' => '<front>')) .'</div>',
  );

  $form['redirect']['redirect'] = array(
    '#type' => 'textfield',
    '#size' => 30,
    '#maxlength' => 255,
    '#default_value' => $edit['redirect'],
    //'#required' => TRUE,
  );

  $form['redirect'][] = array(
    '#value' => '?',
  );

  $form['redirect']['query'] = array(
    '#type' => 'textfield',
    '#size' => 12,
    '#maxlength' => 255,
    '#default_value' => $edit['query'],
  );

  $form['redirect'][] = array(
    '#value' => '#',
  );

  $form['redirect']['fragment'] = array(
    '#type' => 'textfield',
    '#size' => 12,
    '#maxlength' => 50,
    '#default_value' => $edit['fragment'],
  );

  $form[] = array(
    '#value' => "<p> </p>", // little bit of extra space
  );

  $form['language'] = array(
    '#type' => 'value',
    '#title' => t('Language'),
    '#options' => array('' => t('All languages')),
    '#default_value' => $edit['language'],
    '#description' => t('A redirect set for a specific language will always be used when requesting this page in that language, and takes precedence over redirects set for <em>All languages</em>.'),
  );
  if (module_exists('locale')) {
    $form['language']['#type'] = 'select';
    $form['language']['#options'] += locale_language_list('name');
  }

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $status_options = array();
  $status_descriptions = array();
  foreach (path_redirect_status_codes() as $code => $details) {
    $status_options[$code] = $details['title'];
    $status_descriptions[] = $details['title'] .': '. $details['description'];
  }
  $form['advanced']['type'] = array(
    '#type' => 'select',
    '#title' => t('Redirect status'),
    '#description' => theme('item_list', $status_descriptions) . t('You can find more information about HTTP redirect status codes at <a href="@link">@link</a>.', array('@link' => 'http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3')),
    '#default_value' => $edit['type'],
    '#options' => $status_options,
  );

  $form['rid'] = array('#type' => 'value', '#value' => $edit['rid']);
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  $form['cancel'] = array('#value' => l(t('Cancel'), 'admin/build/path-redirect'));
  $form['#redirect'] = 'admin/build/path-redirect';

  return $form;
}

function path_redirect_edit_validate($form, &$form_state) {
  if (strpos($form_state['values']['path'], '#') !== FALSE) {
    // Check that the "from" path is valid and contains no # fragment
    form_set_error('path', t('You cannot redirect from a fragment anchor.'));
  }
  //elseif (!valid_url($form_state['values']['path'])) {
  //  //Make sure "from" has the form of a local Drupal path
  //  form_set_error('path', t('The redirect <strong>from</strong> path does not appear valid. This must be a local Drupal path.'));
  //}
  elseif (($result = path_redirect_load(array('path' => $form_state['values']['path']))) && $form_state['values']['rid'] !== $result->rid) {
    // The "from" path should not conflict with another redirect
    form_set_error('path', t('The <strong>from</strong> path you entered is already redirected. You can <a href="@edit-page">edit this redirect</a> instead.', array('@edit-page' => url('admin/build/path-redirect/edit/'. $result->rid))));
  }
  elseif ($pid = db_result(db_query("SELECT pid FROM {url_alias} WHERE dst = '%s'", array(':dst' => $form_state['values']['path'])))) {
    // A redirect's 'from' cannot match any values from url_alias, it will cause an infinite loop.
    form_set_error('path', t('You cannot add an existing alias as a redirect as it will not work. You must <a href="@alias-delete">delete the alias</a> first.', array('@alias-delete' => url('admin/build/path/delete/' . $pid, array('query' => drupal_get_destination())))));
  }
  elseif (menu_get_item($form_state['values']['path'])) {
    form_set_error('path', t('You cannot create a redirect from a currently valid path.'));
  }

  if (!valid_url($form_state['values']['redirect']) && !valid_url($form_state['values']['redirect'], TRUE) && $form_state['values']['redirect'] != '<front>') {
    form_set_error('redirect', t('The redirect <strong>to</strong> path does not appear valid.'));
  }

  if ($form_state['values']['redirect'] == '<front>') {
    $form_state['values']['redirect'] = variable_get('site_frontpage', 'node');
  }

  // check that there there are no redirect loops
  if ($form_state['values']['path'] === $form_state['values']['redirect']) {
    form_set_error('redirect', t('You are attempting to redirect the page to itself. This will result in an infinite loop.'));
  }

  // Allow spaces in "from" path
  $form_state['values']['path'] = str_replace("+", " ", $form_state['values']['path']);
}

function path_redirect_edit_submit($form, &$form_state) {
  path_redirect_save($form_state['values']);
  drupal_set_message(t('Redirect has been saved.'));
}

function path_redirect_delete_confirm($form_state, $rid) {
  $form['rid'] = array(
    '#type' => 'value',
    '#value' => $rid,
  );
  $redirect = path_redirect_load($rid);
  return confirm_form($form,
    t('Are you sure you want to delete the redirect from %path to %redirect?', array('%path' => $redirect->path, '%redirect' => $redirect->redirect)),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/build/path-redirect');
}

function path_redirect_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    path_redirect_delete($form_state['values']['rid']);
    drupal_set_message(t('The redirect has been deleted.'));
  }
}

/**
 * Form builder; administrative settings for the module.
 *
 * @see system_settings_form()
 */
function path_redirect_settings_form() {
  $form['path_redirect_redirect_warning'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display a warning message to users when they are redirected.'),
    '#default_value' => path_redirect_var('redirect_warning'),
  );

  $form['path_redirect_allow_bypass'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to bypass redirects by adding %code to the URL.', array('%code' => variable_get('clean_url', 0) ? '?redirect=no' : '&redirect=no')),
    '#default_value' => path_redirect_var('allow_bypass'),
  );

  $form['path_redirect_auto_redirect'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically create redirects when URL aliases are changed.'),
    '#default_value' => path_redirect_var('auto_redirect'),
    '#access' => module_exists('path'),
  );

  $form['path_redirect_purge_inactive'] = array(
    '#type' => 'select',
    '#title' => t('Discard redirects that have not been accessed for'),
    '#default_value' => path_redirect_var('purge_inactive'),
    '#options' => array(0 => t('Never (do not discard)')) + drupal_map_assoc(array(604800, 1209600, 2419200, 4838400, 7257600, 9676800, 31536000), 'format_interval'),
  );

  $status_options = array();
  $status_descriptions = array();
  foreach (path_redirect_status_codes() as $code => $details) {
    $status_options[$code] = $details['title'];
    $status_descriptions[] = $details['title'] .': '. $details['description'];
  }
  $form['path_redirect_default_status'] = array(
    '#type' => 'select',
    '#title' => t('Default redirect status'),
    '#description' => theme('item_list', $status_descriptions) . t('You can find more information about HTTP redirect status codes at <a href="@link">@link</a>.', array('@link' => 'http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3')),
    '#default_value' => path_redirect_var('default_status'),
    '#options' => $status_options,
  );

  return system_settings_form($form);
}

/**
 * Return an array of 300-range status codes.
 */
function path_redirect_status_codes() {
  return array(
    300 => array(
      'title' => t('300 Multiple Choices'),
      'description' => t('The request is ambiguous and needs clarification as to which resource was requested.'),
    ),
    301 => array(
      'title' => t('301 Moved Permanently'),
      'description' => t('Moved Permanently. The resource has permanently moved elsewhere, the response indicates where it has gone to. <strong>Recommended.</strong>'),
    ),
    302 => array(
      'title' => t('302 Found'),
      'description' => t("The resource has temporarily moved elsewhere, the response indicates where it is at present. <strong>This is Drupal's default redirect type.</strong>"),
    ),
    303 => array(
      'title' => t('303 See Other'),
      'description' => t('See Other/Redirect. A preferred alternative source should be used at present.'),
    ),
    304 => array(
      'title' => t('304 Not Modified'),
      'description' => t("The server has identified from the request information that the client's copy of the information is up-to-date and the requested information does not need to be sent again."),
    ),
    305 => array(
      'title' => t('305 Use Proxy'),
      'description' => t('The request must be sent through the indicated proxy server.'),
    ),
    307 => array(
      'title' => t('307 Temporary Redirect'),
      'description' => t('The resource has temporarily moved elsewhere, the response indicates where it is at present. Client should still use this URL.'),
    ),
  );
}

/*
 * Extract part of the current Drupal path from a certain 'argument' onward.
 *
 * @todo Replace with drupal_get_path_segment in Drupal 7.
 */
function path_redirect_get_path_segment($index = 0) {
  $path = trim($_GET['q']);
  $path = explode('/', $path, $index + 1);
  $path = (count($path) > $index ? end($path) : '');
  return $path;
}

/**
 * Autocompletion callback for the add/edit redirect form. Returns a list of
 * current 404s on the site.
 */
function path_redirect_js_autocomplete_404() {
  $string = path_redirect_get_path_segment(3);
  $matches = array();

  // Get a list of 404s, sorted by the number of times each 404 was processed.
  $paths = db_query("SELECT message, COUNT(message) AS count FROM {watchdog} WHERE type = 'page not found' AND LOWER(message) LIKE '%%%s%%' GROUP BY message ORDER BY count DESC", drupal_strtolower($string));
  while ($path = db_result($paths)) {
    // If the 404 is now a valid path or already has a redirect, discard it.
    if (!menu_get_item($path) && !path_redirect_load(array('path' => $path))) {
      $matches[$path] = check_plain($path);
    }
  }

  // Limit the output to 10 results and return the JSON.
  $matches = array_slice($matches, 0, 10);
  drupal_json($matches);
}

/**
 * Form builder; an import form for redirects.
 *
 * @see path_redirect_import_form_submit()
 */
function path_redirect_import_form() {
  $form['file'] = array(
    '#type' => 'file',
    '#title' => t('Upload import file'),
    '#size' => 50,
  );
  $form['type'] = array(
    '#type' => 'radios',
    '#title' => t('File format'),
    '#options' => array(
      'auto' => t('Auto detect'),
      'csv' => t('CSV (Tab-delimited)'),
      'xml' => t('XML'),
    ),
    '#default_value' => 'auto',
    '#process' => array('expand_radios', '_path_redirect_process_import_export_options'),
  );
  $form['mode'] = array(
    '#type' => 'radios',
    '#title' => t('Mode'),
    '#description' => t('What action should be taken when there are duplicate redirects?'),
    '#options' => array(
      'overwrite' => t('Redirects in the uploaded file replace existing ones, new ones are added.'),
      'keep' => t('Existing redirects are kept, only new redirects are added.')
    ),
    '#default_value' => 'keep',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
  );
  $form['#attributes']['enctype'] = 'multipart/form-data';

  return $form;
}

/**
 * Handles redirect import.
 *
 * @see path_redirect_import_form()
 * @see path_redirect_import_csv()
 * @see path_redirect_import_xml()
 */
function path_redirect_import_form_submit($form, $form_state) {
  if ($file = file_save_upload('file')) {
    $type = $form_state['values']['type'];

    if ($type == 'auto') {
      if ($file->filemime == 'text/csv' || substr($file->filename, -4) == '.csv') {
        $type = 'csv';
      }
      elseif (strpos($file->filemime, 'xml') !== FALSE || substr($file->filename, -4) == '.xml') {
        $type = 'xml';
      }
      else {
        drupal_set_message(t('Could not determine the correct file type. Please select the file format manually and check the import file.'), 'error');
        return;
      }
    }

    $function = 'path_redirect_import_'. $type;
    if ($text = @file_get_contents($file->filepath)) {
      $function($text);
    }
    else {
      drupal_set_message(t('Could not import file.'), 'error');
    }
  }
  else {
    drupal_set_message(t('Import file not found.'), 'error');
  }
}

/**
 * Imports redirects from a CSV file.
 *
 * @see path_redirect_import_form_submit()
 */
function path_redirect_import_csv($file) {
}

/**
 * Imports redirects from an XML file.
 *
 * @see path_redirect_import_form_submit()
 */
function path_redirect_import_xml($file) {
  $imports = simplexml_load_string($file);
}

/**
 * Form builder;
 */
function path_redirect_export_form() {
  if (!db_result(db_query("SELECT COUNT(*) FROM {path_redirect}"))) {
    drupal_set_message(t('No redirects available.'));
    return;
  }

  $form['type'] = array(
    '#type' => 'radios',
    '#title' => t('Export format'),
    '#options' => array(
      'csv' => t('CSV (Tab-delimited)'),
      'xml' => t('XML'),
    ),
    '#default_value' => 'csv',
    '#process' => array('expand_radios', '_path_redirect_process_import_export_options'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Export'),
  );

  return $form;
}

/**
 * Handles redirect exporting.
 *
 * @see path_redirect_export_form()
 * @see path_redirect_export_csv()
 * @see path_redirect_export_xml()
 */
function path_redirect_export_form_submit($form, $form_state) {
  $type = $form_state['values']['type'];
  $function = 'path_redirect_export_'. $type;

  $filename = variable_get('site_name', 'Drupal') .' redirects '. format_date(REQUEST_TIME, 'custom', 'Y m d');
  $filename = preg_replace(array('/^\W+|\W+$/', '/\W+/'), array('', '-'), drupal_strtolower($filename));
  $filename .= '.'. $type;
  drupal_set_header('Content-Disposition: attachment; filename='. $filename);

  echo $function();
  drupal_page_footer();
  exit();
}

/**
 * Exports redirects to a CSV format.
 *
 * @see path_redirect_export_form_submit()
 */
function path_redirect_export_csv() {
  drupal_set_header('Content-Type: text/csv; charset=utf-8');
  $fields = array('path', 'redirect', 'query', 'fragment', 'language', 'type', 'last_used');
  $redirects = db_query("SELECT ". implode(', ', $fields) ." FROM {path_redirect}");
  $export = implode("\t", $fields) . PHP_EOL;
  while ($redirect = db_fetch_array($redirects)) {
    $export .= implode("\t", $redirect) . PHP_EOL;
  }
  return $export;
}

/**
 * Exports redirects to a XML format.
 *
 * @see path_redirect_export_form_submit()
 */
function path_redirect_export_xml() {
  drupal_set_header('Content-Type: text/xml; charset=utf-8');
  //$export = simplexml_load_string();
  $export = new SimpleXMLElement("<?xml version=\"1.0\" encoding=\"utf-8\"?><redirects />");
  $redirects = db_query("SELECT path, redirect, query, fragment, language, type, last_used FROM {path_redirect}");
  while ($redirect = db_fetch_object($redirects)) {
    $export_item = $export->addChild('redirect');
    foreach ($redirect as $key => $value) {
      if ($value != '' && $value !== NULL) {
        $export_item->addAttribute($key, $value);
      }
    }
  }
  return $export->asXML();
}

/**
 * Disables the XML import/export options
 *
 * @todo Remove in Drupal 7.
 */
function _path_redirect_process_import_export_options($element) {
  if (!extension_loaded('simplexml')) {
    $element['xml']['#disabled'] = TRUE;
    $element['xml']['#description'] = t('This option requires PHP 5.1.');
  }
  return $element;
}

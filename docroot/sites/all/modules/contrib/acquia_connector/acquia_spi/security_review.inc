<?php

/**
 * @file
 * Stand-alone security checks and review system.
 */

/**
 * Public function for running Security Review checklist and returning results.
 *
 * @param array $checklist Array of checks to run, indexed by module namespace.
 * @param boolean $log Whether to log check processing using security_review_log.
 * @param boolean $help Whether to load the help file and include in results.
 * @return array Results from running checklist, indexed by module namespace.
 */
function acquia_spi_security_review_run($checklist = NULL, $log = FALSE, $help = FALSE) {
  // Use Security Review module if available.
  if (module_exists('security_review') && function_exists('security_review_run')) {
    if (!$checklist) {
      $checklist = module_invoke_all('security_checks');
    }
    module_load_include('inc', 'security_review');
    return security_review_run($checklist, $log, $help);
  }
  else {
    return _acquia_spi_security_review_run($checklist, $log);
  }
}

/**
 * Private function run review and returns the full results.
 */
function _acquia_spi_security_review_run($checklist, $log = FALSE) {
  $results = array();
  foreach ($checklist as $module => $checks) {
    foreach ($checks as $check_name => $arguments) {
      $check_result = _acquia_spi_security_review_run_check($module, $check_name, $arguments, $log);
      if (!empty($check_result)) {
        $results[$module][$check_name] = $check_result;
      }
    }
  }
  return $results;
}

/**
 * Run a single Security Review check.
 */
function _acquia_spi_security_review_run_check($module, $check_name, $check, $log, $store = FALSE) {
  $last_check = array();

  $check_result = array();
  $return = array('result' => NULL);
  if (isset($check['file'])) {
    // Handle Security Review defining checks for other modules.
    if (isset($check['module'])) {
      $module = $check['module'];
    }
    module_load_include('inc', $module, $check['file']);
  }
  $function = $check['callback'];
  if (function_exists($function)) {
    $return = call_user_func($function, $last_check);
  }
  $check_result = array_merge($check, $return);
  $check_result['lastrun'] = time();

  if ($log && !is_null($return['result'])) { // Do not log if result is NULL.
    $variables = array('!name' => $check_result['title']);
    if ($check_result['result']) {
      _acquia_spi_security_review_log($module, $check_name, '!name check passed', $variables, WATCHDOG_INFO);
    }
    else {
      _acquia_spi_security_review_log($module, $check_name, '!name check failed', $variables, WATCHDOG_ERROR);
    }
  }
  return $check_result;
}

/**
 * Checks for acquia_spi_security_review_get_checks().
 */
function _acquia_spi_security_review_security_checks() {
  $checks['file_perms'] = array(
    'title' => t('File system permissions'),
    'callback' => 'acquia_spi_security_review_check_file_perms',
    'success' => t('Drupal installation files and directories (except required) are not writable by the server.'),
    'failure' => t('Some files and directories in your install are writable by the server.'),
  );
  $checks['input_formats'] = array(
    'title' => t('Input formats'),
    'callback' => 'acquia_spi_security_review_check_input_formats',
    'success' => t('Untrusted users are not allowed to input dangerous HTML tags.'),
    'failure' => t('Untrusted users are allowed to input dangerous HTML tags.'),
  );
  $checks['nodes'] = array(
    'title' => t('Content'),
    'callback' => 'acquia_spi_security_review_check_nodes',
    'success' => t('Dangerous tags were not found in the body of any nodes.'),
    'failure' => t('Dangerous tags were found in the body of nodes.'),
  );
  $checks['comments'] = array(
    'title' => t('Comments'),
    'callback' => 'acquia_spi_security_review_check_comments',
    'success' => t('Dangerous tags were not found in any comments.'),
    'failure' => t('Dangerous tags were found in comments.'),
  );
  $checks['error_reporting'] = array(
    'title' => t('Error reporting'),
    'callback' => 'acquia_spi_security_review_check_error_reporting',
    'success' => t('Error reporting set to log only.'),
    'failure' => t('Errors are written to the screen.'),
  );
  $checks['private_files'] = array(
    'title' => t('Private files'),
    'callback' => 'acquia_spi_security_review_check_private_files',
    'success' => t('Private files directory is outside the web server root.'),
    'failure' => t('Private files is enabled but the specified directory is not secure outside the web server root.'),
  );
  $checks['upload_extensions'] = array(
    'title' => t('Allowed upload extensions'),
    'callback' => 'acquia_spi_security_review_check_upload_extensions',
    'success' => t('Only safe extensions are allowed for uploaded files.'),
    'failure' => t('Unsafe file extensions are allowed in uploads.'),
  );
  // Checks dependent on dblog.
  if (module_exists('dblog')) {
    $checks['query_errors'] = array(
      'title' => t('Database errors'),
      'callback' => 'acquia_spi_security_review_check_query_errors',
      'success' => t('Few query errors from the same IP.'),
      'failure' => t('Query errors from the same IP. These may be a SQL injection attack or an attempt at information disclosure.'),
    );

    $checks['failed_logins'] = array(
      'title' => t('Failed logins'),
      'callback' => 'acquia_spi_security_review_check_failed_logins',
      'success' => t('Few failed login attempts from the same IP.'),
      'failure' => t('Failed login attempts from the same IP. These may be a brute-force attack to gain access to your site.'),
    );
  }
  $checks['admin_permissions'] = array(
    'title' => t('Drupal admin permissions'),
    'callback' => 'acquia_spi_security_review_check_admin_permissions',
    'success' => t('Untrusted roles do not have administrative permissions.'),
    'failure' => t('Untrusted roles have been granted administrative permissions.'),
  );
  $checks['password_in_emails'] = array(
    'title' => t('Password included in user emails'),
    'callback' => 'acquia_spi_security_review_check_email_passwords',
    'success' => t('User passwords are not included in emails.'),
    'failure' => t('User passwords are included in emails.'),
  );
  // Check dependent on PHP filter being enabled.
  if (module_exists('php')) {
    $checks['untrusted_php'] = array(
      'title' => t('PHP access'),
      'callback' => 'acquia_spi_security_review_check_php_filter',
      'success' => t('Untrusted users do not have access to use the PHP input format.'),
      'failure' => t('Untrusted users have access to use the PHP input format.'),
    );
  }
  $checks['executable_php'] = array(
    'title' => t('Executable PHP'),
    'callback' => 'acquia_spi_security_review_check_executable_php',
    'success' => t('PHP files in the Drupal files directory cannot be executed.'),
    'failure' => t('PHP files in the Drupal files directory can be executed.'),
  );
  $checks['base_url_set'] = array(
    'title' => t('Drupal base URL'),
    'callback' => 'acquia_spi_security_review_check_base_url',
    'success' => t('Base URL is set in settings.php.'),
    'failure' => t('Base URL is not set in settings.php.'),
  );
  $checks['temporary_files'] = array(
    'title' => t('Temporary files'),
    'callback' => 'acquia_spi_security_review_check_temporary_files',
    'success' => t('No sensitive temporary files were found.'),
    'failure' => t('Sensitive temporary files were found on your files system.'),
  );
  if (module_exists('views') && function_exists('views_get_all_views')) {
    $checks['views_access'] = array(
      'title' => t('Views access'),
      'callback' => 'acquia_spi_security_review_check_views_access',
      'success' => t('Views are access controlled.'),
      'failure' => t('There are Views that do not provide any access checks.'),
    );
  }

  return array('security_review' => $checks);
}

/**
 * Checks for acquia_spi_security_review_get_checks() when Filefield is enabled.
 */
function acquia_spi_filefield_security_checks() {
  $checks['filefield_extensions'] = array(
    'title' => t('Filefield allowed uploads'),
    'callback' => 'acquia_spi_security_review_check_filefield_extensions',
    'success' => t('Only safe extensions are allowed for Filefield uploaded files.'),
    'failure' => t('Unsafe file extensions are allowed in Filefield uploads.'),
  );
  return array('filefield' => $checks);
}

/**
 * Checks for acquia_spi_security_review_get_checks() when Views is enabled.
 */
function acquia_spi_views_security_checks() {
  $checks['access'] = array(
    'title' => t('Views access'),
    'callback' => 'acquia_spi_security_review_check_views_access',
    'success' => t('Views are access controlled.'),
    'failure' => t('There are Views that do not provide any access checks.'),
  );
  return array('views' => $checks);
}

/**
 * Check that files aren't writeable by the server.
 */
function acquia_spi_security_review_check_file_perms() {
  $result = TRUE;
  // Extract ending folder for file directory path.
  $file_path = './'. rtrim(file_directory_path(), '/');
  // Set files to ignore.
  $ignore = $ignore = array('..', 'CVS', '.git', '.svn', '.bzr', $file_path);
  $files = _acquia_spi_security_review_check_file_perms_scan('.', $ignore);

  // Try creating or appending files.
  // Assume it doesn't work.
  $create_status = $append_status = FALSE;

  $directory = drupal_get_path('module', 'security_review');
  // Write a file with the timestamp
  $file = './'. $directory .'/file_write_test.'. date('Ymdhis');
  if ($file_create = @fopen($file, 'w')) {
    $create_status = fwrite($file_create, t("This is a vulnerable directory.\n"));
    fclose($file_create);
  }
  // Try to append to our IGNOREME file.
  $file = './'. $directory .'/IGNOREME.txt';
  if ($file_append = fopen($file, 'a')) {
    $append_status = fwrite($file_append, date('Ymdhis') . "\n");
    fclose($file_append);
  }

  if (count($files) || $create_status || $append_status) {
    $result = FALSE;
  }
  return array('result' => $result, 'value' => $files);
}

/**
 * @param $directory
 * @param $ignore
 * @return array
 */
function _acquia_spi_security_review_check_file_perms_scan($directory, $ignore) {
  $items = array();
  if (is_readable($directory) && $handle = opendir($directory)) {
    while (($file = readdir($handle)) !== FALSE) {
      // Don't check hidden files or ones we said to ignore, or subdir site links.
      if ($file[0] != "." && !in_array($file, $ignore) && !(is_link($file) && readlink($file) == '.')) {
        $file = $directory . "/" . $file;
        if (is_dir($file) && !in_array($file, $ignore)) {
          $items = array_merge($items, _acquia_spi_security_review_check_file_perms_scan($file, $ignore));
          if (is_writable($file)) {
            $items[] = preg_replace("/\/\//si", "/", $file);
          }
        }
        elseif (is_writable($file)) {
          $items[] = preg_replace("/\/\//si", "/", $file);
        }
      }
    }
    closedir($handle);
  }
  return $items;
}

/**
 * Check for formats that either do not have HTML filter that can be used by
 * untrusted users, or if they do check if unsafe tags are allowed.
 */
function acquia_spi_security_review_check_input_formats() {
  $result = TRUE;
  $formats = filter_formats();
  $check_result_value = array();
  // Check formats that are accessible by untrusted users.
  $untrusted_roles = acquia_spi_security_review_untrusted_roles();
  // The default format is usable by all users even if no roles are listed on it.
  $default_format = variable_get('filter_default_format', FILTER_FORMAT_DEFAULT);
  foreach ($formats as $id => $format) {
    $format_roles = array_filter(explode(',', $format->roles));
    if ($format->format == $default_format) {
      // The default format is available to all roles.
      $intersect = drupal_map_assoc(array_keys(user_roles()));
    }
    else {
      $intersect = array_intersect($format_roles, $untrusted_roles);
    }
    if (!empty($intersect)) {
      // Untrusted users can use this format.
      $filters = filter_list_format($format->format);
      // Check format for HTML filter.
      if (in_array('filter/0', array_keys($filters))) {
        // Using HTML Filter, good! Now check allowed tags if the filter is stripping instead of escaping.
        $setting = variable_get("filter_html_" . $format->format, FILTER_HTML_STRIP);
        if ($setting == FILTER_HTML_STRIP) {
          // Check for unsafe tags in allowed tags.
          $allowed_tags = variable_get("allowed_html_" . $format->format, '');
          $unsafe_tags = acquia_spi_security_review_unsafe_tags();
          foreach ($unsafe_tags as $tag) {
            if (strpos($allowed_tags, '<' . $tag . '>') !== FALSE) {
              // Found an unsafe tag
              $check_result_value['tags'][$id] = $tag;
            }
          }
        }
      }
      else {
        // Format is usable by untrusted users but does not contain the HTML Filter.
        $check_result_value['formats'][$id] = $format->name;
      }
    }
  }

  if (!empty($check_result_value)) {
    $result = FALSE;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

function acquia_spi_security_review_check_php_filter() {
  $result = TRUE;
  $formats = filter_formats();
  $check_result_value = array();
  // Check formats that are accessible by untrusted users.
  $untrusted_roles = acquia_spi_security_review_untrusted_roles();
  // The default format is usable by all users even if no roles are listed on it.
  $default_format = variable_get('filter_default_format', FILTER_FORMAT_DEFAULT);
  // Loop through each format and look for the PHP filter.
  foreach ($formats as $id => $format) {
    $format_roles = array_filter(explode(',', $format->roles));
    if ($format->format == $default_format) {
      // The default format is available to all roles.
      $intersect = drupal_map_assoc(array_keys(user_roles()));
    }
    else {
      $intersect = array_intersect($format_roles, $untrusted_roles);
    }
    if (!empty($intersect)) {
      // Untrusted users can use this format.
      $filters = filter_list_format($format->format);
      // Check format for PHP filter.
      if (in_array('php/0', array_keys($filters))) {
        $result = FALSE;
        $check_result_value['formats'][$id] = $format->name;
        $check_result_value['roles'] = $intersect;
      }
    }
  }

  return array('result' => $result, 'value' => $check_result_value);
}

function acquia_spi_security_review_check_error_reporting() {
  $error_level = variable_get('error_level', NULL);
  if (is_null($error_level) || intval($error_level) == 1) {
    // When the variable isn't set, or its set to 1 errors are printed to the screen.
    $result = FALSE;
  }
  else {
    $result = TRUE;
  }
  return array('result' => $result);
}

/**
 * If private files is enabled check that the directory is not under the web root.
 *
 * There is ample room for the user to get around this check. @TODO get more sophisticated?
 */
function acquia_spi_security_review_check_private_files() {
  $file_downloads = variable_get('file_downloads', FILE_DOWNLOADS_PUBLIC);
  if ($file_downloads == FILE_DOWNLOADS_PRIVATE) {
    $file_directory_path = file_directory_path();
    if (strpos($file_directory_path, '/') === 0) {
      // Path begins at root.
      $result = TRUE;
    }
    elseif (strpos($file_directory_path, '../') === 0) {
      // Path begins by moving up the system.
      $result = FALSE;
    }
    else {
      // Directory is relative (or crafty).
      $result = FALSE;
    }
  }
  else {
    $result = NULL;
  }
  return array('result' => $result, 'value' => $file_downloads);
}

/**
 * Check for unsafe allowed extensions.
 */
function acquia_spi_security_review_check_upload_extensions() {
  $result = TRUE;
  $check_result_value = array();
  if (module_exists('upload')) {
    // First, check if any unsafe extensions are even allowed.
    $extensions = variable_get('upload_extensions_default', NULL);
    if (!is_null($extensions)) {
      $unsafe_extensions = acquia_spi_security_review_unsafe_extensions();
      foreach ($unsafe_extensions as $unsafe_extension) {
        if (strpos($extensions, $unsafe_extension) !== FALSE) {
          // Found an unsafe extension.
          $check_result_value['upload'][] = $unsafe_extension;
        }
      }
    }
    if (!empty($check_result_value)) {
      $result = FALSE;
    }
  }
  else {
    $result = NULL;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

function acquia_spi_security_review_check_query_errors($last_check = NULL) {
  $timestamp = NULL;
  $check_result_value = array();
  $sql = "SELECT message, hostname FROM {watchdog} WHERE type = 'php' AND severity = %d";
  if (!is_null($last_check)) {
    $sql .= " AND timestamp >= %d";
    $timestamp = $last_check['lastrun'];
  }
  $results = db_query($sql, WATCHDOG_ERROR, $timestamp);
  while ($row = db_fetch_array($results)) {
    if (strpos($row['message'], 'SELECT') !== FALSE) {
      $entries[$row['hostname']][] = $row;
    }
  }
  $result = TRUE;
  if (!empty($entries)) {
    foreach ($entries as $ip => $records) {
      if (count($records) > 10) {
        $check_result_value[] = $ip;
      }
    }
  }
  if (!empty($check_result_value)) {
    $result = FALSE;
  }
  else {
    // Rather than worrying the user about the idea of query errors we skip reporting a pass.
    $result = NULL;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

function acquia_spi_security_review_check_failed_logins($last_check = NULL) {
  $result = TRUE;
  $timestamp = NULL;
  $check_result_value = array();
  $sql = "SELECT message, hostname FROM {watchdog} WHERE type = 'user' AND severity = %d";
  if (!is_null($last_check)) {
    $sql .= " AND timestamp >= %d";
    $timestamp = $last_check['lastrun'];
  }
  $results = db_query($sql, WATCHDOG_NOTICE, $timestamp);
  while ($row = db_fetch_array($results)) {
    if (strpos($row['message'], 'Login attempt failed') !== FALSE) {
      $entries[$row['hostname']][] = $row;
    }
  }
  if (!empty($entries)) {
    foreach ($entries as $ip => $records) {
      if (count($records) > 10) {
        $check_result_value[] = $ip;
      }
    }
  }
  if (!empty($check_result_value)) {
    $result = FALSE;
  }
  else {
    // Rather than worrying the user about the idea of failed logins we skip reporting a pass.
    $result = NULL;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

/**
 * Look for admin permissions granted to untrusted roles.
 */
function acquia_spi_security_review_check_admin_permissions() {
  $result = TRUE;
  $check_result_value = array();
  // Collect the permissions untrusted roles have.
  $untrusted = acquia_spi_security_review_untrusteds_permissions();
  // Admin permissions.
  $admin_perms = acquia_spi_security_review_admin_permissions();
  // Check for intersections.
  foreach ($untrusted as $rid => $permissions) {
    $intersect = array_intersect($permissions, $admin_perms);
    if (!empty($intersect)) {
      $check_result_value[$rid] = $intersect;
    }
  }
  if (!empty($check_result_value)) {
    $result = FALSE;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

function acquia_spi_security_review_check_nodes($last_check = NULL) {
  $result = TRUE;
  $check_result_value = array();
  $timestamp = NULL;

  $sql = "SELECT n.nid FROM {node} n INNER JOIN {node_revisions} r ON n.vid = r.vid WHERE r.body LIKE '%s' ";
  // If the check passed before only look at nodes since the last run.
  if (!is_null($last_check) && $last_check['result'] == '1') {
    $sql .= " AND n.changed >= %d";
    $timestamp = $last_check['lastrun'];
  }
  foreach (array('Javascript' => '%<script%', 'PHP' => '%<?php%') as $description => $comparison) {
    $results = pager_query($sql, 50, 0, NULL, $comparison, $timestamp);
    while ($row = db_fetch_array($results)) {
      $check_result_value[] = array($description => $row['nid']);
    }
    if (!empty($check_result_value)) {
      $result = FALSE;
    }
  }

  return array('result' => $result, 'value' => $check_result_value);
}

function acquia_spi_security_review_check_comments($last_check = NULL) {
  $result = TRUE;
  $check_result_value = array();
  $timestamp = NULL;
  if (module_exists('comment')) {
    $sql = "SELECT nid, cid FROM {comments} WHERE comment LIKE '%s'" ;
    // If the check passed before only look at comments since the last run.
    if (!is_null($last_check) && $last_check['result'] == '1') {
      $sql .= " AND timestamp >= %d";
      $timestamp = $last_check['lastrun'];
    }
    foreach (array('Javascript' => '%<script%', 'PHP' => '%<?php%') as $description => $comparison) {
      $results = pager_query($sql, 20, 0, NULL, $comparison, $timestamp);
      while ($row = db_fetch_array($results)) {
        $check_result_value[$row['cid']] = array($description => $row['nid']);
      }
      if (!empty($check_result_value)) {
        $result = FALSE;
      }
    }
  }
  else {
    $result = NULL;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

/**
 * Check if PHP files written to the files directory can be executed.
 */
function acquia_spi_security_review_check_executable_php($last_check = NULL) {
  global $base_url;
  $result = TRUE;
  $check_result_value = array();

  $message = 'Security review test '. date('Ymdhis');
  $content = "<?php\necho '". $message ."';";
  $directory = file_directory_path();
  $file = '/security_review_test.php';
  if ($file_create = @fopen('./'. $directory . $file, 'w')) {
    $create_status = fwrite($file_create, $content);
    fclose($file_create);
    $response = drupal_http_request($base_url .'/'. $directory . $file);
    if ($response->code == 200 && $response->data === $message) {
      $result = FALSE;
      $check_result_value[] = 'executable_php';
    }
    if (file_exists('./'. $directory . $file)) {
        @unlink('./'. $directory . $file);
    }
    // Check for presence of the .htaccess file and if the contents are correct.
    if (!file_exists($directory .'/.htaccess')) {
      $result = FALSE;
      $check_result_value[] = 'missing_htaccess';
    }
    else {
      $contents = file_get_contents($directory .'/.htaccess');
      // Text from includes/file.inc.
      $expected = "SetHandler Drupal_Security_Do_Not_Remove_See_SA_2006_006\nOptions None\nOptions +FollowSymLinks";
      if ($contents !== $expected) {
        $result = FALSE;
        $check_result_value[] = 'incorrect_htaccess';
      }
      if (is_writable($directory .'/.htaccess')) {
        // Don't modify $result.
        $check_result_value[] = 'writable_htaccess';
      }
    }
    return array('result' => $result, 'value' => $check_result_value);
  }
}

/**
 * Check if $base_url is set in settings.php.
 */
function acquia_spi_security_review_check_base_url($last_check = NULL) {
  $drupal_root = getcwd();
  // Support different methods to check for $base_url.
  $method = variable_get('security_review_base_url_method', 'token');
  $result = NULL;
  if ($method === 'token') {
    $result = FALSE;
    if (file_exists($drupal_root .'/'. conf_path() .'/settings.php')) {
      $content = file_get_contents($drupal_root .'/'. conf_path() .'/settings.php');
      $tokens = token_get_all($content);
      foreach ($tokens as $token) {
        if (is_array($token) && $token[0] === T_VARIABLE && $token[1] == '$base_url') {
          $result = TRUE;
          break;
        }
      }
    }
  }
  elseif ($method === 'include') {
    if (file_exists($drupal_root . '/' . conf_path() . '/settings.php')) {
      include $drupal_root .'/'. conf_path() . '/settings.php';
    }
    if (isset($base_url)) {
      $result = TRUE;
    }
    else {
      $result = FALSE;
    }
  }
  return array('result' => $result, 'value' => '');
}

/**
 * Check for sensitive temporary files like settings.php~.
 */
function acquia_spi_security_review_check_temporary_files($last_check = NULL) {
  $drupal_root = getcwd();
  $result = TRUE;
  $check_result_value = array();
  $files = array();
  $dir = scandir($drupal_root . '/' . conf_path() . '/');
  foreach ($dir as $file) {
    // Set full path to only files.
    if (!is_dir($file)) {
      $files[] = $drupal_root . '/' . conf_path() . '/' . $file;
    }
  }
  drupal_alter('security_review_temporary_files', $files);
  foreach ($files as $path) {
    $matches = array();
    if (file_exists($path) && preg_match('/.*(~|\.sw[op]|\.bak|\.orig|\.save)$/', $path, $matches) !== FALSE && !empty($matches)) {
      $result = FALSE;
      $check_result_value[] = $path;
    }
  }
  return array('result' => $result, 'value' => $check_result_value);
}

/**
 * A Security Review check for allowed extensions on Filefield fields.
 */
function acquia_spi_security_review_check_filefield_extensions($last_check = NULL) {
  $result = TRUE;
  $check_result_value = array();
  $unsafe_extensions = acquia_spi_security_review_unsafe_extensions();

  // Collect list of untrusted roles' permissions.
  $untrusted = acquia_spi_security_review_untrusteds_permissions();
  // Get Filefields and check widget file_extensions.
  $fields = filefield_get_field_list();
  foreach ($fields as $field) {
    /* @TODO Add this back in when also checking upload use on content types.
    // Initialize an empty array of roles IDs that can use this field.
    $rids = array();
    // Check if untrusted users can create or edit the content type that holds
    // this field.
    $field_type_permissions = array(
      'create ' . $field['type_name'] . ' content',
      'edit own ' . $field['type_name'] . ' content',
      'edit any ' . $field['type_name'] . ' content',
    );
    foreach ($untrusted as $rid => $permissions) {
      $intersect = array_intersect($permissions, $field_type_permissions);
      if (empty($intersect)) {
        // No intersection is good.
        continue;
      }
      else {
        $rids[] = $rid;
      }
    }
    // Do not continue check if no untrusted roles can use this field.
    if (empty($rids)) {
      continue;
    }*/

    $extensions = $field['widget']['file_extensions'];
    if (empty($extensions)) {
      // No extensions set, so field allows all unsafe extensions.
      $check_result_value[$field['field_name']]['empty'] = TRUE;
    }
    else {
      // Check if specific unsafe extensions are allowed.
      foreach ($unsafe_extensions as $unsafe_extension) {
        if (strpos($extensions, $unsafe_extension) !== FALSE) {
          // Found an unsafe extension.
          $check_result_value[$field['field_name']]['extensions'][] = $unsafe_extension;
        }
      }
    }
  }
  if (!empty($check_result_value)) {
    $result = FALSE;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

function acquia_spi_security_review_check_views_access($last_check = NULL) {
  $result = TRUE;
  $check_result_value = array();
  $timestamp = NULL;
  // Load and loop through every view, checking the access type in displays.
  $views = views_get_all_views();
  foreach ($views as $view) {
    if ($view->disabled !== TRUE) {
      // Access is set in display options of a display.
      foreach ($view->display as $display_name => $display) {
        if (isset($display->display_options['access']) && $display->display_options['access']['type'] == 'none') {
          $check_result_value[$view->name][] = $display_name;
        }
      }
    }
  }
  if (!empty($check_result_value)) {
    $result = FALSE;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

function acquia_spi_security_review_check_email_passwords($last_check = NULL) {
  $result = TRUE;

  drupal_load('module', 'user');
  $mail_templates = array('register_no_approval_required_subject', 'register_no_approval_required_body', 'register_admin_created_subject', 'register_admin_created_body', 'register_pending_approval_subject', 'register_pending_approval_admin_subject', 'register_pending_approval_body', 'register_pending_approval_admin_body', 'password_reset_subject', 'password_reset_body', 'status_activated_subject', 'status_activated_body', 'status_blocked_subject', 'status_blocked_body', 'status_deleted_subject', 'status_deleted_body');

  $check_result_value = array();
  foreach ($mail_templates as $template_name) {
    $text = _user_mail_text($template_name);
    if (strpos($text, '!password') !== FALSE) {
      $check_result_value[] = $template_name;
    }
  }

  if (!empty($check_result_value)) {
    $result = FALSE;
  }
  return array('result' => $result, 'value' => $check_result_value);
}

/**
 * Helper function defines HTML tags that are considered unsafe.
 *
 * Based on wysiwyg_filter_get_elements_blacklist().
 */
function acquia_spi_security_review_unsafe_tags() {
  return array(
    'applet',
    'area',
    'audio',
    'base',
    'basefont',
    'body',
    'button',
    'comment',
    'embed',
    'eval',
    'form',
    'frame',
    'frameset',
    'head',
    'html',
    'iframe',
    'image',
    'img',
    'input',
    'isindex',
    'label',
    'link',
    'map',
    'math',
    'meta',
    'noframes',
    'noscript',
    'object',
    'optgroup',
    'option',
    'param',
    'script',
    'select',
    'style',
    'svg',
    'table',
    'td',
    'textarea',
    'title',
    'video',
    'vmlframe',
  );
}

/**
 * Helper function defines file extensions considered unsafe.
 */
function acquia_spi_security_review_unsafe_extensions() {
  return array(
    'swf',
    'exe',
    'html',
    'htm',
    'php',
    'phtml',
    'py',
    'js',
    'vb',
    'vbe',
    'vbs',
  );
}

/**
 * Helper function defines the default untrusted Drupal roles.
 */
function acquia_spi_security_review_default_untrusted_roles() {
  $rids = array(DRUPAL_ANONYMOUS_RID);
  $user_register = variable_get('user_register', 1);
  if ($user_register == 1) {
    $rids[] = DRUPAL_AUTHENTICATED_RID;
  }
  return $rids;
}

/**
 * Helper function for user-defined or default unstrusted Drupal roles.
 */
function acquia_spi_security_review_untrusted_roles() {
  $defaults = acquia_spi_security_review_default_untrusted_roles();
  $roles = variable_get('security_review_untrusted_roles', $defaults);
  return array_filter($roles);
}

/**
 * Helper function collects the permissions untrusted roles have.
 */
function acquia_spi_security_review_untrusteds_permissions() {
  static $permissions;
  if (empty($permissions)) {
    $permissions = array();
    // Collect list of untrusted roles' permissions.
    $untrusted_roles = acquia_spi_security_review_untrusted_roles();
    foreach ($untrusted_roles as $rid) {
      $perms = array();
      $results = db_fetch_array(db_query('SELECT r.rid, p.perm FROM {role} r LEFT JOIN {permission} p ON r.rid = p.rid WHERE r.rid = %d', $rid));
      if ($results !== FALSE) {
        $perms = explode(',', str_replace(', ', ',', $results['perm']));
        $permissions[$rid] = $perms;
      }
    }
  }
  return $permissions;
}

/**
 * Helper function for assumed trusted roles.
 */
function acquia_spi_security_review_trusted_roles() {
  $trusted_roles = array();
  $untrusted_roles = acquia_spi_security_review_untrusted_roles();
  $result = db_query('SELECT rid, name FROM {role} WHERE rid NOT IN (' . db_placeholders($untrusted_roles) . ')', $untrusted_roles);
  while ($role = db_fetch_object($result)) {
    $trusted_roles[$role->rid] = $role->name;
  }
  return array_filter($trusted_roles);
}

/**
 * Helper function defines super-admin Drupal permissions.
 */
function acquia_spi_security_review_admin_permissions() {
  return array(
    'administer users',
    'administer permissions',
    'administer site configuration',
    'administer filters',
    'administer content types',
    'administer nodes',
  );
}

/**
 * Helper function checks if role has been granted a permission.
 */
function acquia_spi_security_review_role_permission($rid, $permission) {
  $return = FALSE;
  $result = db_fetch_array(db_query("SELECT perm FROM {permission} WHERE rid = %d", $rid));
  if ($result['perm'] && strpos($result['perm'], $permission) !== FALSE) {
    $return = TRUE;
  }
  return $return;
}

/**
 * Helper function allows for collection of this file's security checks.
 */
function acquia_spi_security_review_get_checks() {
  // Use Security Review's checks if available.
  if (module_exists('security_review') && function_exists('security_review_security_checks')) {
    return module_invoke_all('security_checks');
  }
  $checks = _acquia_spi_security_review_security_checks();
  if (module_exists('filefield') && function_exists('filefield_get_field_list')) {
    $filefield = acquia_spi_filefield_security_checks();
    $checks = array_merge($checks, $filefield);
  }
  if (module_exists('views') && function_exists('views_get_all_views')) {
    $views = acquia_spi_views_security_checks();
    $checks = array_merge($checks, $views);
  }
  return $checks;
}

/**
 * Internal log helper function.
 *
 * Separated from directly using watchdog() in other functions to allow for extended reporting.
 */
function _acquia_spi_security_review_log($module, $check_name, $message, $variables, $type) {
  module_invoke_all('security_review_log', $module, $check_name, $message, $variables, $type);
}

<?php

/**
 * @file Upload to Acquia Cloud.
 */

/**
 * Menu callback for checking client upload.
 *
 */
function acquia_agent_migrate_check() {
  $return = array('compatible' => TRUE);
  $env = _acquia_migrate_check_env();
  if (empty($env) || $env['error'] !== FALSE) {
    $return['compatible'] = FALSE;
    $return['message'] = $env['error'];
  }
  drupal_json($return);
}

/**
 * Check server for migration capabilities.
 *
 * @return Array of environment capabilities or 'error' is set.
 */
function _acquia_migrate_check_env() {
  $env = array('error' => FALSE);

  if (!class_exists('Archive_Tar')) {
    module_load_include('inc', 'acquia_agent', 'archive_tar');
  }
  if (!function_exists('json_decode')) {
    $env['error'] = t('Requires PHP 5.2');
    return $env;
  }
  // Check available compression libs.
  if (function_exists('gzopen')) {
    $env['compression_ext'] = 'gz';
  }
  elseif (function_exists('bzopen')) {
    $env['compression_ext'] = 'bz2';
  }
  elseif (class_exists('ZipArchive')) {
    $env['compression_ext'] = 'zip';
  }
  else {
    $env['error'] = t('No compression libraries available');
  }
  return $env;
}

/**
 * Setup archive directory and internal migrate data struct.
 *
 * @param array $environment
 *   Environment to migrate to, from NSPI acquia_agent_cloud_migration_environments()
 */
function acquia_migrate_prepare($environment) {
  cache_clear_all();

  // Internal migration store is an array because objects cannot be stored
  // by Drupal's Batch API.
  $local_env = _acquia_migrate_check_env();
  if ($local_env['error'] !== FALSE) {
    return $local_env;
  }
  // Modify environment URL if SSL is available for use.
  if (in_array('ssl', stream_get_transports(), TRUE) && !defined('ACQUIA_DEVELOPMENT_NOSSL')) {
    $uri = parse_url($environment['url']);
    if (isset($uri['host'])) {
      $environment['url'] = $uri['host'];
    }
    $environment['url'] .= isset($uri['port']) ? ':' . $uri['port'] : '';
    $environment['url'] .= (isset($uri['path']) && isset($uri['host'])) ? $uri['path'] : '';
    $environment['url'] = 'https://' . $environment['url'];
  }
  $time = time();
  $date = gmdate('Ymd_his');
  $migration = array(
    'error' => FALSE,
    'id' => uniqid() . '_' . $date,
    'date' => $date,
    'time' => $time,
    'compression_ext' => $local_env['compression_ext'],
    // Parameters used in transmission request.
    'request_params' => array(
      'r' => url('admin/settings/acquia-agent', array('absolute' => TRUE)), // Return URL on this site.
      'y' => 'sar', // For Acquia Hosting
      'stage' => $environment['stage'],
      'nonce' => $environment['nonce'],
    ),
    'env' => $environment,
    'no_data_tables' => array(),
  );

  // Set up local storage of archive.
  _acquia_migrate_destination($migration);

  return $migration;
}

/**
 * Ensure this response can work through migration.
 */
function _acquia_migrate_process_setup() {
  if (!defined('OS_WINDOWS') && defined('PHP_OS') && in_array(PHP_OS, array('WINNT', 'WIN32', 'Windows'))) {
    // OS_WINDOWS constant used by Archive_Tar.
    define('OS_WINDOWS', TRUE);
  }
  // Load any required include files.
  _acquia_migrate_check_env();
  // If not in 'safe mode', increase the maximum execution time:
  if (!ini_get('safe_mode') && strpos(ini_get('disable_functions'), 'set_time_limit') === FALSE && ini_get('max_execution_time') < 1200) {
    set_time_limit(variable_get('acquia_migrate_max_time', 1200));
  }
}

/**
 * Create temporary directory and setup file for migration.
 */
function _acquia_migrate_destination(&$migration) {
  $tmp_dir = realpath(file_directory_path()) . DIRECTORY_SEPARATOR . 'acquia_migrate' . $migration['id'];
  if (!mkdir($tmp_dir) || !is_writable($tmp_dir)) {
    $migration['error'] = t('Cannot create temporary directory !dir to store site archive.', array('!dir' => $tmp_dir));
    return;
  }
  $migration['dir'] = $tmp_dir;
  $migration['file'] = $tmp_dir . DIRECTORY_SEPARATOR . 'archive-' . $migration['date'];
}

/**
 * Test migration setup and destination.
 *
 * @param Array of migration information.
 *
 * @return boolean Whether migration can continue.
 */
function _acquia_migrate_test_migration_setup(&$migration) {
  $url = $migration['env']['url'];
  $headers = array(
    'User-Agent' => 'Acquia Migrate Client/1.x (Drupal ' . VERSION . ';)',
  );
  $response = drupal_http_request($url, $headers, 'GET', NULL, 1, 10.0);
  if ($response->code != 200) {
    $migration['error'] = t('Unable to connect to migration destination site, please contact Acquia Support.');
    return FALSE;
  }
  // A 200 response with body 'invalid request' is returned from the AH_UPLOAD
  // script if receiving a GET request.
  if (strpos($url, 'AH_UPLOAD') !== FALSE && trim($response->data) != 'invalid request') {
    $migration['error'] = t('Unable to connect to migration destination site, please contact Acquia Support.');
    return FALSE;
  }
}

/**
 * Complete migration tasks.
 */
function _acquia_migrate_complete(&$migration) {
  //$time = timer_stop('acquia_migrate');
  $identifier = acquia_agent_settings('acquia_identifier');
  $key = acquia_agent_settings('acquia_key');
  $body = array('identifier' => acquia_agent_settings('acquia_identifier'));
  if (isset($migration['redirect']) && is_array($migration['redirect']['data'])) {
    $body += $migration['redirect']['data'];
  }
  $data = acquia_agent_call('acquia.agent.cloud.migration.complete', $body, $identifier, $key, variable_get('acquia_spi_server', 'https://nspi.acquia.com'));

  if ($errno = xmlrpc_errno()) {
    acquia_agent_report_xmlrpc_error();
    $migration['error'] = TRUE;
    return;
  }
  elseif (!$data || !isset($data['result'])) {
    $migration['error'] = t("Server error, please submit again.");
    return;
  }

  // Response is in $data['result'].
  $result = $data['result'];
  if ($result['success']) {
    $migration['network_url'] = $result['network_url'];
  }
  else {
    $migration['error'] = $result['error'];
  }
  return $migration;
}

function acquia_migrate_batch_test($migration, &$context) {
  _acquia_migrate_process_setup();
  // Latest migration might be in $context.
  if (!empty($context['results']['migration'])) {
    $migration = $context['results']['migration'];
    variable_set('acquia_agent_cloud_migration', $migration);
  }
  // Check for error and abort if appropriate.
  if (empty($migration) || $migration['error'] !== FALSE) {
    $context['message'] = t('Encountered error, aborting migration.');
    return;
  }

  _acquia_migrate_test_migration_setup($migration);

  // Store migration in results so it can be used by next operation.
  $context['results']['migration'] = $migration;
  $context['message'] = t('Testing migration capabilities');
}

function acquia_migrate_batch_db($migration, &$context) {
  _acquia_migrate_process_setup();
  // Latest migration might be in $context.
  if (!empty($context['results']['migration'])) {
    $migration = $context['results']['migration'];
    variable_set('acquia_agent_cloud_migration', $migration);
  }
  // Check for error and abort if appropriate.
  if (empty($migration) || $migration['error'] !== FALSE) {
    $context['message'] = t('Encountered error, aborting migration.');
    return;
  }

  _acquia_migrate_backup_db_to_file_mysql($migration);

  // Store migration in results so it can be used by next operation.
  $context['results']['migration'] = $migration;
  $context['message'] = t('Exported database. Archiving files.');
}

function acquia_migrate_batch_tar($migration, &$context) {
  _acquia_migrate_process_setup();
  // Latest migration is in $context.
  if (!empty($context['results']['migration'])) {
    $migration = $context['results']['migration'];
    variable_set('acquia_agent_cloud_migration', $migration);
  }
  // Check for error and abort if appropriate.
  if (empty($migration) || $migration['error'] !== FALSE) {
    $context['message'] = t('Encountered error, aborting migration.');
    return;
  }

  _acquia_migrate_archive_site($migration);

  // Store migration in results so it can be used by next operation.
  $context['results']['migration'] = $migration;
  $context['message'] = t('Created archive. Beginning transfer.');
}

function acquia_migrate_batch_transmit($migration, &$context) {
  _acquia_migrate_process_setup();
  // Latest migration is in $context.
  if (!empty($context['results']['migration'])) {
    $migration = $context['results']['migration'];
    variable_set('acquia_agent_cloud_migration', $migration);
  }
  // Check for error and abort if appropriate.
  if (empty($migration) || $migration['error'] !== FALSE) {
    $context['message'] = t('Encountered error, aborting migration.');
    $context['finished'] = 1;
    return;
  }
  // First call.
  if (empty($context['sandbox'])) {
    $context['sandbox']['position'] = 0;
    $size = filesize($migration['tar_file']);
    $context['sandbox']['size'] = $size;
    $migration['request_params']['file_size'] = $size;
    $migration['request_params']['hash'] = md5_file($migration['tar_file']);
    $migration['file_name'] = basename($migration['tar_file']);
  }

  // Set to 0.5 MB.
  $length = 1024 * 1024 / 2;
  $position = _acquia_migrate_transmit_chunk($migration, $context['sandbox']['position'], $length);
  $context['sandbox']['position'] = $position;

  // Store migration in results so it can be used by next operation.
  $context['results']['migration'] = $migration;
  if ($context['sandbox']['position'] !== FALSE) {
    $context['message'] = t('Uploading archive. Transferred !pos of !size bytes.', array('!pos' => $context['sandbox']['position'], '!size' => $context['sandbox']['size']));
    $context['finished'] = $context['sandbox']['position'] / $context['sandbox']['size'];
  }
  else {
    $context['finished'] = 1;
  }
}

function acquia_migrate_batch_finished($success, $results, $operations) {
  $migration = !empty($results['migration']) ? $results['migration'] : FALSE;
  if ($success && $migration && $migration['error'] == FALSE) {
    // Inform Acquia Cloud of migration completion.
    _acquia_migrate_complete($migration);
    if ($migration['error'] != FALSE) {
      $message = t('There was an error checking for completed migration. !err<br/>See the !network for more information.', array('!err' => $migration['error'], '!network' => l('Network dashboard', 'https://network.acquia.com/')));
      drupal_set_message($message);
    }
    else {
      $message = t('Migrate success. You can see import progress on the !network.', array(
        '!network' => l(t('Acquia Network'), $migration['network_url'], array('external' => TRUE)),
      ));
      drupal_set_message($message);
    }
    // Cleanup migration.
    _acquia_migrate_cleanup($migration);
  }
  else {
    watchdog('acquia-migrate', 'Migration error @m', array('@m' => var_export($migration, TRUE)), WATCHDOG_ERROR);
    $message = t('There was an error during migration.');
    if ($migration && is_string($migration['error'])) {
      $message .= ' ' . $migration['error'];
    }
    drupal_set_message($message, 'error');
    // Cleanup anything left of migration.
    _acquia_migrate_cleanup($migration);
  }

  drupal_goto('admin/settings/acquia-agent');
}

function acquia_migrate_exclude($migration) {
  $exclude = array('.', '..', '.git', '.svn', 'CVS', '.bzr');
  // Exclude the migration directory.
  $exclude[] = basename($migration['dir']);
  if (!variable_get('acquia_migrate_files', 1)) {
    $exclude[] = realpath('.') . DIRECTORY_SEPARATOR . str_replace(realpath('.') . DIRECTORY_SEPARATOR, '', realpath(file_directory_path()));
  }
  return $exclude;
}

function _acquia_migrate_archive_site(&$migration) {
  $exclude = acquia_migrate_exclude($migration);
  $root = getcwd();
  $files = acquia_migrate_files_to_backup($root, $exclude);

  if (!empty($files) && isset($migration['file'])) {
    _acquia_migrate_validate_archive_files($migration, $files);
    if ($migration['error'] != FALSE) {
      return;
    }

    $dest_file = $migration['file'] . '.tar';
    if (!empty($migration['compression_ext'])) {
      $dest_file .= '.' . $migration['compression_ext'];
    }
    $gz = new Archive_Tar($dest_file, $migration['compression_ext'] ? $migration['compression_ext'] : NULL);
    if (!empty($migration['db_file'])) {
      // Add db file.
      $ret = $gz->addModify(array($migration['db_file']), '', $migration['dir'] . DIRECTORY_SEPARATOR);
    }
    if (defined('OS_WINDOWS')) {
      $remove_dir = $root . '\\';
    }
    else {
      $remove_dir = $root . '/';
    }
    $ret = $gz->addModify($files, '', $remove_dir);
    $migration['tar_file'] = $dest_file;
  }
  else {
    $migration['error'] = TRUE;
  }
}

/**
 * Run Acquia's site-uploader.php validation checks.
 */
function _acquia_migrate_validate_archive_files(&$migration, $files) {
  $output = implode("\n", $files);
  if (defined('OS_WINDOWS')) {
    $output = str_replace('\\', '/', $output);
  }
  $docroot = preg_quote(getcwd() . '/');

  // Count the number of sites dirs with settings.php files and files
  // directories.
  $count_settingsphp = preg_match_all('@^' . $docroot . 'sites/[^/\n]+/settings.php$@m', $output, $settings_phps);
  $count_filesdirs = preg_match_all('@^' . $docroot . 'sites/[^/\n]+/files/$@m', $output, $filesdirs);

  // Count the number of sql dumps in the root, plus in the docroot but
  // only if the docroot is a sub-dir (not empty or ./). Record all SQL
  // dumps into $sqls[0].
  $count_sqls = preg_match_all('@^' . $docroot . '[^/\n.][^/\n]*\.sql$@m', $output, $sqls);
  if (strlen($docroot) > 2) {
    $count_sqls += preg_match_all('@^(?:\./)?[^/\n.][^/\n]*\.sql$@m', $output, $docroot_sqls);
    $sqls[0] = array_merge($sqls[0], $docroot_sqls[0]);
  }
  // Allow simpletest SQL files.
  if (!empty($sqls[0])) {
    foreach ($sqls[0] as $key => $sql_file) {
      if (strpos($sql_file, 'simpletest') !== FALSE) {
        $count_sqls--;
        unset($sqls[0][$key]);
      }
    }
  }

  if (!in_array(getcwd() . DIRECTORY_SEPARATOR . 'index.php', $files)) {
    $$migration['error'] = "The archive file will not be in the correct format: no index.php found in root or top-level directory.";
  }
  elseif ($count_settingsphp > 1) {
    $migration['error'] = "The archive file will not be in the correct format: it must have at most one sites directory containing settings.php, but the install has $count_settingsphp: " . implode(', ', $settings_phps[0]) . ". Remove unnecessary settings.php files and try again.";
  }
  elseif ($count_settingsphp == 0 && $count_filesdirs > 1) {
    $migration['error'] = "The archive file will not be in the correct format: no settings.php file and the install has more than one sites directory containing a files directory. Remove unnecessary files directories or consolidate and try again.";
  }
  elseif ($count_sqls > 0) {
    $migration['error'] = "The archive file will not be in the correct format: it contains $count_sqls extra SQL files: " . implode(', ', $sqls[0]) . ". Remove extra .sql files and try again.";
  }
}

function _acquia_migrate_transmit_chunk(&$migration, $position, $length) {
  // Open file in binary mode.
  $handle = fopen($migration['tar_file'], 'rb');
  // Move to position in file.
  if ($position) {
    fseek($handle, $position);
  }
  $contents = fread($handle, $length);
  // Pass starting position.
  $migration['request_params']['position'] = $position;
  // Transfer contents.
  $result = _acquia_migrate_transmit($migration, $contents);

  // Set position to FALSE if the whole file has been read or if transmit failed.
  if (feof($handle) || $result === FALSE) {
    $position = FALSE;
  }
  else {
    // Get current position.
    $position = ftell($handle);
  }
  fclose($handle);
  return $position;
}

/**
 * Perform POST of archive chunk to Acquia hosting environment URL.
 */
function _acquia_migrate_transmit(&$migration, $content) {
  $params = $migration['request_params'];
  $params['nonce'] = $migration['env']['nonce'];
  $params['t'] = time();
  $params[$migration['env']['stage']] = acquia_migrate_get_token($params['t'], $params['r'], $migration['env']['secret']);

  $data = '';
  $boundary = _acquia_migrate_multipart_boundary();
  $data = _acquia_migrate_multipart_encode_params($boundary, $params, $migration['file_name'], $content);

  $headers = array(
    'Content-Type' => "multipart/form-data, boundary=$boundary",
    'User-Agent' => 'Acquia Migrate Client/1.x (Drupal ' . VERSION . ';)',
  );
  $url = $migration['env']['url'];

  $return = drupal_http_request($url, $headers, 'POST', $data);

  if ($return->code == 200) {
    $output = json_decode($return->data, TRUE);

    if (!is_array($output)) {
      $migration['error'] = t('Error occurred, please try again or consult the logs.');
      $migration['error_data'] = $return->data;
      return FALSE;
    }
    elseif (!empty($output['err'])) {
      $migration['error'] = $output['err'];
      $migration['error_data'] = $return->data;
      return FALSE;
    }
    else {
      // Validate signature.
      $response_signature = $output['sig'];
      unset($output['sig']);
      $sig = '';
      foreach ($output as $value) {
        $sig .= $value;
      }
      $signature = hash_hmac('sha256', $sig, $migration['env']['secret']);

      // Check if response is correct, if not stop migration.
      if ($signature != $response_signature) {
        $migration['error'] = t('Signature from server is wrong');
        $migration['error_data'] = $return->data;
        return FALSE;
      }
    }
  }
  elseif ($return->code == 302) {
    // Final chunk, signature and any error is in Location URL.
    $redirect_url = $return->redirect_url;
    $parsed = parse_url($redirect_url);
    parse_str($parsed['query'], $query);
    if (!empty($query['err'])) {
      $migration['error'] = $query['err'];
      $migration['error_data'] = $return->data;
      return FALSE;
    }
    else {
      $query_sig = $query['sig'];
      unset($query['sig']);

      $sig = '';
      foreach ($query as $k => $v) {
        $sig .= $v;
      }
      $signature = hash_hmac('sha256', $sig, $migration['env']['secret']);

      if ($signature == $query_sig) {
        $query['sig'] = $query_sig;
        $migration['redirect'] = array(
          'url' => $redirect_url,
          'data' => $query,
        );
      }
      else {
        $migration['error'] = t('Signature from server is wrong');
        $migration['error_data'] = $return->data;
        return FALSE;
      }
    }
  }
  else {
    $migration['error'] = t('Transfer error');
    $migration['error_data'] = $return->data;
    return FALSE;
  }
}

/**
 * Get upload security token
 *
 * @param $now
 * @param $return
 * @param $stage
 */
function acquia_migrate_get_token($now, $return, $secret) {
  return hash_hmac('sha256', $now . $return, $secret);
}

/**
 * Recursive function to find files to archive.
 */
function acquia_migrate_files_to_backup($directory, $exclude) {
  $array_items = array();
  if ($handle = opendir($directory)) {
    while (FALSE !== ($file = readdir($handle))) {
      if (!is_link($file) && !in_array($file, $exclude) && !in_array($directory . DIRECTORY_SEPARATOR . $file, $exclude)) {
        if (is_dir($directory . DIRECTORY_SEPARATOR . $file)) {
          // Do not include directories that cannot be executed to prevent
          // Archive_Tar error.
          if (@file_exists($directory . DIRECTORY_SEPARATOR . $file . DIRECTORY_SEPARATOR . '.')) {
            $array_items = array_merge($array_items, acquia_migrate_files_to_backup($directory . DIRECTORY_SEPARATOR . $file, $exclude));
          }
        }
        elseif (is_readable($directory . DIRECTORY_SEPARATOR . $file)) {
          $file = $directory . DIRECTORY_SEPARATOR . $file;
          $array_items[] = preg_replace("/\/\//si", DIRECTORY_SEPARATOR, $file);
        }
      }
    }
    closedir($handle);
  }
  return $array_items;
}

/**
 * Remove database file created for migration.
 */
function _acquia_migrate_cleanup_db(&$migration) {
  if (isset($migration['db_file'])) {
    unlink($migration['db_file']);
    unset($migration['db_file']);
  }
}

/**
 * Remove files and directory created for migration.
 */
function _acquia_migrate_cleanup(&$migration) {
  if (isset($migration['db_file'])) {
    _acquia_migrate_cleanup_db($migration);
  }
  if (isset($migration['tar_file'])) {
    unlink($migration['tar_file']);
    unset($migration['tar_file']);
  }
  if (isset($migration['dir'])) {
    if (is_dir($migration['dir'])) {
      if (!@rmdir($migration['dir'])) {
        // Files leftover in directory, reconstruct names and remove.
        $db_file = $migration['file'] . '.sql';
        if (file_exists($db_file)) {
          unlink($db_file);
        }
        $tar_file = $migration['file'] . '.tar';
        $tar_file .= !empty($migration['compression_ext']) ? '.' . $migration['compression_ext'] : '';
        if (file_exists($tar_file)) {
          unlink($tar_file);
        }
        rmdir($migration['dir']);
      }
    }
    unset($migration['dir']);
  }
  variable_set('acquia_agent_cloud_migration', $migration);
}

/**
 * Dump mysql datbase, modified from Backup & Migrate module by ronan.
 */

/**
 * Dump the database to the specified file.
 */
function _acquia_migrate_backup_db_to_file_mysql(&$migration) {
  // Check migration file at first to avoid dumping db to a hidden file.
  if (!isset($migration['file'])) {
    $migration['error'] = TRUE;
    return;
  }
  $file = $migration['file'] . '.sql';
  $handle = fopen($file, 'w');
  $lines = 0;
  $exclude = array();
  $nodata = $migration['no_data_tables'];
  if ($handle) {
    $data = _acquia_migrate_get_sql_file_header_mysql();
    fwrite($handle, $data);
    $alltables = _acquia_migrate_get_tables_mysql();
    foreach ($alltables as $table) {
      if ($table->Name && !isset($exclude[$table->Name])) {
        $data = _acquia_migrate_get_table_structure_sql_mysql($table);
        fwrite($handle, $data);
        $lines++;
        if (!in_array($table->Name, $nodata)) {
          $lines += _acquia_migrate_dump_table_data_sql_to_file($handle, $table);
        }
      }
    }
    $data = _acquia_migrate_get_sql_file_footer_mysql();
    fwrite($handle, $data);
    $stat = fstat($handle);
    fclose($handle);
    // Set migration details.
    $migration['db_size'] = $stat['size'];
    $migration['db_file'] = $file;
  }
  else {
    $migration['error'] = TRUE;
  }
}

/**
 * Get the sql for the structure of the given table.
 */
function _acquia_migrate_get_table_structure_sql_mysql($table) {
  $out = "";

  $result = db_query("SHOW CREATE TABLE `" . $table->Name ."`");
  if ($record = db_fetch_array($result)) {
    $out .= "DROP TABLE IF EXISTS `" . $table->Name . "`;\n";
    $out .= strtr($record['Create Table'], array("\n" => " ", '"' => '`'));
    if (!empty($table->Auto_increment)) {
      $out .= " AUTO_INCREMENT=" . $table->Auto_increment;
    }
    $out .= ";\n";
  }
  return $out;
}

/**
 *  Get the sql to insert the data for a given table
 */
function _acquia_migrate_dump_table_data_sql_to_file($handle, $table) {
  $lines = 0;
  $result = db_query("SELECT * FROM `" . $table->Name . "`");
  while ($row = db_fetch_array($result)) {
    $items = array();
    foreach ($row as $key => $value) {
      $items[] = is_null($value) ? "null" : "'" . db_escape_string($value) . "'";
    }
    if ($items) {
      $data = "INSERT INTO `" . $table->Name . "` VALUES (" . implode(",", $items) . ");\n";
      fwrite($handle, $data);
      $lines++;
    }
  }
  return $lines;
}

/**
 * The header for the top of the sql dump file. These commands set the connection
 *  character encoding to help prevent encoding conversion issues.
 */
function _acquia_migrate_get_sql_file_header_mysql() {
  return "
/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE=NO_AUTO_VALUE_ON_ZERO */;

SET NAMES utf8;
";
}

/**
 * The footer of the sql dump file.
 */
function _acquia_migrate_get_sql_file_footer_mysql() {
  return "

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
";
}

/**
 * Get a list of tables in the db.
 */
function _acquia_migrate_get_tables_mysql() {
  $out = array();
  // get auto_increment values and names of all tables
  $tables = db_query("show table status");
  while ($table = db_fetch_object($tables)) {
    $out[$table->Name] = $table;
  }
  return $out;
}

/**
 * Create multipart boundary
 *
 * @return
 *   Boundary
 */
function _acquia_migrate_multipart_boundary() {
  return '---------------------------' . substr(md5(rand(0, 32000)), 0, 10);
}

/**
 * Encode params array as multipart/form-data string
 *
 * @param $boundary
 *   Boundary to delimit parameters
 * @param $params
 *   Form data array
 * @return
 *   Encoded string for drupal_http_request
 */
function _acquia_migrate_multipart_encode_params($boundary, $params, $filename, $content) {
  $output = "";
  foreach ($params as $key => $value) {
    $output .= "--$boundary\r\n";
    $output .= _acquia_migrate_multipart_enc_text($key, $value);
  }
  $output .= "--$boundary\r\n";
  $output .= _acquia_migrate_multipart_enc_file('files[u]', $filename, $content);
  $output .= "--$boundary--";
  return $output;
}

/**
 * Encode simple param
 * @param type $name
 * @param type $value
 * @return type
 */
function _acquia_migrate_multipart_enc_text($name,  $value) {
  return "Content-Disposition: form-data; name=\"$name\"\r\n\r\n$value\r\n";
}

function _acquia_migrate_multipart_enc_file($name, $filename, $file_content) {
  $mimetype = "application/octet-stream";
  $data = "Content-Disposition: form-data; name=\"$name\"; filename=\"$filename\"\r\n";
  $data .= "Content-Transfer-Encoding: binary\r\n";
  $data .= "Content-Type: $mimetype\r\n\r\n";
  $data .= $file_content . "\r\n";
  return $data;
}

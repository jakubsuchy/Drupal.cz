<?php
// $Id: boost_stats.php,v 1.1.2.11 2009/09/12 23:03:58 mikeytown2 Exp $

if (!isset($_GET['js'])) {
  // stats not called via JS, send image out & close connection.
  boost_stats_async_image();
}

// Exit script if nothing was passed to it.
if (   !isset($_GET['nid'])
    && !isset($_GET['title'])
    && !isset($_GET['qq'])
    && !isset($_GET['referer'])
    ) {
  exit;
}

// connect to db & set variables.
boost_stats_init();

// Set node counter.
if (boost_stats_variable_get('statistics_count_content_views')) {
  boost_stats_update_node_counter();
}

// Set access log.
if (boost_stats_variable_get('statistics_enable_access_log')) {
  boost_stats_add_access_log();
}

if (isset($_GET['js'])) {
  if ($_GET['js'] == 1) {
    $json = array();
    // Get stats block html.
    $json = array_merge($json, boost_stats_output_stats_block());

    // Send JSON Back
    if (!empty($json)) {
      header('Content-Type: text/javascript; charset=utf-8');
      echo json_encode($json);
    }
  }
  // Send HTML back
  elseif ($_GET['js'] == 2) {
    echo array_pop(boost_stats_output_stats_block());
  }
}

// end of script, exit.
exit;



function boost_stats_async_image() {
  // Script should take under 1MB of memory to work.
  // Prime php for background operations
  while (ob_get_level()) {
    ob_end_clean();
  }
  header("Connection: close");
  ignore_user_abort();

  // Output of 1 pixel transparent gif
  ob_start();
  header("Content-type: image/gif");
  header("Expires: Wed, 11 Nov 1998 11:11:11 GMT");
  header("Cache-Control: no-cache");
  header("Cache-Control: must-revalidate");
  header("Content-Length: 43");
  header("Connection: close");
  printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",71,73,70,56,57,97,1,0,1,0,128,255,0,192,192,192,0,0,0,33,249,4,1,0,0,0,0,44,0,0,0,0,1,0,1,0,0,2,2,68,1,0,59);
  ob_end_flush();
  flush();

  // Image returned and connection closed.
  // Do background processing. Time taken below should not effect page load times.
}

function boost_stats_init() {
  Global $nid, $title, $q, $referer, $session_id, $uid, $count_views, $access_log, $variables;

  // Connect to DB.
  include_once './includes/bootstrap.inc';
  drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);

  // Set variables passed via GET.
  $nid = (isset($_GET['nid']) && is_numeric($_GET['nid'])) ? $_GET['nid'] : NULL;
  $title = (isset($_GET['title']) && $_GET['title'] != 'NULL') ? urldecode($_GET['title']) : NULL;
  $q = (isset($_GET['qq']) && $_GET['qq'] != 'NULL') ? $_GET['qq'] : NULL;
  $q = (isset($_GET['q']) && $_GET['q'] != 'NULL') ? $_GET['q'] : NULL;
  $referer = isset($_GET['referer']) ? $_GET['referer'] : NULL;

  // $session_id only goes in the access log; only used for stats, not creds.
  $session_id = session_id();
  if (empty($session_id)) {
    if (empty($_COOKIE[session_name()])) {
      if (empty($_SERVER['HTTP_USER_AGENT'])) {
        $session_id = md5(ip_address());
      }
      else {
        $session_id = md5($_SERVER['HTTP_USER_AGENT'] . ip_address());
      }
    }
    else {
      $session_id = $_COOKIE[session_name()];
    }
  }

  // Anonymous users always get a User ID of 0.
  $uid = 0;

  // load all boost, statistics & throttle variables; 1 transaction instead of multiple
  $result = db_query("
SELECT * FROM {variable}
WHERE name LIKE 'boost_%'
OR name LIKE 'statistics_%'
OR name LIKE 'throttle_%'
");
  while ($variable = db_fetch_object($result)) {
    $variables[$variable->name] = unserialize($variable->value);
  }
}

function boost_stats_update_node_counter() {
  Global $nid;

  // A node has been viewed, so update the node's counters.
  db_query('UPDATE {node_counter} SET daycount = daycount + 1, totalcount = totalcount + 1, timestamp = %d WHERE nid = %d', time(), $nid);
  // If we affected 0 rows, this is the first time viewing the node.
  if (!db_affected_rows()) {
    // We must create a new row to store counters for the new node.
    db_query('INSERT INTO {node_counter} (nid, daycount, totalcount, timestamp) VALUES (%d, 1, 1, %d)', $nid, time());
  }
}

function boost_stats_add_access_log() {
  Global $title, $q, $referer, $session_id, $uid;

  db_query("INSERT INTO {accesslog} (title, path, url, hostname, uid, sid, timer, timestamp) values('%s', '%s', '%s', '%s', %d, '%s', %d, %d)", $title, $q, $referer, ip_address(), $uid, $session_id, timer_read('page'), time());
}

function boost_stats_output_stats_block() {
  if (boost_stats_variable_get('boost_block_show_stats')) {
    if (!boost_stats_variable_get('boost_block_cache_stats_block') && !boost_stats_variable_get('throttle_level')) {
      boost_stats_full_boot();
      $block = module_invoke('statistics', 'block', 'view', 0);
      $block = $block['content'];
    }
    else {
      $block = boost_stats_variable_get('boost_statistics_html');
    }
  }
  else {
    $block = 'NULL';
  }

  return array('#boost-stats' => $block);
}

function boost_stats_full_boot() {
  Global $full_boot;
  if (!isset($full_boot)) {
    $full_boot = FALSE;
  }
  if (!$full_boot) {
    include_once './includes/bootstrap.inc';
    drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
  }
  $full_boot = TRUE;
}

function boost_stats_variable_get($name) {
  Global $variables;
  return isset($variables[$name]) ? $variables[$name] : FALSE;
}
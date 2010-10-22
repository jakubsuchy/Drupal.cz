<?php
// $Id: banner_file.php,v 1.20.2.3 2006/09/23 14:39:43 wulff Exp $

/**
 * @file
 * The file system cache handler for the banner module.
 */

header("Content-Type: application/x-javascript; charset=utf-8");

$group = (int) $_GET['group'];
$count = (int) $_GET['count'];
$terms = (string) $_GET['terms'];
$path = (string) $_GET['path'];
$max = (int) $_GET['max'];
$refresh = 0;
$output = '';

for ($i = 1; $i <= $max; $i++) {
  $cache_file = $path.'/.'.$i.'.banner.cache';
  if (!$fd = @fopen($cache_file, "r+")) {
    if ($i == $max) {
      // we failed to open all cache files, try refreshing
      $refresh = 1;
    }
    continue;
  }
  // determine whether to use a blocking lock or a non-blocking lock
  if ($i < $max) {
    if (!flock($fd, LOCK_EX|LOCK_NB)) {
      @fclose($fd);
      continue;
    }
  }
  else { // last cache file, use a blocking-lock
    if (!flock($fd, LOCK_EX)) {
      // something is very wrong, try refreshing cache
      @fclose($fd);
      $refresh = 1;
      break;
    }
  }
  $contents = fread($fd, filesize($cache_file));

  $struct = unserialize($contents);
  $last_updated = $struct[2];

  $tids = explode(',', $terms);

  $ballot_and = array();
  foreach ($tids as $tid) {
    if (isset($struct[0][$tid][$group])) {
      $ballot_and[] = $struct[0][$tid][$group];
    }
  }
  $ballot_or = array();
  foreach ($tids as $tid) {
    if (isset($struct[0][$tid][$group])) {
      foreach ($struct[0][$tid][$group] as $nid)
      $ballot_or[] = $nid;
    }
  }

  $ballot = array();
  if (count($ballot_and) >= 2) {
    $ballot = call_user_func_array('array_intersect', $ballot_and);
  }
  if (empty($ballot)) {
    $ballot = array_unique($ballot_or);
  }
  if (empty($ballot)) {
    foreach ($struct[0][0][$group] as $nid) {
      $ballot[] = $nid;
    }
  }

  $counter = 0;
  while ($counter < $count && count($ballot)) {
    // select a random banner
    $last = count($ballot) - 1;
    if ($last > 0) {
      $random = mt_rand(0, $last);
    }
    else {
      $random = 0;
    }
    $banner_id = $ballot[$random];

    // get banner and remove it from the ballot
    $this_banner = $struct[1][$banner_id];
    array_splice($ballot, $random, 1);

    // add a view
    $struct[1][$banner_id]->views_this++;
    $struct[1][$banner_id]->views++;
    $struct[1][$banner_id]->views_day++;
    $struct[1][$banner_id]->views_week++;

    // if needed, disable this banner
    if ($this_banner->views_max > 0 && ($this_banner->views + 1) >= $this_banner->views_max) {
      include_once './includes/bootstrap.inc';
      drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);
      // reached maximum views, set status to "blocked"
      db_query('UPDATE {banner} SET workflow = 5 WHERE nid = %d', $banner_id);
      $refresh = 1;
    }
    else if ($this_banner->views_day_max > 0 && ($this_banner->views_day + 1) >= $this_banner->views_day_max) {
      include_once './includes/bootstrap.inc';
      drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);
      // reached day's maximum views, set status to "day's limit reached"
      db_query('UPDATE {banner} SET workflow = 2 WHERE nid = %d', $banner_id);
      $refresh = 1;
    }
    else if ($this_banner->views_week_max > 0 && ($this_banner->views_week + 1) >= $this_banner->views_week_max) {
      include_once './includes/bootstrap.inc';
      drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);
      // reached week's maximum views, set status to "week's limit reached"
      db_query('UPDATE {banner} SET workflow = 3 WHERE nid = %d', $banner_id);
      $refresh = 1;
    }

    $output .= $this_banner->html;

    $counter++;
  }

  // once every minute update views in db
  if ($last_updated < (time() - 60)) {
    $struct[2] = time();
    $refresh = 1;
  }

  // dump back in cache
  $data = serialize($struct);
  rewind($fd);
  fwrite($fd, $data, strlen($data));
  flock($fd, LOCK_UN);
  fclose($fd);
  break;
}

if ($refresh) {
  include_once './includes/bootstrap.inc';
  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
  drupal_set_header("Content-Type: application/x-javascript; charset=utf-8");
  _banner_refresh_cache();
}

print $output;

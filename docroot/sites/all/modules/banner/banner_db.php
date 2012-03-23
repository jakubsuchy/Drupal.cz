<?php
// $Id: banner_db.php,v 1.24.2.1 2006/09/23 14:39:43 wulff Exp $

/**
 * @file
 * The database cache handler for the banner module.
 */

header("Content-Type: application/x-javascript; charset=utf-8");
 
include_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);

$group = (int) $_GET['group'];
$count = (int) $_GET['count'];
$terms = (string) $_GET['terms'];

// build queries
$queries = array();
$columns = ', b.nid, b.chance, b.cache, b.views, b.views_max, b.views_day, b.views_day_max, b.views_week, b.views_week_max, n.uid';
if ($terms != '0') {
  // build query to group terms by vocabulary
  $tids = explode(',', $terms);
  for ($i = 0; $i < count($tids); $i++) {
    $like[] = '%d';
  }
  $query = 'SELECT DISTINCT(tn.tid), td.vid FROM {term_node} tn INNER JOIN {term_data} td ON tn.tid = td.tid WHERE tn.tid IN ('. implode(',', $like) .')';

  $result = db_query($query, $tids);

  $tidgroup = array();
  while ($row = db_fetch_array($result)) {
    $tidgroup[$row['vid']][] = $row['tid'];
  }

  // build query for AND matching between vocabularies
  $join = '';
  $where = array();
  foreach ($tidgroup as $vid => $tids) {
    $join .= ' INNER JOIN {term_node} tn'. $vid .' ON b.nid = tn'. $vid .'.nid';
    $where[] = 'tn'. $vid .'.tid IN ('. implode(',', $tids) .')';
  }
  $query = 'SELECT DISTINCT(b.vid)'. $columns .' FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid INNER JOIN {banner} b ON n.vid = b.vid'. $join .' WHERE tn.tid = %d AND b.workflow = 1';

  if ($where) {
    $query .= ' AND ('. implode(' AND ', $where) .')';
  }
  $queries[] = array($query , $group);

  // build query for OR matching between vocabularies
  $join = '';
 $where = array();
  foreach ($tidgroup as $vid => $tids) {
    $join .= ' INNER JOIN {term_node} tn'. $vid .' ON b.nid = tn'. $vid .'.nid';
    $where[] = 'tn'. $vid .'.tid IN ('. implode(',', $tids) .')';
  }
  $query = 'SELECT DISTINCT(b.vid)'. $columns .' FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid INNER JOIN {banner} b ON n.vid = b.vid'. $join .' WHERE tn.tid = %d AND b.workflow = 1';
  if ($where) {
    $query .= ' AND ('. implode(' OR ', $where) .')';
  }
  $queries[] = array($query , $group);
}
$queries[] = array('SELECT b.vid'. $columns .' FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid INNER JOIN {banner} b ON n.vid = b.vid WHERE tn.tid = %d AND b.workflow = 1', $group);

// run queries, stop after first query which returns one or more rows
foreach ($queries as $query) {
  $result = db_query($query[0], $query[1]);
  if (db_num_rows($result) > 0) {
    break;
  }
}

// build ballot
$ballot = array();
while ($banner = db_fetch_object($result)) {
  for ($i = 0; $i < $banner->chance; $i++) {
    $ballot[] = $banner->nid;
  }
  $banners[$banner->nid] = $banner;
}
// get uid of current user
$result = db_query("SELECT uid, sid FROM {sessions} WHERE sid = '%s'", $_COOKIE[session_name()]);
if (db_num_rows($result)) {
  $session = db_fetch_array($result);
  $uid = $session['uid'];
}
else {
  $uid = 0;
}

$counter = 0;
while ($counter < $count && count($ballot)) {
  // choose random banner
  $max = count($ballot) - 1;
  if ($max > 0) {
    $random = mt_rand(0, $max);
  }
  else {
    $random = 0;
  }
  $nid = $ballot[$random];

  // get banner and remove it from the ballot
  $banner = $banners[$nid];
  array_splice($ballot, $random, 1);

  // update view statistics, admin and owner views are not counted
  if ($uid != 1 && $uid != $banner->uid) {
    db_query('UPDATE {banner} SET views = views + 1, views_day = views_day + 1, views_week = views_week + 1 WHERE vid = %d', $banner->vid);
  }

  // change banner status?
  if ($banner->views_max > 0 && $banner->views >= $banner->views_max) {
    // limit reached
    db_query('UPDATE {banner} SET workflow = 5 WHERE vid = %d', $banner->vid);
  }
  else if ($banner->views_day_max > 0 && $banner->views_day >= $banner->views_day_max) {
    // day limit reached
    db_query('UPDATE {banner} SET workflow = 2 WHERE vid = %d', $banner->vid);
  }
  else if ($banner->views_week_max > 0 && $banner->views_week >= $banner->views_week_max) {
    // week limit reached
    db_query('UPDATE {banner} SET workflow = 3 WHERE vid = %d', $banner->vid);
  }

  print $banner->cache;

  $counter++;
}

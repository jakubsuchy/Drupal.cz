<?php

// $Id: taxonomy_menu.database.inc,v 1.1.2.10 2009/03/25 21:29:13 indytechcook Exp $

/**
 * @file
 * Database functions
 * 
 * @author Neil Hastings      <http://drupal.org/user/245817>
 * @author Mark Theunissen    <http://drupal.org/user/108606>
 * @author Afief Halumi       <http://drupal.org/user/237472>
 */

 /**
 *  helper function to insert a menu item
 *
 * @param $mlid
 * @param $tid
 * @param $vid
 */
function _taxonomy_menu_insert_menu_item($mlid, $tid, $vid) {
  db_query('INSERT INTO {taxonomy_menu} (mlid, tid, vid) VALUES (%d, %d, %d)', $mlid, $tid, $vid);
}

/**
 * Return the corresponding menu link id.
 *
 * @param $tid
 *   the term's id
 */
function _taxonomy_menu_get_mlid($tid, $vid) {
  return db_result(db_query('SELECT mlid FROM {taxonomy_menu} WHERE tid = %d AND vid = %d', $tid, $vid));
}

/**
 * Retrieve the term / menu relations for a vocab.
 *
 * @param $vid
 *   vocabulary's id
 * @return
 *   array(tid => mlid)
 */
function _taxonomy_menu_get_menu_items($vid) {

  $result = db_query('SELECT mlid, tid FROM {taxonomy_menu} WHERE vid = %d', $vid);
  $menu_items = array();
  while ($data = db_fetch_object($result)) {
    $menu_items[$data->tid] = $data->mlid;
  }
  return $menu_items;
}

 /**
  * Delete all links associated with this vocab from both the taxonomy_menu
  * table and the menu_link table.
  *
  * @param $vid
  *   vocabulary's id
  */
function _taxonomy_menu_delete_all($vid) {
  $menu_terms = _taxonomy_menu_get_menu_items($vid);
  if (!empty($menu_terms)) {
    foreach ($menu_terms as $tid => $mlid) {
      db_query('DELETE FROM {menu_links} WHERE mlid = %d', $mlid);
    }
    db_query('DELETE FROM {taxonomy_menu} WHERE vid = %d', $vid);
  }
}

/**
 * Get an array of the tid's related to the node
 * 
 * @param $nid
 * @return array of tids
 */
function _taxonomy_menu_get_node_terms($nid) {
  $output = array();
  $result = db_query('SELECT tid FROM {term_node} WHERE nid = %d', $nid);
  while ($data = db_fetch_object($result)) {
    $output[] = $data->tid;
  }
  return $output;
}

/**
 * Get an array of the tid's from the parent
 * 
 * @param $tid
 * @return array of tid
 */
function _taxonomy_menu_get_parents($tid) {
  $output = array();
  $result = taxonomy_get_parents($tid);
  foreach ($result as $key => $item) {
    $output[] = $key;
  }
  return $output;  
}

/**
  * Delete all rows from {taxomony_menu} associated with this tid
  *
  * @param $vid
  * @param $tid
  */
function _taxonomy_menu_delete_item($vid, $tid) {
  db_query('DELETE FROM {taxonomy_menu} WHERE vid = %d AND tid = %d', $vid, $tid);
}

/**
 * Get all of the tid for a given vid
 *
 * @param $vid
 * @return array of $tid
 */
function _taxonomy_menu_get_terms($vid) {
  $output = array();
  $result = db_query('SELECT tid FROM {term_data} WHERE vid = %d', $vid);
  while ($data = db_fetch_object($result)) {
    $output[] = $data->tid;
  }
  return $output;
}

/**
 * used to get the count without children
 * 
 * @param $tid
 */
function _taxonomy_menu_term_count($tid) {
  return db_result(db_query('SELECT COUNT(n.nid) AS c FROM {term_node} t INNER JOIN {node} n ON t.vid = n.vid WHERE n.status = 1 AND t.tid = %d', $tid));
}

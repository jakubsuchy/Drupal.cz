<?php
// $Id: username_check.unique.inc,v 1.5 2009/09/24 12:57:59 duke Exp $

/**
 * @file
 * AJAX callbacks for the username_check module.
 */

/**
 * Main AJAX function: originality check menu callback.
 */
function username_check_callback() {
  $output = array();

  $username = $_GET['username'];

  // Проверка на наличие недопустимых символов
  $ret = user_validate_name($username);
  if ($ret) {
    $output['allowed'] = FALSE;
    $output['msg'] = $ret;
  }
  else {
    // Проверка на соответствие Access Rules
    $ret = drupal_is_denied('user', $username);
    if ($ret) {
      $output['allowed'] = FALSE;
      $output['msg'] = t('The username %username is not allowed.', array('%username' => $username));
    }
    else {
      $username = check_plain($username);
      $ret = _username_check_is_user_exists($username);

      if ($ret) {
        $output['allowed'] = FALSE;
        $output['msg'] = t('The name %username is already taken.', array('%username' => $username));
      }
      else {
        $output['allowed'] = TRUE;
        $output['msg'] = t('The username %username is available.', array('%username' => $username));
      }
    }
  }

  drupal_page_header();

  print drupal_to_js($output);
  exit();
}

/**
 * Query user table to check if such username is already exists.
 */
function _username_check_is_user_exists($username) {
  return db_result(db_query("SELECT COUNT(u.name) FROM {users} u WHERE LOWER(u.name) = LOWER('%s')", $username));
}

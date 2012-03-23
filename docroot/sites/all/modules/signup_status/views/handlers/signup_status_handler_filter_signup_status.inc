<?php
// $Id: signup_status_handler_filter_signup_status.inc,v 1.2 2009/01/19 11:02:54 miglius Exp $

/**
 * @file
 * Implements signup status views fiilter handler
 */

/**
 * Filter signups based on their signup status.
 */
class signup_status_handler_filter_signup_status extends views_handler_filter_in_operator {
  function get_value_options() {
    $options = array();
    $codes = signup_status_codes();
    foreach ($codes as $cid => $code) {
      $options[$cid] = $code['name'];
    }
    $this->value_options = $options;
  }
}


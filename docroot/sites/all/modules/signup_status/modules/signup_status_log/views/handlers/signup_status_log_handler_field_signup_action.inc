<?php
// $Id: signup_status_log_handler_field_signup_action.inc,v 1.1 2009/01/30 18:34:12 miglius Exp $

/**
 * @file
 * Provides views handler.
 */

/**
 * Field handler to display the signup action.
 */
class signup_status_log_handler_field_signup_action extends views_handler_field {
  function render($values) {
    return t($values->{$this->field_alias});
  }
}


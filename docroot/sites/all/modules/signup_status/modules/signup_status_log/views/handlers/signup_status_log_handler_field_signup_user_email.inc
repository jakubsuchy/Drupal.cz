<?php
// $Id: signup_status_log_handler_field_signup_user_email.inc,v 1.1 2009/01/30 18:34:12 miglius Exp $

/**
 * @file
 * Provides views handler.
 */

/**
 * Field handler to display the email address of a signed up user.
 *
 * This requires the relationship between {signup_status_log} and {users} when
 * generating the query.  When rendering the field, if the uid is 0 (anonymous
 * signup) it pulls the {signup_status_log}.anon_mail field.  For authenticated
 * users, it grabs the {users}.mail field from the relationship.
 */
class signup_status_log_handler_field_signup_user_email extends views_handler_field {
  var $table_alias = '';
  var $users_alias = '';

  function ensure_my_table() {
    $this->table_alias = $this->query->ensure_table('signup_status_log', $this->relationship);
    $this->users_table = $this->query->ensure_table('users', 'signup_status_log');
  }

  function query() {
    $this->ensure_my_table();
    $this->aliases['signup_uid'] = $this->query->add_field($this->table_alias, 'uid');
    $this->aliases['signup_anon_mail'] = $this->query->add_field($this->table_alias, 'anon_mail');
    $this->aliases['users_mail'] = $this->query->add_field($this->users_table, 'mail');
  }

  function render($values) {
    if (empty($values->{$this->aliases['signup_uid']})) {
      return check_plain($values->{$this->aliases['signup_anon_mail']});
    }
    else {
      return check_plain($values->{$this->aliases['users_mail']});
    }
  }
}


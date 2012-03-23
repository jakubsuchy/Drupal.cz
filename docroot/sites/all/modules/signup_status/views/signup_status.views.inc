<?php
// $Id: signup_status.views.inc,v 1.1 2009/01/12 18:43:44 dww Exp $

/**
 * @file
 * Provides support for Views integration.
 *
 * This exposes signup_status data and declares handlers (fields, filters)
 * and plugins (argument validators).
 */

/**
 * Implementation of hook_views_data().
 */
function signup_status_views_data() {
  // signup.views.inc already defines this table, we're just adding a field.
  $data['signup_log']['status'] = array(
    'title' => t('User: status'),
    'help' => t("The status of each user's signup to a specific event."),
    // Information for displaying the field
    'field' => array(
      'handler' => 'signup_status_handler_field_signup_status',
      'click sortable' => TRUE,
    ),
    // Information for accepting status as a filter
    'filter' => array(
      'handler' => 'signup_status_handler_filter_signup_status',
      'help' => t("Filter on each user's signup status."),
      'label' => 'Signup user status',
    ),
    // Information for sorting on status
    'sort' => array(
      'handler' => 'views_handler_sort',
      'help' => t("Sort by the user's signup status."),
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );
  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function signup_status_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'signup_status') .'/views/handlers',
    ),
    'handlers' => array(
      'signup_status_handler_field_signup_status' => array(
        'parent' => 'views_handler_field',
      ),
      'signup_status_handler_filter_signup_status' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}


<?php
// $Id: signup_status_log.views.inc,v 1.1 2009/01/30 18:34:12 miglius Exp $

/**
 * @file
 * Provides support for views integration.
 */

/**
 * Implementation of hook_views_data().
 */
function signup_status_log_views_data() {

  $data['signup_status_log']['table']['group'] = t('Signup status log');

  $data['signup_status_log']['table']['base'] = array(
    'field' => 'lid',
    'title' => t('Signup status log'),
    'help' => t('A signup status represents a relationship between a user and a node.'),
  );

  $data['signup_status_log']['sid'] = array(
    'title' => t('Signup'),
    'help' => t('The signup.'),
    'relationship' => array(
      'base' => 'signup_log',
      'handler' => 'views_handler_relationship',
      'label' => t('Signup'),
    ),
  );

  $data['signup_status_log']['nid'] = array(
    'title' => t('Node'),
    'help' => t('The node a user signed up to.'),
    'relationship' => array(
      'base' => 'node',
      'handler' => 'views_handler_relationship',
      'label' => t('Signup node'),
    ),
  );

  $data['signup_status_log']['uid'] = array(
    'title' => t('User'),
    'help' => t('The user who signed up.'),
    'relationship' => array(
      'base' => 'users',
      'handler' => 'views_handler_relationship',
      'label' => t('Signup user'),
    ),
  );

  $data['signup_status_log']['admin_uid'] = array(
    'title' => t('Changed by'),
    'help' => t('The user who has changed the signup.'),
    'relationship' => array(
      'base' => 'users',
      'handler' => 'views_handler_relationship',
      'label' => t('Changed by user'),
    ),
  );

  $data['signup_status_log']['timestamp'] = array(
    'title' => t('Signup Change Time'),
    'help' => t('Time when user signed up.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
      'help' => t('Time when the user\'s signup was changed.'),
      'label' => t('Signup change time'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
      'help' => t('Sort by time of signup change.'),
    ),
  );

  $data['signup_status_log']['action'] = array(
    'title' => t('Signup Action'),
    'help' => t('What type of action was performed.'),
    'field' => array(
      'handler' => 'signup_status_log_handler_field_signup_action',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter',
      'help' => t('What type of action was performed.'),
      'label' => t('Signup change action'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'help' => t('Sort by the signup atcion.'),
    ),
  );

  $data['signup_status_log']['cid'] = array(
    'title' => t('Signup Status'),
    'help' => t('The status of the signed up user.'),
    'field' => array(
      'handler' => 'signup_status_handler_field_signup_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'signup_status_handler_filter_signup_status',
      'help' => t('The status of the signed up user.'),
      'label' => t('Signup status'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'help' => t('Sort by the signup status.'),
    ),
  );

  $data['signup_status_log']['email'] = array(
    'title' => t('User: Email Address'),
    'click sortable' => TRUE,
    'field' => array(
      'handler' => 'signup_status_log_handler_field_signup_user_email',
      'help' => t('Email address of a user (authenticated or anonymous) who signed up.') .' <strong>'. t('WARNING: only expose this data in a vie    w that is restricted to users whom you can trust with such sensitive information.') .'</strong>',
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 *
 * Register information about all of the views handlers provided by the signup
 * status log module.
 */
function signup_status_log_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'signup_status_log') .'/views/handlers',
    ),
    'handlers' => array(
      'signup_status_log_handler_field_signup_user_email' => array(
        'parent' => 'views_handler_field',
      ),
      'signup_status_log_handler_field_signup_action' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}


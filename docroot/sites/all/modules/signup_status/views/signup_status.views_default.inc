<?php
// $Id: signup_status.views_default.inc,v 1.2 2009/02/02 12:19:15 miglius Exp $

/**
 * @file
 * Provides default views for the Signup status module.
 */

/**
 * Implementation of hook_views_default_views().
 */
function signup_status_views_default_views() {
  $views = array();

  if (module_exists('views_bulk_operations')) {
    $view = new view;
    $view->name = 'signup_status_user_vbo_admin';
    $view->description = t('Per-node signup administration interface with checkboxes for bulk operations.');
    $view->tag = 'Signup status';
    $view->view_php = '';
    $view->base_table = 'signup_log';
    $view->is_cacheable = FALSE;
    $view->api_version = 2;
    $view->disabled = FALSE;
    $handler = $view->new_display('default', 'Defaults', 'default');
    $handler->override_option('relationships', array(
      'nid' => array(
        'label' => 'Signup node',
        'required' => 1,
        'id' => 'nid',
        'table' => 'signup_log',
        'field' => 'nid',
        'relationship' => 'none',
      ),
      'uid' => array(
        'label' => 'Signup user',
        'required' => 1,
        'id' => 'uid',
        'table' => 'signup_log',
        'field' => 'uid',
        'relationship' => 'none',
      ),
    ));
    $handler->override_option('fields', array(
      'name' => array(
        'label' => 'Name',
        'link_to_user' => 1,
        'exclude' => 0,
        'id' => 'name',
        'table' => 'users',
        'field' => 'name',
        'relationship' => 'uid',
      ),
      'signup_time' => array(
        'label' => 'Signup time',
        'date_format' => 'small',
        'custom_date_format' => '',
        'exclude' => 0,
        'id' => 'signup_time',
        'table' => 'signup_log',
        'field' => 'signup_time',
        'relationship' => 'none',
      ),
      'status' => array(
        'label' => 'Status',
        'exclude' => 0,
        'id' => 'status',
        'table' => 'signup_log',
        'field' => 'status',
        'relationship' => 'none',
      ),
      'form_data' => array(
        'label' => 'Additional info',
        'type' => 'ul',
        'separator' => ', ',
        'empty' => '',
        'exclude' => 0,
        'id' => 'form_data',
        'table' => 'signup_log',
        'field' => 'form_data',
        'relationship' => 'none',
        'form_data_fieldname' => '',
      ),
      'attended' => array(
        'label' => 'Attendance',
        'exclude' => 0,
        'id' => 'attended',
        'table' => 'signup_log',
        'field' => 'attended',
        'relationship' => 'none',
      ),
      'edit_link' => array(
        'label' => 'Operations',
        'text' => '',
        'exclude' => 0,
        'id' => 'edit_link',
        'table' => 'signup_log',
        'field' => 'edit_link',
        'relationship' => 'none',
      ),
    ));
    $handler->override_option('arguments', array(
      'nid' => array(
        'default_action' => 'ignore',
        'style_plugin' => 'default_summary',
        'style_options' => array(),
        'wildcard' => 'all',
        'wildcard_substitution' => 'All',
        'title' => '',
        'default_argument_type' => 'fixed',
        'default_argument' => '',
        'validate_type' => 'signup_status',
        'validate_fail' => 'not found',
        'break_phrase' => 0,
        'not' => 0,
        'id' => 'nid',
        'table' => 'node',
        'field' => 'nid',
        'relationship' => 'nid',
        'default_options_div_prefix' => '',
        'default_argument_user' => 0,
        'default_argument_fixed' => '',
        'default_argument_php' => '',
        'validate_argument_node_access' => 0,
        'validate_argument_nid_type' => 'nid',
        'validate_argument_vocabulary' => array(),
        'validate_argument_type' => 'tid',
        'validate_argument_signup_status' => 'any',
        'validate_argument_signup_node_access' => 1,
        'validate_argument_php' => '',
      ),
    ));
    $handler->override_option('access', array(
      'type' => 'none',
    ));
    $handler->override_option('items_per_page', 100);
    $handler->override_option('style_plugin', 'bulk');
    $handler->override_option('style_options', array(
      'grouping' => '',
      'override' => 1,
      'sticky' => 0,
      'order' => 'asc',
      'columns' => array(
        'name' => 'name',
        'signup_time' => 'signup_time',
        'status' => 'status',
        'form_data' => 'form_data',
        'attended' => 'attended',
        'edit_link' => 'edit_link',
      ),
      'info' => array(
        'name' => array(
          'sortable' => 1,
          'separator' => '',
        ),
        'signup_time' => array(
          'sortable' => 1,
          'separator' => '',
        ),
        'status' => array(
          'sortable' => 1,
          'separator' => '',
        ),
        'form_data' => array(
          'separator' => '',
        ),
        'attended' => array(
          'sortable' => 1,
          'separator' => '',
        ),
        'edit_link' => array(
          'separator' => '',
        ),
      ),
      'default' => '-1',
      'execution_type' => '1',
      'display_type' => '0',
      'skip_confirmation' => 0,
      'display_result' => 1,
      'selected_operations' => array(
        '5c7373a6aa7a8bf5549412c9f887c2d3' => '5c7373a6aa7a8bf5549412c9f887c2d3',
        'd3a1a31a80ae6f3a3445aa18d827c62e' => 'd3a1a31a80ae6f3a3445aa18d827c62e',
        '486e69f274fc0c7fb96d00a55ce752ae' => '486e69f274fc0c7fb96d00a55ce752ae',
        '3bf1db3baf6d97fc016412257c02f5ef' => '3bf1db3baf6d97fc016412257c02f5ef',
      ),
    ));
    $views[$view->name] = $view;
  }

  return $views;
}


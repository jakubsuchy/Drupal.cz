<?php
// $Id: signup_status_log.views_default.inc,v 1.1 2009/01/30 18:34:12 miglius Exp $

/**
 * @file
 * Provides default views on behalf of the signup status log module.
 */

/**
 * Implementation of hook_views_default_views().
 */
function signup_status_log_views_default_views() {
  $view = new view;
  $view->name = 'signup_status_log';
  $view->description = 'Signup status log';
  $view->tag = 'Signup';
  $view->view_php = '';
  $view->base_table = 'signup_status_log';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'sid' => array(
      'id' => 'sid',
      'table' => 'signup_status_log',
      'field' => 'sid',
    ),
    'nid' => array(
      'label' => 'Signup node',
      'required' => 1,
      'id' => 'nid',
      'table' => 'signup_status_log',
      'field' => 'nid',
      'relationship' => 'none',
    ),
    'uid' => array(
      'label' => 'Signup user',
      'required' => 0,
      'id' => 'uid',
      'table' => 'signup_status_log',
      'field' => 'uid',
      'relationship' => 'none',
    ),
    'admin_uid' => array(
      'label' => 'Changed by',
      'required' => 0,
      'id' => 'admin_uid',
      'table' => 'signup_status_log',
      'field' => 'admin_uid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'timestamp' => array(
      'label' => 'Time',
      'date_format' => 'small',
      'custom_date_format' => '',
      'exclude' => 0,
      'id' => 'timestamp',
      'table' => 'signup_status_log',
      'field' => 'timestamp',
      'relationship' => 'none',
    ),
    'title' => array(
      'label' => 'Title',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'nid',
    ),
    'name' => array(
      'label' => 'Name',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'uid',
    ),
    'email' => array(
      'label' => 'E-mail',
      'exclude' => 0,
      'id' => 'email',
      'table' => 'signup_status_log',
      'field' => 'email',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'label' => 'Changed by',
      'link_to_user' => 1,
      'exclude' => 0,
      'id' => 'name_1',
      'table' => 'users',
      'field' => 'name',
      'relationship' => 'admin_uid',
    ),
    'cid' => array(
      'label' => 'Status',
      'exclude' => 0,
      'id' => 'cid',
      'table' => 'signup_status_log',
      'field' => 'cid',
      'relationship' => 'none',
    ),
    'action' => array(
      'label' => 'Action',
      'exclude' => 0,
      'id' => 'action',
      'table' => 'signup_status_log',
      'field' => 'action',
      'relationship' => 'none',
    ),
    'edit_link' => array(
      'label' => 'Edit',
      'text' => 'Edit',
      'exclude' => 0,
      'id' => 'edit_link',
      'table' => 'signup_log',
      'field' => 'edit_link',
      'relationship' => 'sid',
    ),
  ));
  $handler->override_option('filters', array(
    'uid' => array(
      'operator' => 'in',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'uid_op',
        'identifier' => 'uid',
        'label' => 'Name',
        'optional' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'uid',
      'table' => 'users',
      'field' => 'uid',
      'relationship' => 'uid',
    ),
    'title' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'title_op',
        'identifier' => 'title',
        'label' => 'Title',
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'nid',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
  ));
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 1,
    'order' => 'desc',
    'columns' => array(
      'timestamp' => 'timestamp',
      'title' => 'title',
      'name' => 'name',
      'email' => 'email',
      'name_1' => 'name_1',
      'cid' => 'cid',
      'edit_link' => 'edit_link',
      'action' => 'action',
    ),
    'info' => array(
      'timestamp' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'email' => array(
        'separator' => '',
      ),
      'name_1' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'cid' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'edit_link' => array(
        'separator' => '',
      ),
      'action' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'timestamp',
  ));
  $views[$view->name] = $view;

  return $views;
}


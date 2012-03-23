<?php
// $Id: location_phone.views.inc,v 1.1 2008/09/29 20:31:20 bdragon Exp $

/**
 * @file
 * Views 2 support for Location Phone.
 */

/**
 * Implementation of hook_views_data().
 */
function location_phone_views_data() {
  // ----------------------------------------------------------------
  // location_phone table -- basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.

  $data['location_phone']['table']['group'] = t('Location');

  // Location_fax joins only to location.
  $data['location_phone']['table']['join'] = array(
    'location' => array(
      'left_field' => 'lid',
      'field' => 'lid',
    ),
  );

  // ----------------------------------------------------------------
  // location_phone table -- fields

  $data['location_phone']['phone'] = array(
    'title' => t('Phone'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'empty field name' => t('None'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
      'allow empty' => TRUE,
    ),
  );

  return $data;
}

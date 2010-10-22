<?php
// $Id: location.views.inc,v 1.16 2008/12/03 22:51:23 bdragon Exp $

/**
 * @file
 * Views 2 support for Location.
 */

/*

TODO:
 * Finish porting!
 * Write "relationships" -- see book.views.inc, upload.views.inc, nodequeue...


*/

/**
 * Implementation of hook_views_handlers().
 */
function location_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'location') .'/handlers',
    ),
    'handlers' => array(
      'location_views_handler_field_latitude' => array(
        'parent' => 'views_handler_field',
      ),
      'location_views_handler_field_longitude' => array(
        'parent' => 'location_views_handler_field_latitude',
      ),
      'location_views_handler_field_coordinates' => array(
        'parent' => 'location_views_handler_field_latitude',
      ),
      'location_handler_field_location_country' => array(
        'parent' => 'views_handler_field',
      ),
      'location_handler_filter_location_country' => array(
        'parent' => 'views_handler_filter',
      ),
      'location_handler_argument_location_country' => array(
        'parent' => 'views_handler_argument',
      ),
      'location_handler_field_location_province' => array(
        'parent' => 'views_handler_field',
      ),
      'location_handler_filter_location_province' => array(
        'parent' => 'views_handler_filter',
      ),
      'location_handler_argument_location_province' => array(
        'parent' => 'views_handler_argument',
      ),
      'location_handler_field_location_address' => array(
        'parent' => 'views_handler_field',
      ),
      'location_handler_field_location_street' => array(
        'parent' => 'views_handler_field',
      ),
      'location_views_handler_filter_proximity' => array(
        'parent' => 'views_handler_filter',
      ),
      'location_handler_field_location_distance' => array(
        'parent' => 'views_handler_field',
      ),
      'location_handler_sort_location_distance' => array(
        'parent' => 'views_handler_sort',
      ),
//      'location_handler_relationship_location_distance' => array(
//        'parent' => 'views_handler_relationship',
//      ),

//      'location_handler_field_location_coordinates_user' => array(
//        'parent' => 'views_handler_field',
//      ),

    ),
  );
}

/**
 * Implementation of hook_views_data().
 */
function location_views_data() {
  // ----------------------------------------------------------------
  // location table -- basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.

  $data['location']['table']['group'] = t('Location');

  // Advertise this table as a possible base table
  $data['location']['table']['base'] = array(
    'field' => 'lid',
    'title' => t('Location'),
    'help' => t('Locations are addresses and map coordinates.'),
    'weight' => -10,
  );

  $data['location']['table']['join'] = array(
    // Location links to node through location_instance via lid.
    'node' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // Location links to node_revisions through location_instance via lid.
    'node_revisions' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    // Location links to users through location_instance via lid.
    'users' => array(
      'left_table' => 'location_instance',
      'left_field' => 'lid',
      'field' => 'lid',
    ),
  );

  // ----------------------------------------------------------------
  // location table -- fields

  // lid
  $data['location']['lid'] = array(
    'title' => t('Lid'),
    'help' => t('The location ID of the location.'), // The help that appears on the UI,
    // Information for displaying the lid
    'field' => array(
      'handler' => 'views_handler_field', // @@@
      'click sortable' => TRUE,
    ),
    // Information for accepting a lid as an argument
    /*
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title', // the field to display in the summary.
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    */
    // Information for accepting a lid as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
      'allow empty' => TRUE,
    ),
    // Information for sorting on a lid.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['location']['name'] = array(
    'title' => t('Name'),
    'help' => t('The name of the selected location.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // @@@ 1.x Conversion -- 'additional' => 'street', style 'additional'
  $data['location']['street'] = array(
    'title' => t('Street'),
    'help' => t('The street address of the selected location.'),
    'field' => array(
      'handler' => 'location_handler_field_location_street',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['location']['city'] = array(
    'title' => t('City'),
    'help' => t('The city of the selected location.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'empty field name' => t('Unknown'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // @@@ 1.x Conversion -- 'province' => 'province', style 'name'; 'province_code' => 'province', style 'code'
  $data['location']['province'] = array(
    'title' => t('Province'),
    'help' => t('The province of the selected location.'),
    'field' => array(
      'handler' => 'location_handler_field_location_province',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'location_handler_argument_location_province',
      //'name field' => 'name',
    ),
    'filter' => array(
      'handler' => 'location_handler_filter_location_province',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      // TODO: needs handler to sort by name, not code
    ),
  );

  $data['location']['postal_code'] = array(
    'title' => t('Postal Code'),
    'help' => t('The postal code of the selected location.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // @@@ 1.x Conversion -- 'country' => 'country', style 'name'; 'country_code' => 'country', style 'code'.
  $data['location']['country'] = array(
    'title' => t('Country'),
    'help' => t('The country of the selected location.'),
    'field' => array(
      'handler' => 'location_handler_field_location_country',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'location_handler_argument_location_country',
      //'name field' => 'name',
    ),
    'filter' => array(
      'handler' => 'location_handler_filter_location_country',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      // TODO: needs handler to sort by name, not code
    ),
  );

  $data['location']['latitude'] = array(
    'title' => t('Latitude'),
    'help' => t('The latitude of the selected location.'),
    'field' => array(
      'handler' => 'location_views_handler_field_latitude',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['location']['longitude'] = array(
    'title' => t('Longitude'),
    'help' => t('The longitude of the selected location.'),
    'field' => array(
      'handler' => 'location_views_handler_field_longitude',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['location']['coordinates'] = array(
    'title' => t('Coordinates'),
    'help' => t("The coordinates of the selected location in 'lat, long' format."),
    'field' => array(
      'field' => 'latitude', // The handler adds the longitude.
      'handler' => 'location_views_handler_field_coordinates',
      'click sortable' => FALSE,
    ),
  );

  $data['location']['distance'] = array(
    'title' => t('Distance / Proximity'),
    'help' => t("The distance from the selected location and either the current user or a specific location."),
    'field' => array(
      'handler' => 'location_handler_field_location_distance',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'location_handler_sort_location_distance',
    ),
    'filter' => array(
      'handler' => 'location_views_handler_filter_proximity',
    ),
//    'relationship' => array(
//      'handler' => 'location_handler_relationship_location_distance',
//    ),
  );

//  $data['location']['coordinates_user'] = array(
//    'title' => t('Coordinates of logged-on user'),
//    'help' => t('This will contain the coordinates of the logged-on user.'),
//    'field' => array(
//      'handler' => 'location_handler_field_location_coordinates_user',
//      'click sortable' => FALSE,
//    ),
//  );

/*
      'latitude' => array(
        'name' => t('Latitude'),
        'sortable' => TRUE,
      ),
      'longitude' => array(
        'name' => t('Longitude'),
        'sortable' => TRUE,
      ),
*/


  $data['location']['address'] = array(
    'title' => t('Address'),
    'help' => t('The entire address block for the location.'),
    'field' => array(
      'field' => 'lid',
      'handler' => 'location_handler_field_location_address',
    ),
  );


  $data['location_instance']['table']['group']  = t('Location');

  $data['location_instance']['table']['join'] = array(
    'location' => array(
      'left_field' => 'lid',
      'field' => 'lid',
    ),
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
    'node' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );


  // Tell the base tables about us.
  $data['node']['table']['join']['location'] = array(
    'left_table' => 'location_instance',
    'left_field' => 'vid',
    'field' => 'vid',
  );
  $data['node_revisions']['table']['join']['location'] = array(
    'left_table' => 'location_instance',
    'left_field' => 'vid',
    'field' => 'vid',
  );
  $data['users']['table']['join']['location'] = array(
    'left_table' => 'location_instance',
    'left_field' => 'uid',
    'field' => 'uid',
  );

  return $data;
}

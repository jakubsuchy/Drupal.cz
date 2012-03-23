<?php
// $Id: location_search.admin.inc,v 1.3 2008/10/09 22:09:05 bdragon Exp $

/**
 * @file
 * Administration related forms.
 */

/**
 * Admin settings form.
 *
 * @ingroup form
 */
function location_search_admin_settings() {
  $form['location_search_map'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use map to pick search coordinates'),
    '#return_value' => 1,
    '#default_value' => variable_get('location_search_map', 1),
  );
  $form['location_search_map_address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add geocoding address field to search map'),
    '#return_value' => 1,
    '#default_value' => variable_get('location_search_map_address', 1),
  );
  $form['location_search_map_macro'] = array(
    '#type' => 'textfield',
    '#title' => t('Macro for search map'),
    '#default_value' => variable_get('location_search_map_macro', '[gmap |behavior=+collapsehack]'),
  );

  return system_settings_form($form);
}

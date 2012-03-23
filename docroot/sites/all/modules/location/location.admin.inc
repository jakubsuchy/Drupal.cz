<?php
// $Id: location.admin.inc,v 1.17.2.2 2009/03/02 20:42:46 bdragon Exp $

/**
 * @file
 * Admin forms for Location.
 */

/**
 * Admin settings form.
 *
 * @ingroup form
 */
function location_admin_settings() {
  // Recalculate the supported countries.
  cache_clear_all('location:supported-countries', 'cache');
  _location_supported_countries();

  $iso_list_sorted = location_get_iso3166_list();
  array_multisort($iso_list_sorted);
  $iso_list_sorted = array_merge(array('' => ''), $iso_list_sorted);

  $form  = array();
  $form['location_default_country'] = array(
    '#type'           => 'select',
    '#title'          => t('Default country selection'),
    '#default_value'  => variable_get('location_default_country', 'us'),
    '#options'        => $iso_list_sorted,
    '#description'    => t('This will be the country that is automatically selected when a location form is served for a new location.')
  );
  $form['location_display_location'] = array(
    '#type'           => 'radios',
    '#title'          => t('Toggle location display'),
    '#default_value'  => variable_get('location_display_location', 1),
    '#options'        => array(
      0 => t('Disable the display of locations.'),
      1 => t('Enable the display of locations.')
    ),
    '#description'    => t('If you are interested in turning off locations and having a custom theme control their display, you may want to disable the display of locations so your theme can take that function.')
  );

  $form['location_usegmap'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('Use a Google Map to set latitude and longitude '),
    '#default_value'  => variable_get('location_usegmap', FALSE),
    '#description'    => t('If the gmap.module is installed and <a href="@enabled">enabled</a>, and this setting is also turned on, users that are allowed to manually enter latitude/longitude coordinates will be able to do so with an interactive Google Map.  You should also make sure you have entered a <a href="@google_maps_api_key">Google Maps API key</a> into your <a href="@gmap_module_settings">gmap module settings</a>.', array('@enabled' => url('admin/build/modules'), '@google_maps_api_key' => 'http://www.google.com/apis/maps', '@gmap_module_settings' => url('admin/settings/gmap'))),
// @@@ megapatch This is an idea, but I'd opt more for a warning here...
//    '#disabled'       => !module_exists('gmap'),
  );

  $form['location_locpick_macro'] = array(
    '#type' => 'textfield',
    '#title' => t('Location chooser macro'),
    '#size' => 50,
    '#maxlength' => 500,
    '#default_value' => variable_get('location_locpick_macro', '[gmap]'),
    '#description' => t('If you would like to change the macro used to generate the location chooser map, you can do so here. Note: Behaviors <em>locpick</em> and <em>collapsehack</em> are forced to be enabled and cannot be changed.'),
  );

  $form['location_jit_geocoding'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable JIT geocoding'),
    '#default_value' => variable_get('location_jit_geocoding', FALSE),
    '#description' => t('If you are going to be importing locations in bulk directly into the database, you may wish to enable JIT geocoding and load the locations with source set to 4 (LOCATION_LATLON_JIT_GEOCODING). The system will automatically geocode locations as they are loaded.'),
  );

  return system_settings_form($form);
}

/**
 * Settings page for map links.
 */
function location_map_link_options_form() {
  $form = array();

  $form['countries'] = array(
    '#type' => 'markup',
    '#value' => ''
  );

  foreach (_location_supported_countries() as $country_iso => $country_name) {
    location_load_country($country_iso);

    $form['countries'][$country_iso] = array(
      '#type' => 'markup',
      '#value' => ''
    );

    $form['countries'][$country_iso]['label_'. $country_iso] = array(
      '#type' => 'markup',
      '#value' => $country_name
    );

    // Set up '#options' array for mapping providers for the current country
    $mapping_options = array();
    $provider_function = 'location_map_link_'. $country_iso .'_providers';
    $default_provider_function = 'location_map_link_'. $country_iso .'_default_providers';
    $checked = variable_get('location_map_link_'. $country_iso, function_exists($default_provider_function) ? $default_provider_function() : array());
    //print "Calling provider function $provider_function";
    if (function_exists($provider_function)) {
      foreach ($provider_function() as $name => $details) {
        $mapping_options[$name] = '<a href="'. $details['url'] .'">'. $details['name'] .'</a> (<a href="'. $details['tos'] .'">Terms of Use</a>)';
      }
    }

    if (count($mapping_options)) {
      $form['countries'][$country_iso]['location_map_link_'. $country_iso] = array(
        '#title' => '',
        '#type' => 'checkboxes',
        '#default_value' => $checked,
        '#options' => $mapping_options
      );
    }
    else {
      $form['countries'][$country_iso]['location_map_link_'. $country_iso] = array(
        '#type' => 'markup',
        '#value' => t('None supported.')
      );
    }
  }

  $form = system_settings_form($form);
  $form['#theme'] = 'location_map_link_options';
  return $form;
}

function location_geocoding_options_form() {
  $form = array();

  $form['countries'] = array();

  // First, we build two arrays to help us figure out on the fly whether a specific country is covered by a multi-country geocoder,
  // and what the details of the multi-country geocoder are
  // (1) Get list of geocoders
  $general_geocoders_list = location_get_general_geocoder_list();

  // (2) get data about each geocoder and the list of coutnries covered by each geocoder
  $general_geocoders_data = array();
  $general_geocoders_countries = array();
  foreach ($general_geocoders_list as $geocoder_name) {
    location_load_geocoder($geocoder_name);
    $info_function = $geocoder_name .'_geocode_info';
    if (function_exists($info_function)) {
      $general_geocoders_data[$geocoder_name] = $info_function();
    }

    $countries_function = $geocoder_name .'_geocode_country_list';
    if (function_exists($countries_function)) {
      $general_geocoders_countries[$geocoder_name] = $countries_function();
    }
  }

  foreach (_location_supported_countries() as $country_iso => $country_name) {
    location_load_country($country_iso);

    $geocoding_options = array();

    $form['countries'][$country_iso] = array(
      '#type' => 'markup',
      '#value' => ''
    );

    $form['countries'][$country_iso]['label_'. $country_iso] = array(
      '#type' => 'markup',
      '#value' => '<div id="'. $country_iso .'">'. $country_name .'</div>'
    );

    // Next, we look for options presented by country specific providers
    $country_specific_provider_function = 'location_geocode_'. $country_iso .'_providers';
    if (function_exists($country_specific_provider_function)) {
      foreach ($country_specific_provider_function() as $name => $details) {
        $geocoding_options[$name .'|'. $country_iso] = '<a href="'. $details['url'] .'">'. $details['name'] .'</a> (<a href="'. $details['tos'] .'">Terms of Use</a>)';
      }
    }

    foreach ($general_geocoders_list as $geocoder_name) {
      if (in_array($country_iso, $general_geocoders_countries[$geocoder_name])) {
        $geocoding_options[$geocoder_name] = '<a href="'. $general_geocoders_data[$geocoder_name]['url'] .'">'. $general_geocoders_data[$geocoder_name]['name'] .'</a> (<a href="'. $general_geocoders_data[$geocoder_name]['tos'] .'">Terms of Use</a>)';
      }
    }

    if (count($geocoding_options)) {
      $geocoding_options = array_merge(array('none' => t('None')), $geocoding_options);

      $form['countries'][$country_iso]['location_geocode_'. $country_iso] = array(
        '#type' => 'radios',
        '#default_value' => variable_get('location_geocode_'. $country_iso, 'none'),
        '#options' => $geocoding_options
      );
    }
    else {
      $form['countries'][$country_iso]['location_geocode_'. $country_iso] = array(
        '#type' => 'markup',
        '#value' => t('None supported.')
      );
    }

    $current_value = variable_get('location_geocode_'. $country_iso, 'none');
    if ($current_value == 'none') {
      $form['countries'][$country_iso]['location_geocode_config_link_'. $country_iso] = array(
        '#type' => 'markup',
        '#value' => t('No service selected for country.')
      );
    }
    else {
      $current_val_chopped = substr($current_value, 0, strpos($current_value, '|'));
      $geocode_settings_form_function_specific = 'location_geocode_'. $country_iso .'_'. $current_val_chopped .'_settings';
      $geocode_settings_form_function_general = $current_value .'_geocode_settings';

      if (function_exists($geocode_settings_form_function_specific)) {
        $form['countries'][$country_iso]['location_geocode_config_link_'. $country_iso] = array(
          '#type' => 'markup',
          '#value' => l(t('Configure parameters'), 'admin/settings/location/geocoding/'. $country_iso .'/'. $current_val_chopped)
        );
      }
      elseif (function_exists($geocode_settings_form_function_general)) {
        $form['countries'][$country_iso]['location_geocode_config_link_'. $country_iso] = array(
          '#type' => 'markup',
          '#value' => l(t('Configure parameters'), 'admin/settings/location/geocoding/'. $country_iso .'/'. $current_value)
        );
      }
      else {
        $form['countries'][$country_iso]['location_geocode_config_link_'. $country_iso] = array(
          '#type' => 'markup',
          '#value' => t('No configuration necessary for selected service.')
        );
      }
    }
  }
  $form = system_settings_form($form);
  $form['#theme'] = 'location_geocoding_options';
  array_unshift($form['#submit'], 'location_geocoding_options_form_submit');

  return $form;
}

function location_geocoding_options_form_submit($form, &$form_state) {
  $general_geocoders = location_get_general_geocoder_list();
  $general_geocoders_in_use = array();

  foreach ($form_state['values'] as $key => $value) {
    if (substr($key, 0, 17) == 'location_geocode_') {
      if (in_array($value, $general_geocoders)) {
        $general_geocoders_in_use[$value] = $value;
        variable_set($key, $value);
      }
    }
  }

  variable_set('location_general_geocoders_in_use', $general_geocoders_in_use);
}

function theme_location_map_link_options(&$form) {
  $header = array(array('align' => 'center', 'data' => '<center>'. t('Country') .'</center>'), array('align' => 'center', 'data' => '<center>'. t('Options') .'</center>'));
  $rows = array();
  foreach (element_children($form['countries']) as $country_iso) {
    $row = array();
    $row[] = array(
      'data' => drupal_render($form['countries'][$country_iso]['label_'. $country_iso])
    );
    $row[] = array(
      'data' => drupal_render($form['countries'][$country_iso]['location_map_link_'. $country_iso])
    );
    $rows[] = $row;
  }
  $output = theme('table', $header, $rows);
  foreach (element_children($form) as $key) {
    $output .= drupal_render($form[$key]);
  }
  $output .= drupal_render($form);
  return $output;
}

function theme_location_geocoding_options(&$form) {
  $header = array(
    array('align' => 'center', 'data' => '<center>'. t('Country') .'</center>'),
    array('align' => 'center', 'data' => '<center>'. t('Options') .'</center>'),
    array('align' => 'center', 'data' => '<center>'. t('Configure') .'</center>')
  );
  $rows = array();
  foreach (element_children($form['countries']) as $country_iso) {
    $row = array();
    $row[] = array(
      'data' => drupal_render($form['countries'][$country_iso]['label_'. $country_iso])
    );
    $row[] = array(
      'data' => drupal_render($form['countries'][$country_iso]['location_geocode_'. $country_iso])
    );
    $row[] = array(
      'data' => drupal_render($form['countries'][$country_iso]['location_geocode_config_link_'. $country_iso])
    );
    $rows[] = $row;
  }
  $output = theme('table', $header, $rows);
  foreach (element_children($form) as $key) {
    $output .= drupal_render($form[$key]);
  }
  $output .= drupal_render($form);
  return $output;
}

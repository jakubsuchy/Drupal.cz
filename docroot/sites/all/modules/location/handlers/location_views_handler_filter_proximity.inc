<?php
// $Id: location_views_handler_filter_proximity.inc,v 1.3 2008/12/03 22:51:23 bdragon Exp $

/**
 * General proximity filter for location latitude/longitude.
 */
class location_views_handler_filter_proximity extends views_handler_filter {
  // This is always single, because of the distance field's possible dependency
  // on it.
  var $no_single = TRUE;

  function option_definition() {
    $options = parent::option_definition();
    $options['type'] = array('default' => 'latlon');

    $options['operator'] = array('default' => 'mbr');

    $options['identifier'] = array('default' => 'dist');

    $options['value'] = array(
      'default' => array(
        'latitude' => '',
        'longitude' => '',
        'postal_code' => '',
        'country' => '',
        'search_distance' => 100,
        'search_units' => 'mile',
      ),
    );
    return $options;
  }

  function admin_summary() {
    return '';
  }

  function operator_options() {
    return array(
      'mbr' => t('Proximity (Rectangular)'),
      'dist' => t('Proximity (Circular)'),
    );
  }

  function expose_form_left(&$form, &$form_state) {
    parent::expose_form_left($form, $form_state);
    $form['expose']['type'] = array(
      '#parents' => array('options', 'type'),
      '#type' => 'select',
      '#title' => t('Form mode'), // @@@ Less stupid title?
      '#options' => array(
        'latlon' => t('Latitude / Longitude input'),
        'postal' => t('Postal Code / Country'),
        'postal_default' => t('Postal Code (assume default country)'),
      ),
      //'#id' => 'edit-options-type',
      '#description' => t('FIXME'),
      '#default_value' => $this->options['type'],
    );
  }

  function value_form(&$form, &$form_state) {
    $val = $this->value;

    // [11:44] <merlinofchaos> If you load the page from scratch, $input for your identifier will be empty.
    // [11:44] <merlinofchaos> So what's going on here is that for $_GET forms, there's no form id, no op button or anything, so they always appear to submit.
    // [11:45] <merlinofchaos> FAPI doesn't quite get along with that; sometimes it handles the input being empty right and sometimes it doesn't.
    // [11:45] <Bdragon> But if I set #default_value to a static string, it doesn't work either
    // [11:45] <merlinofchaos> Right, fapi thinks the empty input is actually input, thus it overrides the default value.
    // [11:45] <Bdragon> Ahh, hmm...
    // [11:46] <Bdragon> So where would I go to clean it up?
    // [11:55] <merlinofchaos> Bdragon: See views_handler_filter_string.inc line 174
    // [11:55] <merlinofchaos> Bdragon: That's how i address this problem.
    // [11:58] <Bdragon> Hmm, OK
    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];
      if (!isset($form_state['input'][$identifier])) {
        // We need to pretend the user already inputted the defaults, because
        // fapi will malfunction otherwise.
        $form_state['input'][$identifier] = $this->value;
      }
    }

    $form['value'] = array(
      '#tree' => TRUE,
    );

    $form['value']['latitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Latitude'),
      '#default_value' => $this->value['latitude'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-type' => array('latlon')),
    );
    $form['value']['longitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Longitude'),
      '#default_value' => $this->value['longitude'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-type' => array('latlon')),
    );

    $form['value']['postal_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Postal code'),
      '#default_value' => $this->value['postal_code'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-type' => array('postal', 'postal_default')),
    );

    $form['value']['country'] = array(
      '#type' => 'select',
      '#title' => t('Country'),
      '#options' => array('' => '') + location_get_iso3166_list(),
      '#default_value' => $this->value['country'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-type' => array('postal')),
    );

    $form['value']['search_distance'] = array(
      '#type' => 'textfield',
      '#title' => t('Distance'),
      '#default_value' => $this->value['search_distance'],
    );

    $form['value']['search_units'] = array(
      '#type' => 'select',
      '#options' => array(
        'mile' => t('Miles'),
        'km' => t('Kilometers'),
      ),
      '#default_value' => $this->value['search_units'],
    );
  }

  function exposed_form(&$form, &$form_state) {
    parent::exposed_form($form, $form_state);
    $key = $this->options['expose']['identifier'];
    $type = $this->options['type'];

    // Remove unneeded fields when exposing the form.
    // It's shorter than redefining value_form.
    if ($type != 'latlon') {
      unset($form[$key]['latitude']);
      unset($form[$key]['longitude']);
    }
    if ($type != 'postal' && $type != 'postal_default') {
      unset($form[$key]['postal_code']);
    }
    if ($type != 'postal') {
      unset($form[$key]['country']);
    }
  }

  // Used from the distance field.
  function calculate_coords() {
    if (!empty($this->value['latitude']) && !empty($this->value['longitude'])) {
      // If there are already coordinates, there's no work for us.
      return TRUE;
    }
    // @@@ Switch to mock location object and rely on location more?

    if ($this->options['type'] == 'postal' || $this->options['type'] == 'postal_default') {
      // Force default for country.
      if ($this->options['type'] == 'postal_default') {
        $this->value['country'] = variable_get('location_default_country', 'us');
      }

      // Zip code lookup.
      if (!empty($this->value['postal_code']) && !empty($this->value['country'])) {
        $coord = location_latlon_rough($this->value);
        if ($coord) {
          $this->value['latitude'] = $coord['lat'];
          $this->value['longitude'] = $coord['lon'];
        }
        else {
          return false;
        }
      }
      else {
        // @@@ Implement full address lookup?
        return false;
      }
    }
    if (empty($this->value['latitude']) || empty($this->value['longitude'])) {
      return false;
    }
    return true;
  }

  function query() {
    if (empty($this->value)) {
      return;
    }

    // Coordinates available?
    if (!$this->calculate_coords()) {
      // Distance set?
      if (!empty($this->value['search_distance'])) {
        // Hmm, distance set but unable to resolve coordinates.
        // Force nothing to match.
        $this->query->add_where($this->options['group'], "0");
      }
      return;
    }

    $this->ensure_my_table();

    $lat = $this->value['latitude'];
    $lon = $this->value['longitude'];

    $distance_meters = _location_convert_distance_to_meters($this->value['search_distance'], $this->value['search_units']);

    $latrange = earth_latitude_range($lon, $lat, $distance_meters);
    $lonrange = earth_longitude_range($lon, $lat, $distance_meters);

    // Add MBR check (always.)
    $this->query->add_where($this->options['group'], "$this->table_alias.latitude > %f AND $this->table_alias.latitude < %f AND $this->table_alias.longitude > %f AND $this->table_alias.longitude < %f", $latrange[0], $latrange[1], $lonrange[0], $lonrange[1]);

    if ($this->operator == 'dist') {
      // Add radius check.
      $this->query->add_where($this->options['group'], earth_distance_sql($lon, $lat, $this->table_alias) .' < %f', $distance_meters);
    }
  }
}

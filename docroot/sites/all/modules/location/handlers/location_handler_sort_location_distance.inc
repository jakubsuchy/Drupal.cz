<?php
// $Id: location_handler_sort_location_distance.inc,v 1.1 2008/12/03 22:51:23 bdragon Exp $

/**
 * @file
 * Coordinates sort handler.
 */

class location_handler_sort_location_distance extends views_handler_sort {

  function option_definition() {
    $options = parent::option_definition();
    $options['origin'] = array('default' => 'user');
    $options['units'] = array('default' => 'km');
    $options['latitude'] = array('default' => '');
    $options['longitude'] = array('default' => '');
    return $options;
  }

  function has_extra_options() {
    return TRUE;
  }

  function extra_options_form(&$form, &$form_state) {
    $form['units'] = array(
      '#type' => 'radios',
      '#title' => t('Units'),
      '#options' => array(
        'km' => t('Kilometers'),
        'mi' => t('Miles'),
      ),
      '#description' => t('FIXME'),
      '#default_value' => $this->options['units'],
    );
    $form['origin'] = array(
      '#type' => 'radios',
      '#title' => t('Origin'),
      '#options' => array(
        'user' => t("User's location (blank if unset)"),
        'hybrid' => t("User's location (fall back to static if unset)"),
        'static' => t("Static location"),
        'tied' => t("Use Distance / Proximity filter"),
      ),
      '#description' => t('FIXME'),
      '#default_value' => $this->options['origin'],
    );
    $form['latitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Latitude'),
      '#default_value' => $this->options['latitude'],
    );
    $form['longitude'] = array(
      '#type' => 'textfield',
      '#title' => t('Longitude'),
      '#default_value' => $this->options['longitude'],
    );
  }

  function query() {
    $latitude = 0;
    $longitude = 0;

    if ($this->options['origin'] == 'hybrid' || $this->options['origin'] == 'user') {
      global $user;
      if (!isset($user->locations)) {
        $user = user_load($user->uid);
      }
      if (isset($user->locations[0]['latitude']) || !empty($user->locations[0]['latitude'])) {
        $latitude = (float)$user->locations[0]['latitude'];
        $longitude = (float)$user->locations[0]['longitude'];
      }
    }

    if ($this->options['origin'] == 'static') {
      $latitude = (float)$this->options['latitude'];
      $longitude = (float)$this->options['longitude'];
    }

    if ($this->options['origin'] == 'tied') {
      if (!empty($this->view->filter)) {
        foreach ($this->view->filter as $k => $v) {
          if ($v->table == 'location' && $v->field == 'distance' && $v->options['relationship'] == $this->options['relationship']) {
            if ($v->calculate_coords()) {
              $latitude = (float)$v->value['latitude'];
              $longitude = (float)$v->value['longitude'];
            }
          }
        }
      }
    }

    $this->ensure_my_table();

    // OK, so this part will need a little explanation.
    // Since the distance calculation is so icky, we try quite hard
    // to save some work for the database.
    // If someone has added a field that matches the sort, we just sort on that column!
    $alias = $this->table_alias .'_'. $this->field .'_sort';
    foreach ($this->view->field as $k => $v) {
      if ($v->table == 'location' && $v->field == 'distance' && $v->options['relationship'] == $this->options['relationship']) {
        if ($v->options['origin'] == $this->options['origin']
            && $v->options['units'] == $this->options['units']
            && $v->options['latitude'] == $this->options['latitude']
            && $v->options['longitude'] == $this->options['longitude']) {
          // We have a match! Sync aliases to make it easier on the database.
          $alias = $v->field_alias;
        }
      }
    }

    if (empty($latitude) && empty($longitude)) {
      // We don't know the distance.
      // Therefore, we don't need to sort on it.
    }
    else {
      // This is done exactly the same as the field version.
      // Views is ok with us redefining the formula for a field.
      // If ANYTHING differs in the configuration, we will use a new alias.
      $this->query->add_orderby(NULL, earth_distance_sql($longitude, $latitude, $this->table_alias), $this->options['order'], $alias);
    }
  }
}

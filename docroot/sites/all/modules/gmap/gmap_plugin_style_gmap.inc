<?php
// $Id: gmap_plugin_style_gmap.inc,v 1.7 2009/02/05 21:51:53 bdragon Exp $

/**
 * @file
 * GMap style plugin.
 */

/**
 * Style plugin to render a map.
 *
 * @ingroup views_style_plugins
 */
class gmap_plugin_style_gmap extends views_plugin_style {
  /**
   * Set default options
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['macro'] = array(
      'default' => '[gmap ]',
    );

    $options['datasource'] = array(
      'default' => 'location',
    );

    $options['markers'] = array('default' => 'static');
    $options['markertype'] = array('default' => 'drupal');

    $options['latfield'] = array('default' => '');
    $options['lonfield'] = array('default' => '');
    $options['markerfield'] = array('default' => '');

    return $options;
  }

  function query() {
    if ($this->options['datasource'] == 'location') {
      $table = $this->view->query->ensure_table('location');
      $this->view->query->add_field($table, 'latitude', 'gmap_lat');
      $this->view->query->add_field($table, 'longitude', 'gmap_lon');
    }

    if ($this->options['markers'] == 'nodetype') {
      $this->view->query->add_field('node', 'type', 'gmap_node_type');
    }
    else if ($this->options['markers'] == 'taxonomy') {
      $this->view->query->add_field('gmap_taxonomy_node', 'marker', 'gmap_node_marker');
    }
    else if ($this->options['markers'] == 'userrole') {
      $this->view->query->add_field('users_roles', 'rid', 'gmap_role_marker');
    }
  }

  function render() {
    if (empty($this->row_plugin)) {
      vpr('gmap_plugin_style_gmap: Missing row plugin');
      return;
    }

    $lat_field = 'gmap_lat';
    $lon_field = 'gmap_lon';

    // Determine fieldname for latitude and longitude fields.
    if ($this->options['datasource'] == 'fields') {
      $lat_field = $this->view->display_handler->get_handler('field', $this->options['latfield'])->field_alias;
      $lon_field = $this->view->display_handler->get_handler('field', $this->options['lonfield'])->field_alias;
    }

    // Determine fieldname for marker field.
    if ($this->options['markers'] == 'field') {
      $marker_field = $this->view->display_handler->get_handler('field', $this->options['markerfield'])->field_alias;
    }

    $markername = isset($this->options['markertype']) ? $this->options['markertype'] : 'drupal';

    $markertypes = variable_get('gmap_node_markers', array());
    if ($this->options['markers'] == 'nodetype') {
      $markertypes = variable_get('gmap_node_markers', array());
    }
    else if ($this->options['markers'] == 'userrole') {
      $markertypes = variable_get('gmap_role_markers', array(DRUPAL_AUTHENTICATED_RID => 'drupal'));
    }

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);

    // Render each group separately and concatenate.  Plugins may override this
    // method if they wish some other way of handling grouping.
    $output = '';
    foreach ($sets as $title => $records) {
      $markers = array();
      $offsets = array();
        foreach ($records as $label => $row) {
        $lat = (float)$row->{$lat_field};
        $lon = (float)$row->{$lon_field};
        if (!empty($lat) && !empty($lon)) {
          if ($this->options['markers'] == 'nodetype') {
            if (isset($markertypes[$row->gmap_node_type])) {
              $markername = $markertypes[$row->gmap_node_type];
            }
          }
          else if ($this->options['markers'] == 'taxonomy') {
            if (!empty($row->gmap_node_marker)) {
              $markername = $row->gmap_node_marker;
            }
          }
          else if ($this->options['markers'] == 'userrole') {
            if (!empty($row->gmap_role_marker)) {
              $markername = $markertypes[DRUPAL_AUTHENTICATED_RID];
              if (isset($markertypes[$row->gmap_role_marker])) {
                $markername = $markertypes[$row->gmap_role_marker];
              }
            }
          }
          else if ($this->options['markers'] == 'field') {
            if (!empty($row->{$marker_field})) {
              $markername = $row->{$marker_field};
            }
          }
          if (!isset($offsets[$markername])) {
            $offsets[$markername] = 0;
          }
          $markers[] = array(
            'latitude' => $lat,
            'longitude' => $lon,
            'markername' => $markername,
            'offset' => $offsets[$markername],
            'text' => $this->row_plugin->render($row),
          );
          $offsets[$markername]++;
        }
      }
      if (!empty($markers)) { // Don't draw empty maps.
        $map = gmap_parse_macro($this->options['macro']);
        $map['markers'] = $markers;
        $output .= theme($this->theme_functions(), $this->view, $this->options, $map, $title);
      }
    }
    return $output;
  }

  /**
   * Render the given style.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['macro'] = array(
      '#type' => 'textfield',
      '#title' => t('Macro'),
      '#size' => 1000,
      '#default_value' => $this->options['macro'],
    );

    $form['datasource'] = array(
      '#type' => 'select',
      '#title' => t('Data Source'),
      '#options' => array(
        'location' => t('Location.module'),
        'fields' => t('Choose latitude and longitude fields'),
      //'geocode' => t('Just-in-time geocoding on field named "address"'),
      ),
      '#default_value' => $this->options['datasource'],
      '#multiple' => FALSE,
    );

    $options = array();
    // @@@ Fix this when I'm not having a monday morning.
    // There's likely a more "correct" way.
    foreach ($this->display->display_options['fields'] as $id => $handler) {
      $data = views_fetch_data($handler['table']);
      $options[$id] = $handler['label'];
    }

    $form['latfield'] = array(
      '#title' => t('Latitude field'),
      '#description' => t('Format must be degrees decimal.'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $this->options['latfield'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-style-options-datasource' => array('fields')),
    );
    $form['lonfield'] = array(
      '#title' => t('Longitude field'),
      '#description' => t('Format must be degrees decimal.'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $this->options['lonfield'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-style-options-datasource' => array('fields')),
    );

    $form['markers'] = array(
      '#type' => 'select',
      '#title' => t('Marker handling'),
      // @@@ Detect view type automatically?
      '#options' => array(
        'nodetype' => t('By content type (for node views)'),
        'taxonomy' => t('By term (for node views)'),
        'userrole' => t('By user role (for user views)'),
        'field' => t('Use marker field'),
        'static' => t('Use single marker type'),
      ),
      '#default_value' => $this->options['markers'],
    );

    $form['markerfield'] = array(
      '#type' => 'select',
      '#title' => t('Marker field'),
      '#description' => t('You can use a views field to set the <em>markername</em> property of the markers.'),
      '#options' => $options,
      '#default_value' => $this->options['markerfield'],
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-style-options-markers' => array('field')),
    );

    // Hide the taxonomy handling if gmap_taxonomy.module isn't installed.
    if (!module_exists('gmap_taxonomy')) {
      unset($form['markers']['#options']['taxonomy']);
    }

    $form['markertype'] = array(
      '#type' => 'gmap_markerchooser',
      '#title' => t('Marker / fallback marker to use'),
      '#default_value' => $this->options['markertype'],
    );
  }
}

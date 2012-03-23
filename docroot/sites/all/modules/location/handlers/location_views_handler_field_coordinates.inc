<?php
// $Id: location_views_handler_field_coordinates.inc,v 1.1 2008/09/10 22:01:05 bdragon Exp $

/**
 * @file
 * Coordinates field handler.
 */

class location_views_handler_field_coordinates extends location_views_handler_field_latitude {

  function construct() {
    parent::construct();
    $this->additional_fields['longitude'] = 'longitude';
  }

  function render($values) {
    if ($this->options['style'] == 'dms') {
      return theme('location_latitude_dms', $values->{$this->field_alias}) .', '. theme('location_longitude_dms', $values->{$this->aliases['longitude']});
    }
    else {
      return check_plain($values->{$this->field_alias}) .', '. check_plain($values->{$this->aliases['longitude']});
    }
  }

}

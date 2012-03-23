<?php
// $Id: location_views_handler_field_longitude.inc,v 1.1 2008/09/10 22:01:05 bdragon Exp $

/**
 * @file
 * Longitude field handler.
 */

class location_views_handler_field_longitude extends location_views_handler_field_latitude {

  function render($values) {
    if ($this->options['style'] == 'dms') {
      return theme('location_longitude_dms', $values->{$this->field_alias});
    }
    return parent::render($values);
  }
}

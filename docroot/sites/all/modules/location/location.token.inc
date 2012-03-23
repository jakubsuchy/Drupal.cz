<?php
// $Id: location.token.inc,v 1.2 2008/10/07 22:07:05 bdragon Exp $

/**
 * @file
 * Short description.
 */


/**
 * Implementation of hook_token_values().
 */
function _location_token_values($type, $object = NULL) {
  if ($type == 'node' || $type == 'user') {
    if (!empty($object->locations)) {
      // @@@ Strip virtual fields?
      foreach ($object->locations as $key => $location) {
        foreach ($location as $field => $value) {
          if (!is_array($value)) {
            $values["location-{$field}_{$key}"] = check_plain($value);
          }
        }
        // Backwards compatibility.
        $values["location-provincename_$key"] = $values["location-province_name_$key"];
        $values["location-countryname_$key"] = $values["location-country_name_$key"];
      }
      return $values;
    }
  }
}

/**
 * Implementation of hook_token_list().
 */
function _location_token_list($type = 'all') {
  if ($type == 'node' || $type == 'user' || $type == 'all') {
    $tokens['location']['location-name_N']            = t('Location Name (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-street_N']          = t('Street (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-additional_N']      = t('Additional (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-city_N']            = t('City (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-province_N']        = t('State/Province (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-province_name_N']   = t('State/Province Name (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-postal_code_N']     = t('Postal Code (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-latitude_N']        = t('Latitude (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-longitude_N']       = t('Longitude (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-country_N']         = t('Country (If there are multiple locations per node, N is the iteration, starting with 0)');
    $tokens['location']['location-country_name_N']    = t('Country Name (If there are multiple locations per node, N is the iteration, starting with 0)');
    return $tokens;
  }
}

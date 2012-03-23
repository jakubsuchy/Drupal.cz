<?php
// $Id: location.no.inc,v 1.8 2008/01/24 09:00:03 bdragon Exp $

// Norway

function location_province_list_no() {
  return array('AK' => "Akershus",
    'AA' => "Aust-Agder",
    'BU' => "Buskerud",
    'FM' => "Finnmark",
    'HM' => "Hedmark",
    'HL' => "Hordaland",
    'MR' => "More og Romdal",
    'NL' => "Nordland",
    'NT' => "Nord-Trondelag",
    'OP' => "Oppland",
    'OL' => "Oslo",
    'OF' => "Ostfold",
    'RL' => "Rogaland",
    'SJ' => "Sogn og Fjordane",
    'ST' => "Sor-Trondelag",
    'TM' => "Telemark",
    'TR' => "Troms",
    'VA' => "Vest-Agder",
    'VF' => "Vestfold");
}

function _location_latlon_rough_no($location = array()) {
  if (!isset($location['postal_code'])) {
    return NULL;
  }

  $result = db_query("SELECT latitude, longitude FROM {zipcodes} WHERE country = '%s' AND zip = '%s'", $location['country'], substr(str_pad($location['postal_code'], 5, '0', STR_PAD_LEFT), 0, 5));

  if ($row = db_fetch_object($result)) {
    return array('lat' => $row->latitude, 'lon' => $row->longitude);
  }
  else {
    return NULL;
  }
}

/**
 * Returns a lat/lon pair of the approximate center of the given postal code in the given country
 *
 * @param $location
 *   An associative array $location where only postal code and country are necessary, but can have the keys:
 *     'street'       => the street portion of the location
 *     'supplemental' => additional street portion of the location
 *     'province'     => the province, state, or territory
 *     'country'      => lower-cased two-letter ISO code (REQUIRED)
 *     'postal_code'  => the international postal code for this location (REQUIRED)
 *
 * @return
 *   An associative array where
 *      'lat' => approximate latitude of the center of the postal code's area
 *      'lon' => approximate longitude of the center of the postal code's area
 *      'city'     => the city
 *      'province'     => the province, state, or territory
 *      'country'      => lower-cased two-letter ISO code
 *
 */
function _location_latlon_postalcode_no($location = array()) {
  // Now we pad the thing and query.
  #$res = db_query("SELECT * FROM {zipcodes} where country = '%s' AND zip = '%s'", $location['country'], str_pad($location['postal_code'], 4, "0", STR_PAD_LEFT));
  $res = db_query("SELECT * FROM {zipcodes} where country = '%s' AND zip = '%s'", $location['country'], $location['postal_code']);
  if ($row = db_fetch_object($res)) {
    return array('lat' => $row->latitude, 'lon' => $row->longitude, 'city' => $row->city, 'province' => $row->state, 'country' => $row->country);
  }
  else {
    return NULL;
  }
}

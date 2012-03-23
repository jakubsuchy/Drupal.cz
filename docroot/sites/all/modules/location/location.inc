<?php
// $Id: location.inc,v 1.95.2.1 2009/01/09 17:34:39 bdragon Exp $

/**
 * @defgroup Location Location: An API for working with locations.
 *
 * Public API for the Location module.
 */


/**
 * @file
 * An implementation of a universal API for location manipulation.  Provides functions for
 * postal_code proximity searching, deep-linking into online mapping services.  Currently,
 * some options are configured through an interface provided by location.module.
 */

include_once drupal_get_path('module', 'location') .'/earth.inc';

/**
 * Get a deep-link to a mapping service such as Yahoo! Maps or MapPoint given an location.  The
 * call is delegated based on the 'country' value in the $location parameter.
 *
 * @param $location
 *   An associative array where
 *      'street'       => A string representing the street location
 *      'additional'   => A string for any additional portion of the street location
 *      'city'         => A string for the city name
 *      'province'     => The standard postal abbreviation for the province
 *      'country'      => The two-letter ISO code for the country of the location (REQUIRED)
 *      'postal_code'  => The international postal code for the location
 *
 * @return
 *   A link to a map provided by a third-party.  The idea is to encode the appropriate
 *   parameters as HTTP GET variables to the URL.
 *
 * @ingroup Location
 */
function location_map_link($location = array(), $link_text = 'See map: ') {
  if (!isset($location['country']) && $location['country'] != 'xx') {
    return '';
  }

  location_load_country($location['country']);

  $default_func = 'location_map_link_'. $location['country'] .'_default_providers';
  $providers_func = 'location_map_link_'. $location['country'] .'_providers';
  $providers = function_exists($providers_func) ? $providers_func() : array();
  $selected_providers = variable_get('location_map_link_'. $location['country'], function_exists($default_func) ? $default_func() : array());

  $links = array();
  foreach ($selected_providers as $mapper) {
    $link_func = 'location_map_link_'. $location['country'] .'_'. $mapper;
    if (function_exists($link_func)) {
      if ($link = $link_func($location)) {
        $links[] = '<a href="'. $link .'">'. $providers[$mapper]['name'] .'</a>';
      }
    }
  }
  if (count($links)) {
    return t($link_text) . implode($links, ", ");
  }
  else {
    return NULL;
  }
}

/**
 * Try to extract the the Latitude and Longitude data from the
 * postal code.
 *
 * @param $location
 *   Array. the location data
 *   -> the values are:
 *     'street'         => the string representing the street location (REQUIRED)
 *     'additional'     => the string representing the additional street location portion in the location form
 *     'city'           => the city name (REQUIRED)
 *     'province'       => the province code defined in the country-specific include file
 *     'country'        => the lower-case of the two-letter ISO code (REQUIRED)
 *     'postal_code'    => the postal-code (REQUIRED)
 *
 * @return
 *   Array or NULL. NULL if the delegated-to function that does the
 *   actual look-up does not exist. If the appropriate function exists,
 *   then this function returns an associative array where
 *    'lon' => A floating point number for the longitude coordinate of the parameter location
 *    'lat' => A floating point number for the latitude coordinate of the parameter location
 *
 * @ingroup Location
 */
function location_get_postalcode_data($location = array()) {
  $location['country'] = isset($location['country']) ? trim($location['country']) : NULL;
  $location['postal_code'] = isset($location['postal_code']) ? trim($location['postal_code']) : NULL;
  if (is_null($location['postal_code']) || is_null($location['country']) || empty($location['country']) || empty($location['postal_code']) || $location['postal_code'] == 'xx') {
    return NULL;
  }
  $country_specific_function = 'location_get_postalcode_data_'. $location['country'];
  if (function_exists($country_specific_function)) {
    return $country_specific_function($location);
  }
  else {
    return NULL;
  }
}

/**
 * Given two points in lat/lon form, returns the distance between them.
 *
 * @param $latlon_a
 *   An associative array where
 *      'lon' => is a floating point of the longitude coordinate for the point given by latlonA
 *      'lat' => is a floating point of the latitude coordinate for the point given by latlonB
 *
 * @param $latlon_b
 *      Another point formatted like $latlon_b
 *
 * @param $distance_unit
 *      A string that is either 'km' or 'mile'.
 *      If neither 'km' or 'mile' is passed, the parameter is forced to 'km'
 *
 * @return
 *    NULL if sense can't be made of the parameters.
 *    An associative array where
 *      'scalar' => Is the distance between the two lat/lon parameter points
 *      'distance_unit' => Is the unit of distance being represented by 'scalar'.
 *                         This will be 'km' unless 'mile' is passed for the $distance_unit param
 *
 * @ingroup Location
 */
function location_distance_between($latlon_a = array(), $latlon_b = array(), $distance_unit = 'km') {
  if (!isset($latlon_a['lon']) || !isset($latlon_a['lat']) || !isset($latlon_b['lon']) || !isset($latlon_b['lat'])) {
    return NULL;
  }

  if ($distance_unit != 'km' && $distance_unit != 'mile') {
    return NULL;
  }

  // $conversion_factor = number to divide by to convert meters to $distance_unit
  // At this point, $distance_unit == 'km' or 'mile' and nothing else
  //$conversion_factor = ($distance_unit == 'km') ? 1000.0 : 1609.347;

  $meters = earth_distance($latlon_a['lon'], $latlon_a['lat'], $latlon_b['lon'], $latlon_b['lat']);
  return array('scalar' => round($meters/(($distance_unit == 'km') ? 1000.0 : 1609.347), 1), 'distance_unit' => $distance_unit);
}

/**
 * Takes two locations and tries to return a deep-link to driving directions.
 *
 * Parameters:
 * @param $location_a
 *   An associative array that represents an location where
 *      'street'       => the street portions of the location
 *      'additional'   => additional street portion of the location
 *      'city'         => the city name
 *      'province'     => the province, state, or territory
 *      'country'      => lower-cased two-letter ISO code (REQUIRED)
 *      'postal_code'  => the postal code
 *
 * @param $location_b
 *   An associative array that represents an location in the same way that
 *   parameter $location_a does.
 *
 * @param $link_text
 *   The text of the HTML link that is to be generated.
 *
 * @return
 *   A deep-link to driving directions on Yahoo! or some other mapping service, if enough fields are filled in the parameters.
 *   A deep-link to a form for driving directions with information pre-filled if not enough, but some fields are filled in the parameters.
 *   The empty string if no information is provided (or if so little information is provided that there is no function to which to delegate
 *   the call.
 *
 *   We dispatch the call to a country-specific function.  The country-specific function, in this case,
 *   will be the one reflected by the country parameter of the first function.  We require that
 *   both locationes supplied have a country field at the minimum.
 *
 *   The country-specific functions will ultimately decide, with the parameters given, whether to
 *   to link to a form for driving directions is provided, where this form will be
 *   pre-populated with whatever values were available or whether to link directly to the driving
 *   directions themselves if enough fields are filled for each location.
 *
 * @ingroup Location
 */
function location_driving_directions_link($location_a = array(), $location_b = array(), $link_text = 'Get directions') {
  if (!isset($location_a['country']) or !isset($location_b['country'])) {
    return '';
  }

  // For now, return empty string if starting-point and destinations are in different countries
  //if ($location_a['country'] != $location_b['country']) {
  //  return '';
  //}
  // Lines above commented out because I want to let the country-specific function of the departure point decide
  // what it will do with driving destination locationes from other countries.  As an example, Yahoo! Maps supports driving
  // direction queries for locations between the U.S. and Canada.

  $driving_direction_function = 'location_driving_directions_link_'. $location_a['country'];
  if (function_exists($driving_direction_function)) {
    $http_link = $driving_direction_function($location_a, $location_b);
    if (strlen($http_link)) {
      return '<a href="'. $http_link .'">'. $link_text .'</a>';
    }
    else {
      return '';
    }
  }

  return '';
}

/**
 * @param $distance
 *   A number in either miles or km.
 *
 * @param $distance_unit
 *   String (optional). Either 'mile' or 'km' (default)
 *
 * @return
 *   A floating point number where the number in meters after the initially passed scalar has been ceil()'d
 *   This is done after the $distance_unit parmeter is forced to be 'km' or 'mile'
 */
function _location_convert_distance_to_meters($distance, $distance_unit = 'km') {
  if (!is_numeric($distance)) {
    return NULL;
  }

  // Force an integer version of distance, just in case anyone wants to add a caching mechanism
  // for postal code proximity searches.

  if (is_float($distance)) {
    $distance = intval(ceil($distance));
  }

  if ($distance < 1) {
    return NULL;
  }

  if ($distance_unit != 'km' && $distance_unit != 'mile') {
    $distance_unit = 'km';
  }

  // Convert distance to meters
  //$distance_float = floatval($distance) * (($distance_unit == 'km') ? 1000.0 : 1609.347);
  //return round($distance_float, 2);
  $retval = round(floatval($distance) * (($distance_unit == 'km') ? 1000.0 : 1609.347), 2);
  return $retval;
}

/**
 * Takes an location and returns a "rough" latitude/longitude pair based on the postal code
 * data available for the given country.
 *
 * @param $location
 *   An associative array $location where
 *     'street'       => the street portion of the location
 *     'additional' => additional street portion of the location
 *     'province'     => the province, state, or territory
 *     'country'      => lower-cased two-letter ISO code (REQUIRED)
 *     'postal_code'  => international postal code (REQUIRED)
 *
 * @return
 *   NULL if data cannont be found.
 *   Otherwise, an associative array where
 *     'lat' => is a floating point of the latitude coordinate of this location
 *     'lon' => is a floating point of the longitude coordinate of this location
 *
 * @ingroup Location
 */
function location_latlon_rough($location = array()) {
  if (!isset($location['country']) || !isset($location['postal_code'])) {
    return NULL;
  }

  location_load_country($location['country']);

  $latlon_function = 'location_latlon_rough_'. $location['country'];
  if (function_exists($latlon_function)) {
    return $latlon_function($location);
  }
  else {
    return NULL;
  }
}

/**
 * Currently, this is not a priority until there is an implementable use for exact longitude,
 * latitude coordinates for an location.  The idea is that this call will eventually retrieve
 * information through a web-service.  Whereas location_latlon_rough() returns an approximate
 * lat/lon pair based strictly on the postal code where this lat/lon pair is pulled from a
 * database table, this function is intended to send the entire location to a web-service and
 * to retrieve exact lat/lon coordinates.
 *
 * @param $location
 *   An array where
 *     -> the key values are 'street', 'additional', 'province', 'country', 'postal_code'
 *     -> the values are:
 *         'street'         => the string representing the street location (REQUIRED)
 *         'additional'     => the string representing the additional street location portion in the location form
 *         'city'           => the city name (REQUIRED)
 *         'province'       => the province code defined in the country-specific include file
 *         'country'        => the lower-case of the two-letter ISO code (REQUIRED)
 *         'postal_code'    => the postal-code (REQUIRED)
 *
 * @return
 *   NULL if the delegated-to function that does the actual look-up does not exist.
 *   If the appropriate function exists, then this function returns an associative array where
 *      'lon' => A floating point number for the longitude coordinate of the parameter location
 *      'lat' => A floating point number for the latitude coordinate of the parameter location
 *
 * @ingroup Location
 */
function location_latlon_exact($location = array()) {
  $country = $location['country'];
  location_standardize_country_code($country);
  $service = variable_get('location_geocode_'. $country, 'none');
  if (!empty($country) && $service != 'none') {
    // figure out what the exact function should be
    if (strpos($service, '|')) {
      location_load_country($country);
      // The code change below fixes the problem of the country specific
      // function for geocoding not being correctly called (it removes any
      // text from the pipe (|) onwards)
      $exact_latlon_function = 'location_geocode_'. $country .'_'. substr($service, 0, strpos($service, '|'));
    }
    else {
      location_load_geocoder($service);
      $exact_latlon_function = $service .'_geocode_location';
    }
    if (function_exists($exact_latlon_function)) {
      return $exact_latlon_function($location);
    }
    else {
      return NULL;
    }
  }
  return NULL;
}

/**
 * Returns an associative array of countries currently supported
 * by the location system where
 *   -> the keys represent the two-letter ISO code and
 *   -> the values represent the English name of the country.
 * The array is sorted the index (i.e., by the short English name of the country).
 *
 * Please note the different between "supported" countries and "configured"
 * countries: A country being "supported" means that there is an include file
 * to support the country while "configure" implies that the site admin has
 * configured the site to actually use that country.
 *
 * @ingroup Location
 */
function _location_supported_countries() {
  static $supported_countries = array();

  // If this function has already been called this request, we can avoid a DB hit.
  if (!empty($supported_countries)) {
    return $supported_countries;
  }

  // Try first to load from cache, it's much faster than the scan below.
  if ($cache = cache_get('location:supported-countries', 'cache_location')) {
    $supported_countries = $cache->data;
  }
  else {
    // '<ISO two-letter code>' => '<English name for country>'
    $iso_list = location_get_iso3166_list();
    $path = drupal_get_path('module', 'location') .'/supported/location.';
    foreach ($iso_list as $cc => $name) {
      if (file_exists($path . $cc .'.inc')) {
        $supported_countries[$cc] = $name;
      }
    }
    cache_set('location:supported-countries', $supported_countries, 'cache_location');
  }
  return $supported_countries;
}

// @@@ New in 3.x, document.
/**
 * Fetch the provinces for a country.
 */
function location_get_provinces($country = 'us') {
  static $provinces = array();
  location_standardize_country_code($country);
  if (isset($provinces[$country])) {
    return $provinces[$country];
  }
  if ($cache = cache_get("provinces:$country", 'cache_location')) {
    $provinces[$country] = $cache->data;
    return $provinces[$country];
  }
  location_load_country($country);
  $func = 'location_province_list_'. $country;
  if (function_exists($func)) {
    $provinces[$country] = $func();
    cache_set("provinces:$country", $provinces[$country], 'cache_location');
    return $provinces[$country];
  }
  return array();
}

// @@@ New in 3.x, document.
/**
 * Get the translated name of a country code.
 */
function location_country_name($country = 'us') {
  location_standardize_country_code($country);
  $countries = location_get_iso3166_list();
  if (isset($countries[$country])) {
    return $countries[$country];
  }
  else {
    return '';
  }
}

// @@@ New in 3.x, document.
/**
 * Get the full name of a province code.
 */
function location_province_name($country = 'us', $province = 'xx') {
  $provinces = location_get_provinces($country);
  $province = strtoupper($province);
  if (isset($provinces[$province])) {
    return $provinces[$province];
  }
  else {
    return '';
  }
}

// @@@ New in 3.x, document.
/**
 * Get a province code from a code or full name and a country.
 */
function location_province_code($country = 'us', $province = 'xx') {
  // An array of countries is useful if someone specified multiple countries
  // in an autoselect for example.
  // It *is* possibly ambiguous, especially if the province was already a code.
  // We make an array here for single (the usual case) for code simplicity reasons.
  if (!is_array($country)) {
    $country = array($country);
  }

  $p = strtoupper($province);
  foreach ($country as $c) {
    $provinces = location_get_provinces($c);
    foreach ($provinces as $k => $v) {
      if ($p == strtoupper($k) || $p == strtoupper($v)) {
        return $k;
      }
    }
  }
  return '';
}

// @@@ New in 3.x, document.
/**
 * Canonicalize a country code.
 */
function location_standardize_country_code(&$country) {
  $country = trim($country);
  // @@@ Double check the validity of this validity check. ;)
  if (!ctype_alpha($country) || strlen($country) != 2) {
    $country = 'xx';
    return FALSE;
  }
  else {
    $country = strtolower($country);
    return TRUE;
  }
}

// @@@ New in 3.x, document.
/**
 * Load the support file for a country.
 */
function location_load_country($country) {
  location_standardize_country_code($country);
  if ($country != 'xx') {
    include_once(drupal_get_path('module', 'location') .'/supported/location.'. $country .'.inc');
  }
}

// @@@ New in 3.x, document.
/**
 * Load a general geocoding service.
 */
function location_load_geocoder($geocoder) {
  include_once drupal_get_path('module', 'location') .'/geocoding/'. $geocoder .'.inc';
}

/**
 * Create a single line address.
 *
 * @param $location
 *   Array. The address parts
 * @return
 *   String. The single line address
 */
function location_address2singleline($location = array()) {
  // Check if its a valid address
  if (empty($location)) {
    return '';
  }

  $address = '';
  if (!empty($location['street'])) {
    $address .= $location['street'];
  }

  if (!empty($location['city'])) {
    if (!empty($location['street'])) {
      $address .= ', ';
    }

    $address .= $location['city'];
  }

  if (!empty($location['province'])) {
    if (!empty($location['street']) || !empty($location['city'])) {
      $address .= ', ';
    }

    // @@@ Fix this!
    if (substr($location['province'], 0, 3) == $location['country'] .'-') {
      $address .= substr($location['province'], 3);
      watchdog('Location', 'BUG: Country found in province attribute.');
    }
    else {
      $address .= $location['province'];
    }
  }

  if (!empty($location['postal_code'])) {
    if (!empty($address)) {
      $address .= ' ';
    }
    $address .= $location['postal_code'];
  }

  if (!empty($location['country'])) {
    $address .= ', '. $location['country'];
  }

  return $address;
}

function location_get_general_geocoder_list() {
  static $list;

  if (!count($list)) {
    $files = file_scan_directory(drupal_get_path('module', 'location') .'/geocoding', '\.inc$', array('.', '..', 'CVS', '.svn'));
    foreach ($files as $full_path_name => $fileinfo) {
      $list[] = $fileinfo->name;
    }
  }

  return $list;
}

/**
 * The following is an array of all
 * countrycode => country-name pairs as layed out in
 * ISO 3166-1 alpha-2
 */
function location_get_iso3166_list($upper = FALSE) {
  static $countries;

  if (isset($countries)) {
    // In fact, the ISO codes for countries are all Upper Case.
    // So, if someone needs the list as the official records,
    // it will convert.
    if (!empty($upper)) {
      return array_change_key_case($countries, CASE_UPPER);
    }
    return $countries;
  }

  $countries = array(
    'ad' => t('Andorra'),
    'ae' => t('United Arab Emirates'),
    'af' => t('Afghanistan'),
    'ag' => t('Antigua and Barbuda'),
    'ai' => t('Anguilla'),
    'al' => t('Albania'),
    'am' => t('Armenia'),
    'an' => t('Netherlands Antilles'),
    'ao' => t('Angola'),
    'aq' => t('Antarctica'),
    'ar' => t('Argentina'),
    'as' => t('American Samoa'),
    'at' => t('Austria'),
    'au' => t('Australia'),
    'aw' => t('Aruba'),
    'ax' => t('Aland Islands'),
    'az' => t('Azerbaijan'),
    'ba' => t('Bosnia and Herzegovina'),
    'bb' => t('Barbados'),
    'bd' => t('Bangladesh'),
    'be' => t('Belgium'),
    'bf' => t('Burkina Faso'),
    'bg' => t('Bulgaria'),
    'bh' => t('Bahrain'),
    'bi' => t('Burundi'),
    'bj' => t('Benin'),
    'bm' => t('Bermuda'),
    'bn' => t('Brunei'),
    'bo' => t('Bolivia'),
    'br' => t('Brazil'),
    'bs' => t('Bahamas'),
    'bt' => t('Bhutan'),
    'bv' => t('Bouvet Island'),
    'bw' => t('Botswana'),
    'by' => t('Belarus'),
    'bz' => t('Belize'),
    'ca' => t('Canada'),
    'cc' => t('Cocos (Keeling) Islands'),
    'cd' => t('Congo (Kinshasa)'),
    'cf' => t('Central African Republic'),
    'cg' => t('Congo (Brazzaville)'),
    'ch' => t('Switzerland'),
    'ci' => t('Ivory Coast'),
    'ck' => t('Cook Islands'),
    'cl' => t('Chile'),
    'cm' => t('Cameroon'),
    'cn' => t('China'),
    'co' => t('Colombia'),
    'cr' => t('Costa Rica'),
    'cs' => t('Serbia And Montenegro'), // Transitional reservation
    'cu' => t('Cuba'),
    'cv' => t('Cape Verde'),
    'cx' => t('Christmas Island'),
    'cy' => t('Cyprus'),
    'cz' => t('Czech Republic'),
    'de' => t('Germany'),
    'dj' => t('Djibouti'),
    'dk' => t('Denmark'),
    'dm' => t('Dominica'),
    'do' => t('Dominican Republic'),
    'dz' => t('Algeria'),
    'ec' => t('Ecuador'),
    'ee' => t('Estonia'),
    'eg' => t('Egypt'),
    'eh' => t('Western Sahara'),
    'er' => t('Eritrea'),
    'es' => t('Spain'),
    'et' => t('Ethiopia'),
    'fi' => t('Finland'),
    'fj' => t('Fiji'),
    'fk' => t('Falkland Islands'),
    'fm' => t('Micronesia'),
    'fo' => t('Faroe Islands'),
    'fr' => t('France'),
    'ga' => t('Gabon'),
    'gd' => t('Grenada'),
    'ge' => t('Georgia'),
    'gf' => t('French Guiana'),
    'gg' => t('Guernsey'),
    'gh' => t('Ghana'),
    'gi' => t('Gibraltar'),
    'gl' => t('Greenland'),
    'gm' => t('Gambia'),
    'gn' => t('Guinea'),
    'gp' => t('Guadeloupe'),
    'gq' => t('Equatorial Guinea'),
    'gr' => t('Greece'),
    'gs' => t('South Georgia and the South Sandwich Islands'),
    'gt' => t('Guatemala'),
    'gu' => t('Guam'),
    'gw' => t('Guinea-Bissau'),
    'gy' => t('Guyana'),
    'hk' => t('Hong Kong S.A.R., China'),
    'hm' => t('Heard Island and McDonald Islands'),
    'hn' => t('Honduras'),
    'hr' => t('Croatia'),
    'ht' => t('Haiti'),
    'hu' => t('Hungary'),
    'id' => t('Indonesia'),
    'ie' => t('Ireland'),
    'il' => t('Israel'),
    'im' => t('Isle of Man'),
    'in' => t('India'),
    'io' => t('British Indian Ocean Territory'),
    'iq' => t('Iraq'),
    'ir' => t('Iran'),
    'is' => t('Iceland'),
    'it' => t('Italy'),
    'je' => t('Jersey'),
    'jm' => t('Jamaica'),
    'jo' => t('Jordan'),
    'jp' => t('Japan'),
    'ke' => t('Kenya'),
    'kg' => t('Kyrgyzstan'),
    'kh' => t('Cambodia'),
    'ki' => t('Kiribati'),
    'km' => t('Comoros'),
    'kn' => t('Saint Kitts and Nevis'),
    'kp' => t('North Korea'),
    'kr' => t('South Korea'),
    'kw' => t('Kuwait'),
    'ky' => t('Cayman Islands'),
    'kz' => t('Kazakhstan'),
    'la' => t('Laos'),
    'lb' => t('Lebanon'),
    'lc' => t('Saint Lucia'),
    'li' => t('Liechtenstein'),
    'lk' => t('Sri Lanka'),
    'lr' => t('Liberia'),
    'ls' => t('Lesotho'),
    'lt' => t('Lithuania'),
    'lu' => t('Luxembourg'),
    'lv' => t('Latvia'),
    'ly' => t('Libya'),
    'ma' => t('Morocco'),
    'mc' => t('Monaco'),
    'md' => t('Moldova'),
    'me' => t('Montenegro'),
    'mg' => t('Madagascar'),
    'mh' => t('Marshall Islands'),
    'mk' => t('Macedonia'),
    'ml' => t('Mali'),
    'mm' => t('Myanmar'),
    'mn' => t('Mongolia'),
    'mo' => t('Macao S.A.R., China'),
    'mp' => t('Northern Mariana Islands'),
    'mq' => t('Martinique'),
    'mr' => t('Mauritania'),
    'ms' => t('Montserrat'),
    'mt' => t('Malta'),
    'mu' => t('Mauritius'),
    'mv' => t('Maldives'),
    'mw' => t('Malawi'),
    'mx' => t('Mexico'),
    'my' => t('Malaysia'),
    'mz' => t('Mozambique'),
    'na' => t('Namibia'),
    'nc' => t('New Caledonia'),
    'ne' => t('Niger'),
    'nf' => t('Norfolk Island'),
    'ng' => t('Nigeria'),
    'ni' => t('Nicaragua'),
    'nl' => t('Netherlands'),
    'no' => t('Norway'),
    'np' => t('Nepal'),
    'nr' => t('Nauru'),
    'nu' => t('Niue'),
    'nz' => t('New Zealand'),
    'om' => t('Oman'),
    'pa' => t('Panama'),
    'pe' => t('Peru'),
    'pf' => t('French Polynesia'),
    'pg' => t('Papua New Guinea'),
    'ph' => t('Philippines'),
    'pk' => t('Pakistan'),
    'pl' => t('Poland'),
    'pm' => t('Saint Pierre and Miquelon'),
    'pn' => t('Pitcairn'),
    'pr' => t('Puerto Rico'),
    'ps' => t('Palestinian Territory'),
    'pt' => t('Portugal'),
    'pw' => t('Palau'),
    'py' => t('Paraguay'),
    'qa' => t('Qatar'),
    're' => t('Reunion'),
    'ro' => t('Romania'),
    'rs' => t('Serbia'),
    'ru' => t('Russia'),
    'rw' => t('Rwanda'),
    'sa' => t('Saudi Arabia'),
    'sb' => t('Solomon Islands'),
    'sc' => t('Seychelles'),
    'sd' => t('Sudan'),
    'se' => t('Sweden'),
    'sg' => t('Singapore'),
    'sh' => t('Saint Helena'),
    'si' => t('Slovenia'),
    'sj' => t('Svalbard and Jan Mayen'),
    'sk' => t('Slovakia'),
    'sl' => t('Sierra Leone'),
    'sm' => t('San Marino'),
    'sn' => t('Senegal'),
    'so' => t('Somalia'),
    'sr' => t('Suriname'),
    'st' => t('Sao Tome and Principe'),
    'sv' => t('El Salvador'),
    'sy' => t('Syria'),
    'sz' => t('Swaziland'),
    'tc' => t('Turks and Caicos Islands'),
    'td' => t('Chad'),
    'tf' => t('French Southern Territories'),
    'tg' => t('Togo'),
    'th' => t('Thailand'),
    'tj' => t('Tajikistan'),
    'tk' => t('Tokelau'),
    'tl' => t('East Timor'),
    'tm' => t('Turkmenistan'),
    'tn' => t('Tunisia'),
    'to' => t('Tonga'),
    'tr' => t('Turkey'),
    'tt' => t('Trinidad and Tobago'),
    'tv' => t('Tuvalu'),
    'tw' => t('Taiwan'),
    'tz' => t('Tanzania'),
    'ua' => t('Ukraine'),
    'ug' => t('Uganda'),
    'uk' => t('United Kingdom'),
    'um' => t('United States Minor Outlying Islands'),
    'us' => t('United States'),
    'uy' => t('Uruguay'),
    'uz' => t('Uzbekistan'),
    'va' => t('Vatican'),
    'vc' => t('Saint Vincent and the Grenadines'),
    've' => t('Venezuela'),
    'vg' => t('British Virgin Islands'),
    'vi' => t('U.S. Virgin Islands'),
    'vn' => t('Vietnam'),
    'vu' => t('Vanuatu'),
    'wf' => t('Wallis and Futuna'),
    'ws' => t('Samoa'),
    'ye' => t('Yemen'),
    'yt' => t('Mayotte'),
    'za' => t('South Africa'),
    'zm' => t('Zambia'),
    'zw' => t('Zimbabwe'),
  );

  // Sort the list.
  natcasesort($countries);

  // In fact, the ISO codes for countries are all Upper Case.
  // So, if someone needs the list as the official records,
  // it will convert.
  if (!empty($upper)) {
    return array_change_key_case($countries, CASE_UPPER);
  }
  return $countries;
}

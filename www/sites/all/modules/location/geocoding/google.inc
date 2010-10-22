<?php
// $Id: google.inc,v 1.11.2.2 2009/01/09 18:36:48 bdragon Exp $

/**
 * @file
 * Google geocoder.
 */

/**
 * Return the list of ISO3166 codes supported by this geocoder.
 * Coverage list: http://gmaps-samples.googlecode.com/svn/trunk/mapcoverage_filtered.html
 * Possibly in the future, allow to autogenerate this by pulling down and parsing the spreadsheet?
 */
function google_geocode_country_list() {
  return array(
    /* Albania                      */ 'al',
    /* Argentina                    */ 'ar',
    /* Australia                    */ 'au',
    /* Austria                      */ 'at',
    /* Belarus                      */ 'by',
    /* Belgium                      */ 'be',
    /* Bosnia and Herzegovina       */ 'ba',
    /* Brazil                       */ 'br',
    /* Bulgaria                     */ 'bg',
    /* Canada                       */ 'ca',
    /* Chile                        */ 'cl',
    /* China                        */ 'cn',
    /* Croatia                      */ 'hr',
    /* Czech Republic               */ 'cz',
    /* Denmark                      */ 'dk',
    /* Ecuador                      */ 'ec',
    /* El Salvador                  */ 'sv',
    /* Estonia                      */ 'ee',
    /* Finland                      */ 'fi',
    /* France                       */ 'fr',
    /* Germany                      */ 'de',
    /* Greece                       */ 'gr',
    /* Hong Kong                    */ 'hk',
    /* Hungary                      */ 'hu',
    /* India                        */ 'in',
    /* Ireland                      */ 'ie',
    /* Italy                        */ 'it',
    /* Japan                        */ 'jp',
    /* Latvia                       */ 'lv',
    /* Lebanon                      */ 'lb',
    /* Liechtenstein                */ 'li',
    /* Lithuania                    */ 'lt',
    /* Luxembourg                   */ 'lu',
    /* Macau                        */ 'mo',
    /* Macedonia                    */ 'mk',
    /* Malaysia                     */ 'my',
    /* Mexico                       */ 'mx',
    /* Moldova                      */ 'md',
    /* Montenegro                   */ 'me',
    /* Netherlands                  */ 'nl',
    /* New Zealand                  */ 'nz',
    /* Nicaragua                    */ 'ni',
    /* Norway                       */ 'no',
    /* Poland                       */ 'pl',
    /* Portugal                     */ 'pt',
    /* Romania                      */ 'ro',
    /* Russia                       */ 'ru',
    /* San Marino                   */ 'sm',
    /* Serbia                       */ 'rs',
    /* Singapore                    */ 'sg',
    /* Slovakia                     */ 'sk',
    /* Slovenia                     */ 'si',
    /* South Korea                  */ 'kr',
    /* Spain                        */ 'es',
    /* Sweden                       */ 'se',
    /* Switzerland                  */ 'ch',
    /* Taiwan                       */ 'tw',
    /* Thailand                     */ 'th',
    /* Turkey                       */ 'tr',
    /* Ukraine                      */ 'ua',
    /* United Kingdom               */ 'uk',
    /* United States                */ 'us',
    /* Uruguay                      */ 'uy',
  );
}

/**
 * Return general information about this geocoder.
 */
function google_geocode_info() {
  return array(
    'name' => 'Google Maps',
    'url' => 'http://maps.google.com',
    'tos' => 'http://www.google.com/help/terms_local.html',
    'general' => TRUE,
  );
}

/**
 * Perform a geocode on a location array.
 * @param $location
 *   The location array to process.
 * @return
 *   an associative array with keys 'lat' and 'lon' containing the coordinates.
 */
function google_geocode_location($location = array()) {

  if (function_exists('gmap_get_key')) {
    $key = gmap_get_key();
  }
  else {
    $key = variable_get('location_geocode_google_apikey', '');
  }

  $query = array(
    'key' => $key,
    'sensor' => 'false', // Required by TOS.
    'output' => 'xml',
    //'ll' => 0,
    //'spn' => 0,
    'gl' => $location['country'],
    'q' => _google_geocode_flatten($location),
  );

  $url = url('http://maps.google.com/maps/geo', array(
    'query' => $query,
    'external' => TRUE,
  ));

  $http_reply = drupal_http_request($url);

  $status_code_match = array();
  preg_match('/<code>(.*)<\/code>/', $http_reply->data, $status_code_match);
  $status_code = $status_code_match[1];
  if ($status_code != 200) {
    return NULL;
  }

  $accuracy_code_match = array();
  preg_match('/Accuracy="([0-9])"/', $http_reply->data, $accuracy_code_match);
  $accuracy_code = $accuracy_code_match[1];
  if ($accuracy_code < 3) {
    return NULL;
  }

  $latlon_match = array();
  preg_match('/<coordinates>(.*)<\/coordinates>/', $http_reply->data, $latlon_match);

  $latlon_exploded = explode(',', $latlon_match[1]);

  return array('lat' => $latlon_exploded[1], 'lon' => $latlon_exploded[0]);
}

/**
 * General settings for this geocoder.
 */
function google_geocode_settings() {
  $form = array();
  $key = '';
  if (function_exists('gmap_get_key')) {
    $key = gmap_get_key();
  }

  if (!empty($key)) {
    $form['location_geocode_google_apikey'] = array(
      '#type' => 'item',
      '#title' => t('Google Maps API Key'),
      '#value' => $key,
      '#description' => t('The key in use was automatically provided by GMap.'),
    );
  }
  else {
    $form['location_geocode_google_apikey'] = array(
      '#type' => 'textfield',
      '#title' => t('Google Maps API Key'),
      '#size' => 64,
      '#maxlength' => 128,
      '#default_value' => variable_get('location_geocode_google_apikey', ''),
      '#description' => t('In order to use the Google Maps API geocoding web-service, you will need a Google Maps API Key.  You can obtain one at the !sign_up_link for the !google_maps_api.  PLEASE NOTE: You will <em>not</em> have to re-enter your API key for each country for which you have selected Google Maps for geocoding.  This setting is global.', array('!sign_up_link' => '<a href="http://www.google.com/apis/maps/signup.html">sign-up page</a>', '!google_maps_api' => '<a href="http://www.google.com/apis/maps/">Google Maps API</a>'))
    );
  }

  return $form;
}

function _google_geocode_flatten($location = array()) {
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

  return $address;
}

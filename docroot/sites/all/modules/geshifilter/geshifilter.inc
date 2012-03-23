<?php
// $Id: geshifilter.inc,v 1.5 2008/07/21 17:44:23 soxofaan Exp $

/**
 * @file
 * General GeSHi filter helper functions.
 */

function _geshifilter_get_geshi_dir() {
  return variable_get('geshifilter_geshi_dir', drupal_get_path('module', 'geshifilter') .'/geshi');
}

/**
 * Helper function for loading/checking the GeSHi library v 1.0.x (if not already)
 * Returns an array with keys 'success', 'loaded' and 'message'
 */
function _geshifilter_check_geshi_library($use_cache=TRUE, $geshi_dir=NULL, $load_when_found=TRUE) {
  static $geshi_library_cache = NULL;
  if ($use_cache && $geshi_library_cache !== NULL) {
    // get from cache
    $geshi_library = $geshi_library_cache;
  }
  else {
    // initialisation
    $geshi_library = array('success' => NULL, 'loaded' => FALSE, 'message' => NULL);
    // no cache
    if (!$geshi_dir) {
      $geshi_dir = _geshifilter_get_geshi_dir();
    }
    if (!is_dir($geshi_dir)) {
      $geshi_library['success'] = FALSE;
      $geshi_library['message'] = t('GeSHi library error: %dir is not a directory.', array('%dir' => $geshi_dir));
    }
    elseif (is_file($geshi_dir .'/geshi.php')) {
      // GeSHi 1.0.x found (probably, we can only be sure by loading it)
      $geshi_library['success'] = TRUE;
      if ($load_when_found) {
        require_once($geshi_dir .'/geshi.php');
        // check version
        $geshi_library_version = explode('.', GESHI_VERSION);
        if (!($geshi_library_version[0] == '1' &&  $geshi_library_version[1] == '0')) {
          $geshi_library['success'] = FALSE;
          $geshi_library['loaded'] = FALSE;
          $geshi_library['message'] = t('GeSHi library error: The detected version of GeSHi library (%version) is not supported. A version from the 1.0.x branch is required.', array('%version' => GESHI_VERSION));
        }
        else {
          $geshi_library['loaded'] = TRUE;
        }
      }
    }
    else {
      $geshi_library['success'] = FALSE;
      $geshi_library['message'] = t('GeSHi library error: Could not find a known version of the GeSHi library at %dir.' , array('%dir' => $geshi_dir));
    }
    // store in cache if needed
    if ($use_cache) {
      $geshi_library_cache = $geshi_library;
    }
  }
  return $geshi_library;
}

/**
 * List of available languages.
 * @return an array mapping language code to array with the language path and full language name
 */
function _geshifilter_get_available_languages() {
  // try to get it from cache (database actually)
  $available_languages = variable_get('geshifilter_available_languages_cache', NULL);
  if ($available_languages === NULL) {
    // not in cache: build the array of available_languages
    $geshi_library = _geshifilter_check_geshi_library();
    $available_languages = array();
    if ($geshi_library['success']) {
      $dirs = array(_geshifilter_get_geshi_dir() .'/geshi', drupal_get_path('module', 'geshifilter') .'/geshi-extra');
      foreach ($dirs as $dir) {
        foreach (file_scan_directory($dir, '\.[pP][hH][pP]$') as $filename => $fileinfo) {
          // short name
          $name = $fileinfo->name;
          // get full name
          $geshi = new GeSHi('', $name);
          $geshi->set_language_path($dir);
          $fullname = $geshi->get_language_name();
          unset($geshi);
          // store
          $available_languages[$name] = array('language_path' => $dir, 'fullname' => $fullname);
        }
      }
      ksort($available_languages);
      // save array to database
      variable_set('geshifilter_available_languages_cache', $available_languages);
    }
  }
  return $available_languages;
}

/**
 * List of enabled languages.
 * (with caching)
 * @return array with enabled languages mapping language code to full name.
 */
function _geshifilter_get_enabled_languages() {
  static $enabled_languages = NULL;
  if ($enabled_languages === NULL) {
    $enabled_languages = array();
    $languages = _geshifilter_get_available_languages();
    foreach ($languages as $language => $language_data) {
      if (variable_get('geshifilter_language_enabled_'. $language, FALSE)) {
        $enabled_languages[$language] = $language_data['fullname'];
      }
    }
  }
  return $enabled_languages;
}

/**
 * Helper function for gettings the tags
 * (with caching)
 */
function _geshifilter_get_tags($format) {
  static $geshifilter_tags_cache = array();
  if (!isset($geshifilter_tags_cache[$format])) {
    $generic_code_tags = _geshifilter_tag_split(geshifilter_tags($format));
    $language_tags = array();
    $tag_to_lang = array();
    $enabled_languages = _geshifilter_get_enabled_languages();
    foreach ($enabled_languages as $language => $fullname) {
      $lang_tags = _geshifilter_tag_split(geshifilter_language_tags($language, $format));
      foreach ($lang_tags as $lang_tag) {
        $language_tags[] = $lang_tag;
        $tag_to_lang[$lang_tag] = $language;
      }
    }
    $geshifilter_tags_cache[$format] = array($generic_code_tags, $language_tags, $tag_to_lang);
  }
  return $geshifilter_tags_cache[$format];
}

/**
 * helper function for generating a GeSHi object
 * @param $language the language to generate a GeSHi object for
 */
function _geshifilter_geshi_factory($source_code, $language) {
  $available_languages = _geshifilter_get_available_languages();
  $geshi = new GeSHi($source_code, $language);
  $geshi->set_language_path($available_languages[$language]['language_path']);
  return $geshi;
}

/**
 * Helper function for splitting a string on white spaces.
 * Using explode(' ', $string) is not enough because it returns empty elements
 * if $string contains consecutive spaces.
 */
function _geshifilter_whitespace_explode($string) {
  return preg_split('/\s+/', $string, -1, PREG_SPLIT_NO_EMPTY);
}

function _geshifilter_tag_split($string) {
  return preg_split('/\s+|<|>|\[|\]/', $string, -1, PREG_SPLIT_NO_EMPTY);
}

// General settings
function geshifilter_use_format_specific_options() {
  return variable_get('geshifilter_format_specific_options', FALSE);
}

function geshifilter_tags($format = NULL) {
  if (!geshifilter_use_format_specific_options() || $format === NULL) {
    return variable_get('geshifilter_tags', 'code blockcode');
  }
  return variable_get("geshifilter_tags_{$format}", geshifilter_tags());
}

function _geshifilter_php_delimeters($format = NULL) {
  if (!geshifilter_use_format_specific_options() || $format === NULL) {
    return variable_get('geshifilter_enable_php_delimiters', FALSE);
  }
  return variable_get("geshifilter_enable_php_delimiters_{$format}", _geshifilter_php_delimeters());
}

function _geshifilter_brackets($format = NULL) {
  if (!geshifilter_use_format_specific_options() || $format === NULL) {
    return variable_get('geshifilter_brackets', GESHIFILTER_BRACKETS_BOTH);
  }
  return variable_get("geshifilter_brackets_{$format}", _geshifilter_brackets());
}

function geshifilter_language_tags($language, $format = NULL) {
  if (!geshifilter_use_format_specific_options() || $format === NULL)
    return variable_get("geshifilter_language_tags_{$language}", '');
  return variable_get("geshifilter_language_tags_{$language}_{$format}", geshifilter_language_tags($language));
}

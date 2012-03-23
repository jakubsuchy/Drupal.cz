<?php
// $Id: random_captcha_type.inc,v 1.1 2008/02/13 22:32:15 soxofaan Exp $

/**
 * Function for getting all the possible CAPTCHA types to switch between.
 * For use as #options entry of a checkboxes widget.
 */
function _random_captcha_type_get_all_types() {
  $captcha_types = array();
  foreach (module_implements('captcha') as $module) {
    // skip random_captcha_type module
    if ($module == 'random_captcha_type') {
      continue;
    }
    // get available  types
    $result = module_invoke($module, 'captcha', 'list');
    if (is_array($result)) {
      foreach ($result as $type) {
        $captcha_types["$module/$type"] = "$type ($module)";
      }
    }
  }
  return $captcha_types;
}

/**
 * Function for getting the enabled CAPTCHA types.
 * Alternative of variable_get('random_captcha_type_enabled_types', ...) with
 * sensible default value.
 * For use as #default_value of checkboxes widget.
 * Returns an array mapping "$module/$type" to "$module/$type" for the enabled types
 */
function _random_captcha_type_get_enabled_types() {
  $enabled_types = array();
  $_enabled_types = variable_get('random_captcha_type_enabled_types', NULL);
  if ($_enabled_types === NULL) {
    foreach (_random_captcha_type_get_all_types() as $key => $value) {
      $enabled_types[$key] = $key;
    }
    variable_set('random_captcha_type_enabled_types', $enabled_types);
  }
  else {
    foreach ($_enabled_types as $key => $value) {
      if ($value) {
        list($module, $type) = explode('/', $value);
        // check if type is still available
        $list = module_invoke($module, 'captcha', 'list');
        if ($list && in_array($type, $list)) {
            $enabled_types[$key] = $value;
        }
      }
    }
  }
  return $enabled_types;
}

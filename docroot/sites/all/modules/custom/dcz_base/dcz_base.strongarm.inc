<?php

/**
 * Implementation of hook_strongarm().
 */
function dcz_base_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'bueditor_roles';
  $strongarm->value = array(
    4 => array(
      'weight' => '0',
      'editor' => '0',
      'alt' => '0',
    ),
    3 => array(
      'weight' => '0',
      'editor' => '0',
      'alt' => '0',
    ),
    6 => array(
      'weight' => '0',
      'editor' => '0',
      'alt' => '0',
    ),
    7 => array(
      'weight' => '0',
      'editor' => '0',
      'alt' => '0',
    ),
    8 => array(
      'weight' => '0',
      'editor' => '0',
      'alt' => '0',
    ),
    2 => array(
      'editor' => '1',
      'alt' => '0',
      'weight' => 11,
    ),
    1 => array(
      'editor' => '1',
      'alt' => '0',
      'weight' => 12,
    ),
  );
  $export['bueditor_roles'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'bueditor_user1';
  $strongarm->value = '1';
  $export['bueditor_user1'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'bueditor_user1_alt';
  $strongarm->value = '0';
  $export['bueditor_user1_alt'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'shield_allow_cli';
  $strongarm->value = 1;
  $export['shield_allow_cli'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'shield_pass';
  $strongarm->value = 'cz';
  $export['shield_pass'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'shield_print';
  $strongarm->value = '--- Username: [user], Password: [pass] --- --- Username: [user], Password: [pass] --- --- Username: [user], Password: [pass] ---';
  $export['shield_print'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'shield_state';
  $strongarm->value = 1;
  $export['shield_state'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'shield_user';
  $strongarm->value = 'drupal';
  $export['shield_user'] = $strongarm;

  return $export;
}

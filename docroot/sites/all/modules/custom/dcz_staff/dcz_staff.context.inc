<?php

/**
 * Implementation of hook_context_default_contexts().
 */
function dcz_staff_context_default_contexts() {
  $export = array();

  $context = new stdClass;
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'staff';
  $context->description = '';
  $context->tag = '';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'node/11901' => 'node/11901',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'block-26' => array(
          'module' => 'block',
          'delta' => 26,
          'region' => 'obsahdole',
          'weight' => 0,
        ),
        'views-staff-block_2' => array(
          'module' => 'views',
          'delta' => 'staff-block_2',
          'region' => 'obsahdole',
          'weight' => 1,
        ),
        'block-41' => array(
          'module' => 'block',
          'delta' => 41,
          'region' => 'obsahdole',
          'weight' => 2,
        ),
        'views-staff-block_1' => array(
          'module' => 'views',
          'delta' => 'staff-block_1',
          'region' => 'obsahdole',
          'weight' => 3,
        ),
        'block-36' => array(
          'module' => 'block',
          'delta' => 36,
          'region' => 'obsahdole',
          'weight' => 4,
        ),
        'views-staff-block_3' => array(
          'module' => 'views',
          'delta' => 'staff-block_3',
          'region' => 'obsahdole',
          'weight' => 5,
        ),
        'block-31' => array(
          'module' => 'block',
          'delta' => 31,
          'region' => 'obsahdole',
          'weight' => 6,
        ),
      ),
    ),
  );
  $context->condition_mode = 0;
  $export['staff'] = $context;

  return $export;
}

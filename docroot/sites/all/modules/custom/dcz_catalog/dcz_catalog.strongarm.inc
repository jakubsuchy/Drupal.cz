<?php

/**
 * Implementation of hook_strongarm().
 */
function dcz_catalog_strongarm() {
  $export = array();

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_anonymous_content_katalog';
  $strongarm->value = '1';
  $export['comment_anonymous_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_content_katalog';
  $strongarm->value = '2';
  $export['comment_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_controls_content_katalog';
  $strongarm->value = '3';
  $export['comment_controls_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_mode_content_katalog';
  $strongarm->value = '4';
  $export['comment_default_mode_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_order_content_katalog';
  $strongarm->value = '2';
  $export['comment_default_order_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_per_page_content_katalog';
  $strongarm->value = '50';
  $export['comment_default_per_page_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_form_location_content_katalog';
  $strongarm->value = '1';
  $export['comment_form_location_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_preview_content_katalog';
  $strongarm->value = '0';
  $export['comment_preview_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_subject_field_content_katalog';
  $strongarm->value = '1';
  $export['comment_subject_field_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'content_extra_weights_content_katalog';
  $strongarm->value = array(
    'title' => '-5',
    'menu' => '-2',
    'taxonomy' => '-3',
    'book' => '10',
  );
  $export['content_extra_weights_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_content_katalog';
  $strongarm->value = array();
  $export['node_options_content_katalog'] = $strongarm;

  $strongarm = new stdClass;
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'upload_content_katalog';
  $strongarm->value = '0';
  $export['upload_content_katalog'] = $strongarm;

  return $export;
}

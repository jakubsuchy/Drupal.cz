<?php


/**
 * Implementation of hook_flag_default_flags().
 */
function dcz_frontpage_flag_default_flags() {
  $flags = array();
  
  // Exported flag: "Promote frontpage".
  $flags['dcz_frontpage'] = array(
    'content_type' => 'node',
    'title' => 'Promote frontpage',
    'global' => '0',
    'types' => array(
      '0' => 'blog',
    ),
    'flag_short' => 'Promote to frontpage',
    'flag_long' => '',
    'flag_message' => '',
    'unflag_short' => 'Unpromote from frontpage',
    'unflag_long' => '',
    'unflag_message' => '',
    'unflag_denied_text' => '',
    'link_type' => 'confirm',
    'roles' => array(
      'flag' => array(
          '0' => 3,
      ),
      'unflag' => array(
          '0' => 3,
      ),
    ),
    'weight' => 0,
    'show_on_page' => 1,
    'show_on_teaser' => 0,
    'show_on_form' => 0,
    'access_author' => '',
    'i18n' => 1,
    'flag_confirmation' => 'Are you sure you want to promote this content to frontpage?',
    'unflag_confirmation' => 'Are you sure you want to unpromote this content from frontpage?',
    'api_version' => 2,
    'module' => 'dcz_frontpage',
    'locked' => array(
      '0' => 'name',
    ),
  );
  return $flags;
}

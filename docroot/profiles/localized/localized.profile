<?php
// $Id: localized.profile,v 1.1 2006/12/27 16:01:08 goba Exp $

/**
 * Return an array of the modules to be enabled when this profile is installed.
 *
 * @return
 *  An array of modules to be enabled.
 */
function localized_profile_modules() {
  return array('block', 'color', 'comment', 'filter', 'help', 'menu', 'node', 'system', 'taxonomy', 'user', 'watchdog', 'locale', 'autolocale', 'pathauto', 'path');
}

/**
 * Return a description of the profile.
 */
function localized_profile_details() {
  return array(
    'name' => 'Český Drupal',
    'description' => 'Základní instalace Drupalu s hotovým českým překladem a modulem pro automatické generování URL.'
  );
}

/**
 * Uses functionality in autolocale.install to import PO files
 */
function localized_install() {
  _autolocale_install_po_files();
}

function localized_profile_final() {
  // Insert default user-defined node types into the database.
  $types = array(
    array(
      'type' => 'page',
      'name' => st('Page'),
      'module' => 'node',
      'description' => st('If you want to add a static page, like a contact page or an about page, use a page.'),
      'custom' => TRUE,
      'modified' => TRUE,
      'locked' => FALSE,
    ),
    array(
      'type' => 'story',
      'name' => st('Story'),
      'module' => 'node',
      'description' => st('Stories are articles in their simplest form: they have a title, a teaser and a body, but can be extended by other modules. The teaser is part of the body too. Stories may be used as a personal blog or for news articles.'),
      'custom' => TRUE,
      'modified' => TRUE,
      'locked' => FALSE,
    ),
  );

  foreach ($types as $type) {
    $type = (object) _node_type_set_defaults($type);
    node_type_save($type);
  }

  // Default page to not be promoted and have comments disabled.
  variable_set('node_options_page', array('status'));
  variable_set('comment_page', COMMENT_NODE_DISABLED);

  // Configure pathauto
  variable_set('pathauto_version', array('text' => "2005-9-18", "build" => 5));
  variable_set('pathauto_indexaliases_bulkupdate', 0);
  variable_set('pathauto_node_supportsfeeds', 'feed');
  variable_set('pathauto_taxonomy_supportsfeeds', '0/feed');
  variable_set('pathauto_user_supportsfeeds', '');
  variable_set('pathauto_modulelist', array('node', 'taxonomy', 'user'));
  variable_set('pathauto_verbose', 0);
  variable_set('pathauto_separator', "-");
  variable_set('pathauto_quotes', "0");
  variable_set('pathauto_max_length', 100);
  variable_set('pathauto_max_component_length', "100");
  variable_set('pathauto_indexaliases', 0);
  variable_set('pathauto_update_action', "2");
  variable_set('pathauto_ignore_words', "a,an,as,at,before,but,by,for,from,is,in,into,like,of,off,on,onto,per,since,than,the,this,that,to,up,via,with");
  variable_set('pathauto_node_pattern', "[title]");
  variable_set('pathauto_node_page_pattern', "");
  variable_set('pathauto_node_story_pattern', "");
  variable_set('pathauto_node_bulkupdate', 0);
  variable_set('pathauto_node_applytofeeds', 0);
  variable_set('pathauto_taxonomy_pattern', "[vocab]/[catpath]");
  variable_set('pathauto_taxonomy_bulkupdate', 0);
  variable_set('pathauto_taxonomy_applytofeeds', 0);
  variable_set('pathauto_user_pattern', "user/[user]");
  variable_set('pathauto_user_bulkupdate', 0);

  // Configure date settings for Czech language
  variable_set('date_default_timezone', "3600");
  variable_set('configurable_timezones', 0);
  variable_set('date_format_short', "d.m.Y - H:i");
  variable_set('date_format_medium', "j. F Y - G:i");
  variable_set('date_format_long', "l, j. F, Y - G:i");
  variable_set('date_first_day', 1);

  // Don't display date and author information for page nodes by default.
  $theme_settings = variable_get('theme_settings', array());
  $theme_settings['toggle_node_info_page'] = FALSE;
  variable_set('theme_settings', $theme_settings);
}

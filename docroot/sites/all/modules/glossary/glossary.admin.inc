<?php
// $Id: glossary.admin.inc,v 1.1.2.28 2008/09/03 18:40:32 nancyw Exp $

/**
 * @file
 *   Glossary administration functions and forms.
 */

/* 
 *  This is the introductory stuff for the settings.
 */ 
function glossary_settings_page() {
  $mb_status = extension_loaded('mbstring') ? t('enabled') : t('disabled');
  $overload = ini_get('mbstring.func_overload') ? t('enabled') : t('disabled');
  $output = '<p>'. t('Multibyte string support is !status; multibyte function overload is !overload.', array('!status' => $mb_status, '!overload' => $overload)) .'</p>'; 
  $output .= drupal_get_form('glossary_general_settings_form');
  return $output;
}

function glossary_general_settings_form() {
  drupal_add_js(drupal_get_path('module', 'glossary') .'/glossary.js', 'module');

  $form = array();

  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    );

  $form['general']['glossary_disable_indicator'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow the user to disable glossary links.'),
    '#default_value' => variable_get('glossary_disable_indicator', FALSE),
    '#description' => t('Determines whether or not the individual user may disable the Glossary indicators.'),
    );

  $form['general']['glossary_hide_menus'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide unused input format tabs.'),
    '#default_value' => variable_get('glossary_hide_menus', FALSE),
    '#description' => t('Determines whether or not to hide settings tabs for input formats that are not glossary-enabled. Changing this setting may require you to clear the cache_menu table.'),
    );

  $click_options = array(
    t('Show only the single term.'),
    t('Advance the whole glossary to the term.'),
    );
  $form['general']['glossary_click_option'] = array(
    '#type' => 'radios',
    '#title' => t('Clicking on a term link will'),
    '#options' => $click_options,
    '#default_value' => variable_get('glossary_click_option', 0),
    '#description' => t('Changing this setting may require you to clear the cache_filter.'),
    '#prefix' => '<div class="glossary_radios">',
    '#suffix' => '</div>',
    );

  $form['general']['glossary_show_edit'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show "edit" link.'),
    '#default_value' => variable_get('glossary_show_edit', TRUE),
    '#description' => t('Determines whether or not to show an "edit term" link for each entry.'),
    );

  $form['general']['glossary_show_search'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show "search" link.'),
    '#default_value' => variable_get('glossary_show_search', TRUE),
    '#description' => t('Determines whether or not to show an "search for term" link for each entry.'),
    );

  $form['glossary_page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Glossary Page'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    );

    // This next setting cannot vary by format since the glossary overview doesn't care or know about input formats
  $form['glossary_page']['glossary_page_per_letter'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show glossary across many smaller pages.'),
    '#default_value' => variable_get('glossary_page_per_letter', FALSE),
    '#description' => t('Do you want to show all terms on one glossary page or break up the glossary into a page for each first letter (i.e. many pages).'),
    );

  $form['glossary_page']['glossary_separate_letters'] = array(
    '#type' => 'checkbox',
    '#title' => t('Separate letters.'),
    '#default_value' => variable_get('glossary_separate_letters', FALSE),
    '#description' => t('Separate the terms by the first letters. This will create a large letter at the beginning of each section.'),
    );

  $form['glossary_page']['glossary_allow_no_description'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show glossary terms even if there is no description.'),
    '#default_value' => variable_get('glossary_allow_no_description', FALSE),
    '#description' => t('By default, Glossary omits terms from the list if there is no term description. This setting overrides that. This is useful on free-tagging vocabularies that rarely get descriptions.'),
    );

  $form['glossary_page']['glossary_show_description'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show glossary term descriptions on the Glossary page.'),
    '#default_value' => variable_get('glossary_show_description', FALSE),
    '#description' => t('Glossary term descriptions may be large and/or include pictures, therefore the Glossary page can take a long time to load if you include the full descriptions.'),
    );

  $form['glossary_page']['glossary_show_detailed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show detailed descriptions on the Glossary page.'),
    '#default_value' => variable_get('glossary_show_detailed', FALSE),
    '#description' => t('Glossary terms may have nodes associated with them. This option allows you to include the teasers of those nodes under the term.'),
    );

  $form['glossary_page']['glossary_link_related'] = array(
    '#type' => 'checkbox',
    '#title' => t('Link related terms on the Glossary page.'),
    '#default_value' => variable_get('glossary_link_related', TRUE),
    '#description' => t('Do you want terms that are related to link to each other? The type of link is determined by "Clicking on a term link will" above.'),
    );

  $form['glossary_page']['glossary_link_related_how'] = array(
    '#type' => 'checkbox',
    '#title' => t('Related terms link one-way.'),
    '#default_value' => variable_get('glossary_link_related_how', FALSE),
    '#description' => t('By default, links are two way, that is if "a" is related to "b" then "b" is also related to "a". This option changes that so that "b" points to "a" only if it is set explicitly for the term. Requires "Link related" above to be selected.'),
    '#prefix' => '<div class="glossary_link_related_how">',
    '#suffix' => '</div>',
    );

  $form['glossary_page']['glossary_term_class'] = array(
    '#type' => 'textfield',
    '#title' => t('Term link class'),
    '#default_value' => variable_get('glossary_term_class', 'glossary-term'),
    '#description' => t('This is the style class that will be used for "acronym," "abbr," and "hovertip" links. It should only be used by those with specific site standards.'),
    );

  return system_settings_form($form);
}

/* 
 *  This is the form for the getting the user's alphabet for the alphabar.
 */ 
function glossary_alphabet_form() {
  global $language;
  $form = array();
  
//  $status = db_fetch_array(db_query("SHOW TABLE STATUS LIKE 'term_data'"));
//  $lang = $language->language .' ('. $language->name .')';
//  $form['locale'] = array(
//    '#type' => 'markup',
//    '#value' => '<big>'. t('The current locale is set to "@loc". The term_data collation is "!collate".', array('@loc' => $lang, '!collate' => $status['Collation'])) .'</big>',
//    );

  $form['alphabet'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter all the letters of your alphabet, in the correct order, and in lower case.'),
    '#default_value' => implode(' ', variable_get('glossary_alphabet', range('a', 'z'))),
    '#description' => t('Separate the letters by a blank.'),
    '#rows' => 1,
    );

  $form['digits'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter all the digits of your alphabet, in the correct order.'),
    '#default_value' => implode(' ', variable_get('glossary_digits', range('0', '9'))),
    '#description' => t("Separate the digits by a blank. If you don't want terms to start with digits, leave this blank."),
    '#rows' => 1,
    );

  $form['suppress_unused'] = array(
    '#type' => 'checkbox',
    '#title' => t('Suppress unused letters?'),
    '#default_value' => variable_get('glossary_suppress_unused', FALSE),
    '#description' => t('This will cause unused letters to be omitted from the alphabar.'),
    );

  $ab_seps = array(
    ' ' => t('&lt;none>'),
    '|' => t('vertical bar (pipe)'),
    '&bull;' => t('bullet'),
    '&#8211;' => t('en-dash (&#8211;)'),
    '&#8212;' => t('em-dash (&#8212;)'),
    '_' => t('underscore'),
    );
  $form['alphabar_separator'] = array(
    '#type' => 'radios',
    '#options' => $ab_seps,
    '#title' => t('Alphabar separator'),
    '#default_value' => variable_get('glossary_alphabar_separator', '|'),
    '#description' => t('This is the character that will separate the letters in the alphabar.'),
    '#prefix' => '<div class="glossary_radios">',
    '#suffix' => '</div>',
    );

  $form['alphabar_instruction'] = array(
    '#type' => 'textarea',
    '#title' => t('Alphabar instruction'),
    '#default_value' => variable_get('glossary_alphabar_instruction', _alphabar_instruction_default()),
    '#description' => t('This is the text that will appear immediately below the alphabar.'),
    '#rows' => 1,
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#weight' => 5,
    );

  return $form;    
}

function glossary_alphabet_form_submit($form, &$form_state) {
  variable_set('glossary_alphabet', explode(' ', $form_state['values']['alphabet']));
  if ($form_state['values']['digits']) {
    variable_set('glossary_digits', explode(' ', $form_state['values']['digits']));
  }
  else {
    variable_set('glossary_digits', array());
  }
  variable_set('glossary_suppress_unused', $form_state['values']['suppress_unused']);
  variable_set('glossary_alphabar_separator', $form_state['values']['alphabar_separator']);
  variable_set('glossary_alphabar_instruction', $form_state['values']['alphabar_instruction']);

  drupal_set_message(t('Configuration saved'), 'status');
}

/* 
 *  This is the form for the settings for an individual input format.
 */ 
function glossary_filter_form($form_state, $format = 1) {
  drupal_add_js(drupal_get_path('module', 'glossary') .'/glossary.js', 'module');

  $form = array();

  drupal_add_js(drupal_get_path('module', 'glossary') .'/glossary.js', 'module');

  $options = array();
  $result = db_query('SELECT vid, name FROM {vocabulary} ORDER BY name');
  while ($vocabulary = db_fetch_array($result)) {
    $options[$vocabulary['vid']] = $vocabulary['name'];
  }
  if (!$options) {
    drupal_set_message(t('No vocabularies were found. Until you set up, and select, at least one vocabulary for Glossary, no substitutions can be done.'));
  }
  
  // Make sure we know if we need to clear the cache.
  variable_del('glossary_need_to_clear_cache', TRUE);

  // Get information about this filter.
  $filter = db_fetch_array(db_query_range('SELECT * FROM {filter_formats} WHERE format=%d', 0, 1, $format));
  // See if we are enabled for this input format.
  $enabled = db_result(db_query("SELECT COUNT(delta) FROM {filters} WHERE format=%d AND module='glossary'", $format));
  if ($enabled) {
    $enabled_msg = NULL;
  }
  else {
    $enabled_msg = '<p><big>'. t('The Glossary module is not enabled for this input format. <a href="!url">Change the settings</a>.', array('!url' => url('admin/settings/filters/'. $format))) .'</big></p>';
  }

  if ($filter['cache']) {
    $cache_msg = t('This filter may be cached.');

    // Is there anything in the cache now?
    // We can't use cache_get because we would need the md5 hash of the text.  
    $cached_content = db_result(db_query("SELECT COUNT(cid) FROM {cache_filter} WHERE cid LIKE ('%d:%%')", $format));
    if ($cached_content == 0) {
      $cache_exist = FALSE;
    }
    else {
      $cache_exist = TRUE;
      $cache_msg .= ' '. t('There is currently cached data for this input format.');
    }
    $cache_lifetime = variable_get('cache_lifetime', 0);
    if ($cache_lifetime) {
      $cache_msg .= ' '. t('You are using a minimum cache lifetime of !life.', array('!life' => format_interval($cache_lifetime, 1)));
    }
    else {
      $cache_msg .= ' '. t('You do not have a minimum cache lifetime.');
    }
  }
  else {
    $cache_msg = t('This filter may not be cached.');
  }

  $form['format'] = array(
    '#type' => 'value',
    '#value' => $format,
    );

  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Input format settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => $enabled_msg . $cache_msg
      .'<br/>'. t('Submitting this form will clear the "cache_filter" table, which will have a short-term performance impact.'),
    );

  if (!$enabled) {
    return $form;
  }

  $form['filter']["glossary_vids_$format"] = array(
//    '#type' => 'select',
    '#type' => 'checkboxes',   // <--- has a problem with "<none>"
    '#title' => t('Select Vocabulary'),
    '#default_value' => variable_get("glossary_vids_$format", array()),
    '#options' => $options,
    '#description' => t('Select one or more vocabularies which hold all terms for your glossary. When enabled, posts will be scanned for glossary terms from the selected vocabulary(ies) and an icon or link will be inserted for each term. Choosing no vocabularies will result in no terms being flagged.'),
    '#multiple' => TRUE,
    '#required' => FALSE,
    '#prefix' => '<div class="glossary_checkboxes">',
    '#suffix' => '</div>',
    );

  $form['filter']['match'] = array(
    '#type' => 'fieldset',
    '#title' => t('Term matching'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    );
    
  $form['filter']['match']["glossary_match_$format"] = array(
//    '#type' => 'select',
    '#type' => 'radios',
    '#title' => t('Match type'),
    '#default_value' => variable_get("glossary_match_$format", 'b'),
    '#options' => array(
      'b' => t('Word'),
      'lr' => t('Right or left substring'),
      'l' => t('Left substring'),
      'r' => t('Right substring'),
      's' => t('Any substring'),
      ),
    '#description' => t('Choose the match type of glossary links. "Word" means a word break must occur on both sides of the term. "Right or left" requires a word break on either side. "Left" requires a word break on the left side of the term. "Right" requires a word break on the right. "Any" means any substring will match.'),
    '#prefix' => '<div class="glossary_radios">',
    '#suffix' => '</div>',
    );

  $form['filter']['match']["glossary_case_$format"] = array(
//    '#type' => 'select',
    '#type' => 'radios',
    '#title' => t('Case sensitivity'),
    '#default_value' => variable_get("glossary_case_$format", '1'),
    '#options' => array(
      t('Case insensitive'),
      t('Case sensitive')
      ),
    '#description' => t('Match either case sensitive or not. Case sensitive matches are not very resource intensive.'),
    '#prefix' => '<div class="glossary_radios">',
    '#suffix' => '</div>',
    );

  $form['filter']['match']["glossary_replace_all_$format"] = array(
//    '#type' => 'select',
    '#type' => 'radios',
    '#title' => t('Replace matches'),
    '#default_value' => variable_get("glossary_replace_all_$format", 0),
    '#options' => array(
      t('Only the first match'),
      t('All matches')
      ),
    '#description' => t('Whether only the first match should be replaced or all matches.'),
    '#prefix' => '<div class="glossary_radios">',
    '#suffix' => '</div>',
    );

  $form['filter']['match']["glossary_blocking_tags_$format"] = array(
    '#type' => 'textarea',
    '#title' => t('Blocked elements'),
    '#default_value' => variable_get("glossary_blocking_tags_$format", 'abbr acronym'),
    '#cols' => 60,
    '#rows' => 1,
    '#maxlength' => 512,
    '#description' => t('Which HTML elements (tags) should not include Glossary links;
      that is, text within these elements will not be scanned for glossary terms.
      Enter the list separated by a space and do not include < and > characters (e.g. h1 h2).
      To use a %span element to skip text, prefix the class name with a dot (e.g. ".skipping-this").
      All "a," "code," and "pre" elements will be skipped by default.
      ', array('%span' => 'span')),
    );

  $form['filter']['indicator'] = array(
    '#type' => 'fieldset',
    '#title' => t('Link style'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    );

  $form['filter']['indicator']["glossary_link_$format"] = array(
    '#type' => 'radios',
    '#options' => array(
      'none' => t('none'),
      'normal' => t('normal'),
      'absolute' => t('absolute'),
      ),
    '#title' => t('Link type'),
    '#default_value' => variable_get("glossary_link_$format", 'normal'),
    '#description' => t('You may choose no linking of terms ("none"), standard site linking ("normal"), or "absolute" links. RSS feeds need absolute links to ensure they point back to this site. If you are not providing RSS feeds, it is better to choose one of the other types.'),
    '#prefix' => '<div class="glossary_radios">',
    '#suffix' => '</div>',
    );

  $indicator_options = array(
      'superscript' => t('Superscript'),
      'icon' => t('Icon'),
      'abbr' => t('Use !type element', array('!type' => l('abbr', 'http://www.w3.org/TR/html401/struct/text.html#edef-ABBR'))),
      'acronym' => t('Use !type element', array('!type' => l('acronym', 'http://www.w3.org/TR/html401/struct/text.html#edef-ACRONYM'))),
      'cite' => t('Use !type element', array('!type' => l('cite', 'http://www.w3.org/TR/html401/struct/text.html#edef-CITE'))),
      'dfn' => t('Use !type element', array('!type' => l('dfn', 'http://www.w3.org/TR/html401/struct/text.html#edef-DFN'))),
      );

  if (module_exists('hovertip')) {
    $indicator_options['hovertip'] = t('Hovertip');
  }

  $form['filter']['indicator']['glossary_replace'] = array(
    '#type' => 'radios',
    '#title' => t('Term Indicator'),
    '#default_value' => variable_get("glossary_replace_$format", 'superscript'),
    '#options' => $indicator_options,
    '#description' => t('This determines how the link to the glossary term will be indicated. The "phrase" items are linked to the standards in case you want to study them.'),
    '#validate' => array('glossary_indicator_intercept' => array()),
    '#prefix' => '<div class="glossary_radios">',
    '#suffix' => '</div>',
    );

  $form['filter']['indicator']["glossary_superscript"] = array(
    '#type' => 'textfield',
    '#title' => t('Superscript'),
    '#default_value' =>  variable_get("glossary_superscript_$format", 'i'),
    '#size' => 15,
    '#maxlength' => 255,
    '#description' => t('If you chose "superscript" above, enter the superscript text.'),
    '#validate' => array('glossary_indicator_intercept' => array()),
    '#prefix' => '<div class="glossary_superscript">',
    '#suffix' => '</div>',
    );

  $mypath = base_path() . drupal_get_path('module', 'glossary');
  $form['filter']['indicator']["glossary_icon"] = array(
    '#type' => 'textfield',
    '#title' => t('Glossary Icon URL'),
    '#default_value' => variable_get("glossary_icon_$format", $mypath .'/glossary.gif'),
    '#size' => 50,
    '#maxlength' => 255,
    '#description' => t('If you chose "icon" above, enter the URL of the glossary icon relative to the root ("%root") of your Drupal site.', array('%root' => base_path())),
    '#prefix' => '<div class="glossary_icon">',
    '#suffix' => '</div>',
    );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#weight' => 5,
    );

  return $form;
}

function glossary_filter_form_submit($form, &$form_state) {
  $format = $form_state['values']['format'];
  _glossary_clear_cache($format);

  $vids_name = "glossary_vids_$format";
  $link_name = "glossary_link_$format";
  $match_name = "glossary_match_$format";
  $case_name = "glossary_case_$format";
  $replace_all_name = "glossary_replace_all_$format";
  $blocking_tags_name = "glossary_blocking_tags_$format";
  foreach ($form_state['values'][$vids_name] as $vid => $value) {
    if ($value == 0) {
      unset($form_state['values'][$vids_name][$vid]);
    }
  }
  variable_set($vids_name, $form_state['values'][$vids_name]);
  variable_set($link_name, $form_state['values'][$link_name]);
  variable_set($match_name, $form_state['values'][$match_name]);
  variable_set($case_name, $form_state['values'][$case_name]);
  variable_set($replace_all_name, $form_state['values'][$replace_all_name]);
  variable_set($blocking_tags_name, $form_state['values'][$blocking_tags_name]);
  variable_set("glossary_replace_$format", $form_state['values']['glossary_replace']);
  variable_set("glossary_superscript_$format", $form_state['values']["glossary_superscript"]);
  variable_set("glossary_icon_$format", $form_state['values']["glossary_icon"]);
  drupal_set_message(t('Configuration has been updated.'));
   
  return;
}

// Menu call back for clear cache button on settings page.
function glossary_clearcache_form() {
  $form = array();
  $form['text'] = array(
    '#value' => '<p>'. t('This clears only the cache_filter table. To clear all cached data, you may use the button on the <a href="!url">Performance settings</a> page.', array('!url' => url('admin/settings/performance'))) .'</p>',
    );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Clear cache'),
    );
  $form['#submit'][] = '_glossary_clear_cache';

  return $form;  
}

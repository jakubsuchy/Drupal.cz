<?php
// $Id: geshifilter.admin.inc,v 1.19 2008/09/04 09:13:15 soxofaan Exp $

/**
 * @file
 * Implementation of administration functionality.
 */

require_once drupal_get_path('module', 'geshifilter') .'/geshifilter.inc';

/**
 * Form (items) for filter settings.
 */
function _geshifilter_filter_settings($format) {
  $form = array();
  $form['geshifilter'] = array(
    '#type' => 'fieldset',
    '#title' => t('GeSHi filter'),
    '#collapsible' => TRUE,
  );
  if (geshifilter_use_format_specific_options()) {
    // tags and attributes
    $form['geshifilter']['general_tags'] = _geshifilter_general_highlight_tags_settings($format);
    // per language tags
    $form['geshifilter']['per_language_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Per language tags'),
      '#collapsible' => TRUE,
      'table' => geshifilter_per_language_settings($format, 'enabled', FALSE, TRUE),
    );
  }
  else {
    $form['geshifilter']['info'] = array(
      '#value' => '<p>'. t('GeSHi filter is configured to use global tag settings. For separate settings per input format, enable this option in the <a href="!geshi_admin_url">general GeSHi filter settings</a>.', array('!geshi_admin_url' => url('admin/settings/geshifilter'))) .'</p>'
    );
  }
  $form['#validate'][] = 'geshifilter_per_language_settings_validate';
  return $form;
}

/**
 * Menu callback for admin settings
 */
function geshifilter_admin_general_settings(&$form_state) {
  $form = array();

  // try to load GeSHi library and get version if successful
  $geshi_library = _geshifilter_check_geshi_library();

  // GeSHi library settings (constant GESHI_VERSION is defined in GeSHi library)
  $form['geshifilter_library'] = array(
    '#type' => 'fieldset',
    '#title' => defined('GESHI_VERSION') ? t('GeSHi library version @version detected', array('@version' => GESHI_VERSION)) : t('GeSHi library'),
    '#description' => t('The GeSHi filter requires the GeSHi library (which needs to be <a href="!geshi">downloaded</a> and installed seperately).',
      array('!geshi' => url('http://qbnz.com/highlighter/'))),
    '#collapsible' => TRUE,
    '#collapsed' => $geshi_library['success'],
  );
  $form['geshifilter_library']['geshifilter_geshi_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to GeSHi library'),
    '#default_value' => _geshifilter_get_geshi_dir(),
    '#description' => t('Specify the path to the GeSHi library directory (which contains a file called <em>geshi.php</em>).')
  );
  $form['geshifilter_library']['#after_build'] = array('geshifilter_admin_general_settings_after_build');

  if ($geshi_library['loaded']) {

    // GeSHi filter tags and delimiters options
    $form['geshifilter_tag_options'] = array(
      '#type' => 'fieldset',
      '#title' => t('GeSHi filter tags and delimiters'),
      '#collapsible' => TRUE,
    );
    // usage of format specific option
    $form['geshifilter_tag_options']['geshifilter_format_specific_options'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use input format specific tag settings.'),
      '#default_value' => geshifilter_use_format_specific_options(),
      '#description' => t('Enable seperate tag settings of the GeSHi filter for each <a href="!input_formats">input format</a> instead of global tag settings.', array('!input_formats' => url('admin/settings/filters'))),
    );
    // generic tags
    if (!geshifilter_use_format_specific_options()) {
      $form['geshifilter_tag_options']['geshifilter_general_tags'] = _geshifilter_general_highlight_tags_settings();
    }

    // GeSHi filter highlighting options
    $form['geshifilter_highlighting_options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Syntax highlighting options'),
      '#collapsible' => TRUE,
    );
    // default language
    $languages = _geshifilter_get_enabled_languages();
    $form['geshifilter_highlighting_options']['geshifilter_default_highlighting'] = array(
      '#type' => 'select',
      '#title' => t('Default highlighting mode'),
      '#default_value' => variable_get('geshifilter_default_highlighting', GESHIFILTER_DEFAULT_PLAINTEXT),
      '#options' => array(
        t('No highlighting') => array(
          GESHIFILTER_DEFAULT_DONOTHING => t('Do nothing'),
          GESHIFILTER_DEFAULT_PLAINTEXT => t('As plain text'),
          ),
        t('Languages') => $languages,
      ),
      '#description' => t('Select the default highlighting mode to use when no language is defined with a language attribute in the tag.')
    );
    // Default line numbering scheme
    $form['geshifilter_highlighting_options']['geshifilter_default_line_numbering'] = array(
      '#type' => 'select',
      '#title' => t('Default line numbering'),
      '#default_value' => variable_get('geshifilter_default_line_numbering', GESHIFILTER_LINE_NUMBERS_DEFAULT_NONE),
      '#options' => array(
        GESHIFILTER_LINE_NUMBERS_DEFAULT_NONE => t('no line numbers'),
        GESHIFILTER_LINE_NUMBERS_DEFAULT_NORMAL => t('normal line numbers'),
        GESHIFILTER_LINE_NUMBERS_DEFAULT_FANCY5 => t('fancy line numbers (every @n lines)', array('@n' => GESHIFILTER_LINE_NUMBERS_DEFAULT_FANCY5)),
        GESHIFILTER_LINE_NUMBERS_DEFAULT_FANCY10 => t('fancy line numbers (every @n lines)', array('@n' => GESHIFILTER_LINE_NUMBERS_DEFAULT_FANCY10)),
        GESHIFILTER_LINE_NUMBERS_DEFAULT_FANCY20 => t('fancy line numbers (every @n lines)', array('@n' => GESHIFILTER_LINE_NUMBERS_DEFAULT_FANCY20)),
      ),
      '#description' => t('Select the default line numbering scheme: no line numbers, normal line numbers or fancy line numbers. With fancy line numbers every n<sup>th</sup> line number is highlighted. (GeSHi documentation: !link).',
        array('!link' => l('Line numbers', 'http://qbnz.com/highlighter/geshi-doc.html#line-numbers'))),
    );
    // highlight_string usage option
    $form['geshifilter_highlighting_options']['geshifilter_use_highlight_string_for_php'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use built-in PHP function <code>highlight_string()</code> for PHP source code.'),
      '#description' => t('When enabled, PHP source code will be syntax highlighted with the built-in PHP function <code>!highlight_string</code> instead of with the GeSHi library. GeSHi features like for example line numbering and usage of an external CSS stylesheet are not available.', array('!highlight_string' => l('highlight_string()', 'http://php.net/manual/en/function.highlight-string.php'))),
      '#default_value' => variable_get('geshifilter_use_highlight_string_for_php', FALSE),
    );
    // Option to disable Keyword URL's
    $form['geshifilter_highlighting_options']['geshifilter_enable_keyword_urls'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable GeSHi keyword URLs'),
      '#description' => t('For some languages GeSHi can link language keywords (e.g. standard library functions) to their online documentation. (GeSHi documentation: !link)',
        array('!link' => l('Keyword URLs', 'http://qbnz.com/highlighter/geshi-doc.html#keyword-urls'))),
      '#default_value' => variable_get('geshifilter_enable_keyword_urls', TRUE),
    );

    // Styling, layout and CSS
    $form['geshifilter_styling'] = array(
      '#type' => 'fieldset',
      '#title' => t('Styling, layout and CSS'),
      '#collapsible' => TRUE,
    );

    // CSS mode
    $form['geshifilter_styling']['geshifilter_css_mode'] = array(
      '#type' => 'radios',
      '#title' => t('CSS mode for syntax highlighting'),
      '#options' => array(
        GESHIFILTER_CSS_INLINE => t('Inline CSS style attributes.'),
        GESHIFILTER_CSS_CLASSES_AUTOMATIC => t('Use CSS classes and an automatically managed external CSS style sheet.'),
        GESHIFILTER_CSS_CLASSES_ONLY => t('Only add CSS classes to the markup.'),
      ),
      '#default_value' => variable_get('geshifilter_css_mode', GESHIFILTER_CSS_INLINE),
      '#description' => t('Inline CSS is easier to set up, does not depend on an external style sheets and is consequently more robust to copy/paste operations like content aggregation. However, usage of CSS classes and an external stylesheet requires much less markup code and bandwidth. The external style sheet can be managed automatically by the GeSHi filter module, but this feature requires the public <a href="!filesystem">download method</a>. If the GeSHi filter is configured to only add the CSS classes to the markup, the administrator or themer is responsible for adding the appropriate CSS rules to the pages (e.g. based on <a href="!cssdefaults">these defaults</a>). (GeSHi documentation: !geshidoc).',
        array(
          '!geshidoc' => l('Using CSS Classes', 'http://qbnz.com/highlighter/geshi-doc.html#using-css-classes'),
          '!filesystem' => url('admin/settings/file-system'),
          '!cssdefaults' => url('admin/settings/geshifilter/generate_css'),
        )),
    );

    // Code container
    $form['geshifilter_styling']['geshifilter_code_container'] = array(
      '#type' => 'radios',
      '#title' => t('Code container'),
      '#description' => t('Define the container element to use for code blocks. (GeSHi documentation: !link).',
        array('!link' => l('The Code Container', 'http://qbnz.com/highlighter/geshi-doc.html#the-code-container'))
      ),
      '#options' => array(
        GESHI_HEADER_PRE => t('Use @cnt container (efficient whitespace coding, no automatic line wrapping)', array('@cnt' => '<pre>')),
        GESHI_HEADER_DIV => t('Use @cnt container (enables automatic line wrapping)', array('@cnt' => '<div>')),
        GESHI_HEADER_NONE => t('Use no container'),
      ),
      '#default_value' => variable_get('geshifilter_code_container', GESHI_HEADER_PRE),
    );
  }
  // Make it a system settings form.
  $form  = system_settings_form($form);
  // Set custom validate/submit handlers (to be run after system_settings_form submit).
  $form['#validate'][] = 'geshifilter_admin_general_settings_validate';
  $form['#submit'][] = 'geshifilter_admin_general_settings_submit';
  return $form;
}

/**
 * After build function for admin settings
 */
function geshifilter_admin_general_settings_after_build($form, &$form_state) {
  $geshi_library = _geshifilter_check_geshi_library(FALSE, $form_state['values']['geshifilter_geshi_dir'], FALSE);
  if (!$geshi_library['success']) {
    form_set_error('geshifilter_geshi_dir', $geshi_library['message']);
  }
  return $form;
}

/**
 * Validate function for admin settings
 */
function geshifilter_admin_general_settings_validate($form, &$form_state) {
  // Check if automatically managed style sheet is posible
  if ($form_state['values']['geshifilter_css_mode'] == GESHIFILTER_CSS_CLASSES_AUTOMATIC && !_geshifilter_managed_external_stylesheet_possible()) {
    form_set_error('geshifilter_css_mode', t('GeSHi filter can not automatically manage an external CSS style sheet when the download method is private.'));
  }
}

/**
 * Submit function for admin settings
 */
function geshifilter_admin_general_settings_submit($form, &$form_state) {
  // Cache of available languages should be cleared when GeSHi library dir changes
  // _geshifilter_get_geshi_dir() returns the new value set by
  // system_settings_form_submit() which runs before
  // geshifilter_admin_general_settings_submit(),
  // $form['geshifilter_library']['geshifilter_geshi_dir']['#default_value'] is
  // the old value
  if (_geshifilter_get_geshi_dir() != $form['geshifilter_library']['geshifilter_geshi_dir']['#default_value']) {
    variable_del('geshifilter_available_languages_cache');
  }
  // Regenerate language_css
  // Note the use of variable_get('geshifilter_css_mode') which is alread set
  // to $form_state['values']['geshifilter_css_mode'] by
  // system_settings_form_submit, which is run before this submit handler.
  if (variable_get('geshifilter_css_mode', GESHIFILTER_CSS_INLINE) == GESHIFILTER_CSS_CLASSES_AUTOMATIC) {
    _geshifilter_generate_languages_css_file();
  }
  _geshifilter_clear_filter_cache();
}

/**
 * Menu callback for per language settings
 */
function geshifilter_admin_per_language_settings($form_state, $view = 'enabled') {
  $form = array();
  // check if GeSHi library is available
  $geshi_library = _geshifilter_check_geshi_library();
  if (!$geshi_library['success']) {
    drupal_set_message($geshi_library['message'], 'error');
    return;
  }
  $add_checkbox = TRUE;
  $add_tag_option = (!geshifilter_use_format_specific_options());
  $form['language_settings'] = geshifilter_per_language_settings(NULL, $view, $add_checkbox, $add_tag_option);
  $form['rescan_languages'] = array(
    '#type' => 'markup',
    '#value' => '<p>'. l(t('Flush cache of available languages'), 'geshifilter/clearavailablelanguagescache', array('query' => drupal_get_destination())) .'</p>',
  );
  // Make it a system settings form.
  $form = system_settings_form($form);
  // set custom validate and submit handlers
  $form['#validate'][] = 'geshifilter_per_language_settings_validate';
  $form['#submit'][] = 'geshifilter_admin_per_language_settings_submit';
  return $form;
}

/**
 * Validate function for per language settings
 *
 * Check that language tags differ between languages and fro the generic tags.
 */
function geshifilter_per_language_settings_validate($form, &$form_state) {

  // if we're coming from the _geshifilter_filter_settings (sub)form, we should
  // take the input format into account
  $format = isset($form_state['values']['format']) ? $form_state['values']['format'] : NULL;
  $f = ($format === NULL) ? '' : "_$format";

  // language tags should differ from each other
  $languages = _geshifilter_get_available_languages();
  foreach ($languages as $language1 => $language_data1) {
    // iterate over the enabled languages: skip disabled ones
    $field = "geshifilter_language_enabled_{$language1}";
    if (!(isset($form_state['values'][$field]) ? $form_state['values'][$field] : variable_get($field, FALSE))) {
      continue;
    }
    // get the associated tags as $tags1
    $field = "geshifilter_language_tags_{$language1}{$f}";
    if (isset($form_state['values'][$field])) {
      $tags1 = _geshifilter_tag_split($form_state['values'][$field]);
    }
    else {
      $tags1 = _geshifilter_tag_split(geshifilter_language_tags($language1, $format));
    }
    // also include the generic tags
    $field = "geshifilter_tags{$f}";
    $generic_tags = isset($form_state['values'][$field]) ? $form_state['values'][$field] : geshifilter_tags($format);
    $tags1 = array_merge($tags1, _geshifilter_tag_split($generic_tags));

    // check that other languages do not use these tags
    foreach ($languages as $language2 => $language_data2) {
      // check these tags against the tags of other enabled languages
      $field = "geshifilter_language_enabled_{$language2}";
      if ($language1 == $language2 || !(isset($form_state['values'][$field]) ? $form_state['values'][$field] : variable_get($field, FALSE))) {
        continue;
      }
      // get tags for $language2, or skip when not in $form_state['values']
      $field = "geshifilter_language_tags_{$language2}{$f}";
      if (isset($form_state['values'][$field])) {
        $tags2 = _geshifilter_tag_split($form_state['values'][$field]);
      }
      else {
        continue;
      }
      // and now we can check tags1 against tags2
      foreach ($tags1 as $tag1) {
        foreach ($tags2 as $tag2) {
          if ($tag1 == $tag2) {
            form_set_error("geshifilter_language_tags_{$language2}{$f}",  t('The language tags should differ between languages and from the generic tags.'));
          }
        }
      }
    }
  }
}

/**
 * Submit function for per language settings
 */
function geshifilter_admin_per_language_settings_submit($form, &$form_state) {
  // regenerate language_css
  if (variable_get('geshifilter_css_mode', GESHIFILTER_CSS_INLINE) == GESHIFILTER_CSS_CLASSES_AUTOMATIC) {
    _geshifilter_generate_languages_css_file();
  }
  _geshifilter_clear_filter_cache();
}

/**
 * Helper function for some settings form fields usable as general and specific settings
 */
function _geshifilter_general_highlight_tags_settings($format=NULL) {
  $form = array();
  $f = ($format === NULL) ? '' : "_$format";
  // generic tags
  $form["geshifilter_tags$f"] = array(
    '#type' => 'textfield',
    '#title' => t('Generic syntax highlighting tags'),
    '#default_value' => geshifilter_tags($format),
    '#description' => t('Tags that should activate the GeSHi syntax highlighting. Specify a space-separated list of tagnames.'),
  );
  // tag style
  $form["geshifilter_brackets$f"] = array(
    '#type' => 'select',
    '#title' => t('Tag style'),
    '#options' => array(
      GESHIFILTER_BRACKETS_ANGLE => '<foo>',
      GESHIFILTER_BRACKETS_SQUARE => '[foo]',
      GESHIFILTER_BRACKETS_BOTH => t('!angle or !square', array('!angle' => '<foo>', '!square' => '[foo]')),
    ),
    '#default_value' => _geshifilter_brackets($format),
    '#description' => t('Select which brackets should be used for the source code container tags.'),
  );
  // PHP specific delimiters
  $form['geshifilter']["geshifilter_enable_php_delimiters$f"] = array(
    '#type' => 'checkbox',
    '#title' => t('Also apply syntax highlighting to &lt;?php ... ?&gt; style PHP source code blocks.'),
    '#description' => t('When enabled, PHP source code enclosed in &lt;?php...?&gt; or &lt;%...%&gt; will also be processed.'),
    '#default_value' => _geshifilter_php_delimeters($format),
  );
  return $form;
}

/**
 * function for generating a form table for per language settings
 */
function geshifilter_per_language_settings($format, $view, $add_checkbox, $add_tag_option) {
  $form = array(
    '#theme' => 'geshifilter_per_language_settings',
  );
  // table header
  $form['header'] = array(
    '#type' => 'value',
    '#value' => array(),
  );
  $form['header']['#value'][] = t('Language');
  $form['header']['#value'][] = t('GeSHi language code');
  if ($add_tag_option) {
    $form['header']['#value'][] = t('Tag/language attribute value');
  }
  // table body
  $form['languages'] = array();
  $languages = _geshifilter_get_available_languages();
  foreach ($languages as $language => $language_data) {
    $enabled = variable_get("geshifilter_language_enabled_{$language}", FALSE);
    // skip items to hide
    if (($view == 'enabled' && !$enabled) || ($view == 'disabled' && $enabled)) {
      continue;
    }
    // build language row
    $form['languages'][$language] = array();
    // add enable/disable checkbox
    if ($add_checkbox) {
      $form['languages'][$language]["geshifilter_language_enabled_{$language}"] = array(
        '#type' => 'checkbox',
        '#default_value' => $enabled,
        '#title' => $language_data['fullname'],
      );
    }
    else {
      $form['languages'][$language]['fullname'] = array(
        '#type' => 'markup',
        '#value' => $language_data['fullname'],
      );
    }
    // language code
    $form['languages'][$language]['name'] = array(
      '#type' => 'markup',
      '#value' => $language,
    );
    // add a textfield for tags
    if ($add_tag_option) {
      $f = ($format === NULL) ? '' : "_$format";
      $form['languages'][$language]["geshifilter_language_tags_{$language}{$f}"] = array(
        '#type' => 'textfield',
        '#default_value' => geshifilter_language_tags($language, $format),
        '#size' => 20,
      );
    }
  }
  return $form;
}

/**
 * theming function for the per language settings table
 */
function theme_geshifilter_per_language_settings($form) {
  // build table body
  $rows = array();
  foreach (element_children($form['languages']) as $language) {
    $row = array();
    foreach (element_children($form['languages'][$language]) as $key) {
      $row[] = drupal_render($form['languages'][$language][$key]);
    }
    $rows[] = $row;
  }
  // build table header
  $header = array();
  foreach ($form['header']['#value'] as $cell) {
    if ($cell == t('Enabled')) {
      // add JavaScript 'select all/select none' stuff
      $header[] = array('data' => $cell) + theme('table_select_header_cell');
    }
    else {
      $header[] = $cell;
    }
  }
  // return themed table
  return theme('table', $header, $rows);
}

/**
 * Helper function for clearing the appropriate filter cache entries
 */
function _geshifilter_clear_filter_cache() {
  $message = FALSE;
  // iterate over input formats
  foreach (filter_formats() as $format => $input_format) {
    // Get the filters in this input format
    $filters = filter_list_format($format);
    // look if GeSHi is enabled in this input format
    if (isset($filters['geshifilter/0'])) {
      cache_clear_all($format .':', 'cache_filter', TRUE);
      $message = TRUE;
    }
  }
  // show a message that the filter cache was cleared
  if ($message) {
    drupal_set_message(t('GeSHi filter cleared the filter cache for the appropriate input formats.'));
  }
}

/**
 * callback function for clearing the available languages cache
 */
function geshifilter_clear_available_languages_cache() {
  variable_del('geshifilter_available_languages_cache');
  drupal_set_message('Flushed cache of available languages.');
  drupal_goto();
}

/**
 * Helper function for generating the CSS rules
 *
 * @return string with the CSS rules
 */
function _geshifilter_generate_languages_css_rules() {
  $output = '';
  $geshi_library = _geshifilter_check_geshi_library();
  if ($geshi_library['loaded']) {
    require_once drupal_get_path('module', 'geshifilter') .'/geshifilter.pages.inc';
    $languages = _geshifilter_get_enabled_languages();
    foreach ($languages as $langcode => $language_full_name) {
      // create GeSHi object
      $geshi = _geshifilter_GeSHi_factory('', $langcode);
      _geshifilter_override_geshi_defaults($geshi, $langcode);
      // add CSS rules for current language
      $output .= $geshi->get_stylesheet(FALSE) ."\n";
      // release GeSHi object
      unset($geshi);
    }
  }
  else {
    drupal_set_message(t('Error while generating CSS rules: could not load GeSHi library.'), 'error');
  }
  return $output;
}

/**
 * Function for generating the external stylesheet.
 */
function _geshifilter_generate_languages_css_file() {
  $languages = _geshifilter_get_enabled_languages();
  // create files/geshifilter directory if not already available
  $path = file_directory_path() .'/geshifilter';
  file_check_directory($path, FILE_CREATE_DIRECTORY);
  // Make stylesheet file name (depends on enabled languages).
  $stylesheet_file = $path .'/geshifilter-languages-'. substr(md5(serialize($languages)), 0, 8) .'.css';
  if (!file_exists($stylesheet_file)) {
    // build stylesheet
    $stylesheet = _geshifilter_generate_languages_css_rules();
    // save stylesheet
    $ret = file_save_data($stylesheet, $stylesheet_file, FILE_EXISTS_REPLACE);
    if ($ret) {
      variable_set('geshifilter_languages_css', $ret);
      drupal_set_message(t('Generated external CSS file %file.', array('%file' => $ret)));
    }
    else {
      drupal_set_message(t('Could not generate external CSS file. Check the settings of your !filesystem.',
        array('!filesystem' => l(t('file system'), 'admin/settings/file-system'))), 'error');
    }
  }
}

<?php
// $Id: texy.admin.inc,v 1.1.2.7 2009/04/27 17:43:24 havran Exp $

/**
 * @file
 * Admin page callbacks for the Texy module.
 */

// PHP4/PHP5 compatibility
if ( version_compare('5.0.0', PHP_VERSION, '>') ) {
  require_once 'texy.php4.inc';
}
else {
  require_once 'texy.php5.inc';
}

/**
 * Form builder; Configure main Texy settings.
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function texy_form_main_settings() {
  // for Texy! version
  $texy =& _texy_get_object(TRUE);
  

  $form['basicinfo'] = array(
    '#type' => 'fieldset',
    '#weight' => -11,
  );
  $form['basicinfo']['info'] = array(
    '#type' => 'markup',
    '#value' => t(
      '<strong>PHP version:</strong> @php_version<br />
<strong>Texy! version:</strong> @texy_version',
      array(
        '@php_version' => PHP_VERSION,
        '@texy_version' => _get_texy_version(),
      )
    ),
  );

  $form['basicset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -10,
  );
  $form['basicset']['texy_heading_base'] = array(
    '#type' => 'select',
    '#title' => t('Highest heading level'),
    '#options' => array(1 => '<h1>', 2 => '<h2>', 3 => '<h3>', 4 => '<h4>', 5 => '<h5>', 6 => '<h6>'),
    '#description' => t('Highest allowed heading level. Default is &lt;h2&gt;. Setting a higher level is not recommended.'),
    '#default_value' => variable_get('texy_heading_base', 3),
  );
  $form['basicset']['texy_line_wrap'] = array(
    '#type' => 'textfield',
    '#size' => '3',
    '#title' => t('Maximum number of characters per line'),
    '#description' => t('Enter desired number of characters per line or "0" to disable line wrapping (default).'),
    '#default_value' => variable_get('texy_line_wrap', 0),
  );
  // next two elements are connected in theme function
  $form['basicset']['texy_allowed_longwords'] = array(
    '#type' => 'checkbox',
    '#title' => t('Break words longer than'),
    '#default_value' => variable_get('texy_allowed_longwords', FALSE),
  );
  $form['basicset']['texy_word_limit'] = array(
    '#type' => 'textfield',
    '#size' => '3',
    '#title' => t('Break long words'),
    '#description' => t('If enabled, Texy! breaks long words with inserting <code>&amp;shy;</code> entity (as invisible character). (Warning! This may interfere with some modules that are use <code>[someword]</code> like filter syntax. (For example <a href="@inline-url">Inline</a> module.) Disabled by default.', array('@inline-url' => url('http://drupal.org/project/inline'))),
    '#default_value' => variable_get('texy_word_limit', 20),
  );
  // end
  $form['basicset']['texy_obfuscate_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('E-mail address obfuscation'),
    '#description' => t('Obfuscate emails from spambots? Enabled by default.'),
    '#default_value' => variable_get('texy_obfuscate_email', TRUE),
  );
  $form['basicset']['texy_force_nofollow'] = array(
    '#type' => 'checkbox',
    '#title' => t('Nofollow attribute'),
    '#description' => t('Applies the <i>rel="nofollow"</i> attribute to all external links. Disabled by default.'),
    '#default_value' => variable_get('texy_force_nofollow', FALSE),
  );
  $form['basicset']['texy_locale'] = array(
    '#type' => 'select',
    '#title' => t('Locale to use'),
    '#options' => array('en' => 'English', 'cs' => 'Czech/Slovak', 'fr' => 'French', 'de' => 'German', 'pl' => 'Polish'),
    '#description' => t('Locale settings to use.  English by default.'),
    '#default_value' => variable_get('texy_locale', 'en'),
  );

  $form['cssset'] = array(
    '#type' => 'fieldset',
    '#title' => t('CSS settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -9,
  );
  $form['cssset']['texy_allowed_classes'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed classes'),
    '#description' => t('Allow following CSS classes and IDs to appear in the text. Default value is empty - allow no classes. Specify <i>&lt;all&gt;</i> to allow all classes (not recommended). Use commas as separators.'),
    '#default_value' => variable_get('texy_allowed_classes', ''),
  );
  $form['cssset']['texy_allowed_styles'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed inline styles'),
    '#description' => t('Allow following CSS inline styles such as \'font-size\', \'color\', to appear in the text. Default value is empty - allow no styles. Specify <i>&lt;all&gt;</i> to allow all styles (not recommended). Use commas as separators.'),
    '#default_value' => variable_get('texy_allowed_styles', ''),
  );

  $form['tagset'] = array(
    '#type' => 'fieldset',
    '#title' => t('HTML tags settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -8,
  );
  $form['tagset']['texy_allowed_tags'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed tags'),
    '#description' => t('Allow following tags to appear in the text. Default value is empty - allow no tags. Specify <i>&lt;all&gt;</i> to allow all tags (not recommended). Use commas as separators.'),
    '#default_value' => variable_get('texy_allowed_tags', ''),
  );
  $form['tagset']['texy_phrase_sup'] = array(
    '#type' => 'checkbox',
    '#title' => t('Superscript'),
    '#description' => t('Use <i>text^^superscript^^</i> syntax for text<sup>superscript</sup>. Enabled by default.'),
    '#default_value' => variable_get('texy_phrase_sup', TRUE),
  );
  $form['tagset']['texy_phrase_sup_alt'] = array(
    '#type' => 'checkbox',
    '#title' => t('Superscript - alternative syntax for numbers only'),
    '#description' => t('Use <i>text^2</i> syntax for text<sup>2</sup>. Enabled by default.'),
    '#default_value' => variable_get('texy_phrase_sup_alt', TRUE),
  );
  $form['tagset']['texy_phrase_sub'] = array(
    '#type' => 'checkbox',
    '#title' => t('Subscript'),
    '#description' => t('Use <i>text__subscript__</i> syntax for text<sub>subscript</sub>. Enabled by default.'),
    '#default_value' => variable_get('texy_phrase_sub', TRUE),
  );
  $form['tagset']['texy_phrase_sub_alt'] = array(
    '#type' => 'checkbox',
    '#title' => t('Subscript - alternative syntax for numbers only'),
    '#description' => t('Use <i>text_2</i> syntax for text<sub>2</sub>. Enabled by default.'),
    '#default_value' => variable_get('texy_phrase_sub_alt', TRUE),
  );

  $form = system_settings_form($form);
  unset($form['#theme']); // because system_settings_form() set his own theme
  return $form;
}

function theme_texy_form_main_settings($form) {
  // we need this element
  $my_output = drupal_render($form['basicset']['texy_allowed_longwords']);
  // get all between beginning <div...> and ending </div>
  // but if is altered theme_form_element($element, $value) function? who know...
  preg_match('@<(div+)([^>]*?)>(.*?)(</\1>)@si', $my_output, $match);
  // and now we have only form element itself (in $match[3])
  $form['basicset']['texy_word_limit']['#field_prefix'] = $match[3];
  $form['basicset']['texy_word_limit']['#field_suffix'] = t('characters');

  return drupal_render($form);
}


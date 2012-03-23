<?php
// $Id: ascii_art_captcha.admin.inc,v 1.2 2007/12/22 15:24:31 soxofaan Exp $

/**
 * Function for the settings form
 */
function ascii_art_captcha_settings_form() {
  $form = array();
  $available_fonts = _ascii_art_captcha_available_fonts();
  $form['ascii_art_captcha_code_length'] = array(
    '#type' => 'select',
    '#title' => t('Code length'),
    '#options' => array(4 => 4, 5 => 5, 6 => 6, 7 => 7, 8 => 8, 9 => 9, 10 => 10),
    '#default_value' => (int) variable_get('ascii_art_captcha_code_length', 6),
  );
  $form['ascii_art_captcha_font'] = array(
    '#type' => 'select',
    '#title' => t('Font'),
    '#options' => $available_fonts,
    '#default_value' => variable_get('ascii_art_captcha_font', 'standard'),
    '#description' => t('Define the ASCII art font to use. Note that some characters are not very recognizable in some (small/weird) fonts. Make sure to disable the right character sets in these cases.'),
  );
  // font size
  $font_sizes = array(0 => t('default'));
  foreach (array(4, 6, 8, 9, 10, 11, 12) as $pt) {
    $font_sizes[$pt] = $pt .'pt';
  }
  $form['ascii_art_captcha_font_size'] = array(
    '#type' => 'select',
    '#title' => t('Font size'),
    '#options' => $font_sizes,
    '#default_value' => variable_get('ascii_art_captcha_font_size', 0),
    '#description' => t('Set the font size for the ASCII art.'),
  );
  $form['ascii_art_captcha_allowed_characters'] = array(
    '#type' => 'checkboxes',
    '#title' => 'Character sets to use',
    '#options' => array(
      'upper' => t('upper case characters'),
      'lower' => t('lower case characters'),
      'digit' => t('digits'),
    ),
    '#default_value' => variable_get('ascii_art_captcha_allowed_characters', array('upper' => 'upper', 'lower' => 'lower', 'digit' => 'digit')),
    '#description' => t('Enable the character sets to use in the code. Choose wisely by taking the recognizability of the used font into account.'),
  );
  $form['#validate'] = array('ascii_art_captcha_settings_form_validate');
  return system_settings_form($form);
}

/**
 * Validation function for the settings
 */
function ascii_art_captcha_settings_form_validate($form, &$form_state) {
  if (count(array_filter($form_state['values']['ascii_art_captcha_allowed_characters'])) < 1) {
    form_set_error('ascii_art_captcha_allowed_characters', t('You should select at least one type of characters to use.'));
  }
}

/**
 * function to get a list of available fonts
 */
function _ascii_art_captcha_available_fonts() {
  $available_fonts = array();
  $fontsdirectory = drupal_get_path('module', 'ascii_art_captcha') .'/fonts';
  $pattern = 'ascii_art_captcha_font_([a-zA-Z0-9]+)\.[iI][nN][cC]$';
  foreach (file_scan_directory($fontsdirectory, $pattern) as $filename => $font) {
    $regs = array();
    ereg($pattern, $font->basename, $regs);
    $available_fonts[$regs[1]] = $regs[1];
  }
  asort($available_fonts);
  return $available_fonts;
}

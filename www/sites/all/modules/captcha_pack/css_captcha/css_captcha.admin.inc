<?php
// $Id: css_captcha.admin.inc,v 1.1 2007/12/22 14:10:43 soxofaan Exp $

/**
 * Function for the settings form
 */
function css_captcha_settings_form() {
  $form = array();
  $form['css_captcha_allowed_characters'] = array(
    '#type' => 'textfield',
    '#title' => t('Characters to use in the code'),
    '#default_value' => variable_get('css_captcha_allowed_characters', CSS_CAPTCHA_DEFAULT_ALLOWED_CHARACTERS),
  );
  $form['css_captcha_code_length'] = array(
    '#type' => 'select',
    '#title' => t('Code length'),
    '#options' => array(4 => 4, 5 => 5, 6 => 6, 7 => 7, 8 => 8, 9 => 9, 10 => 10),
    '#default_value' => (int) variable_get('css_captcha_code_length', 6),
  );
  $form['#validate'] = array('css_captcha_settings_form_validate');
  return system_settings_form($form);
}

/**
 * Validation function
 */
function css_captcha_settings_form_validate($form, &$form_state) {
  if (preg_match('/\s/', $form_state['values']['css_captcha_allowed_characters'])) {
    form_set_error('css_captcha_allowed_characters', t('The list of characters to use should not contain spaces.'));
  }
}

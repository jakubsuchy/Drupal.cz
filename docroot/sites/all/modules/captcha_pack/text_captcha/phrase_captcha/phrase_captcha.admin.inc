<?php
// $Id: phrase_captcha.admin.inc,v 1.1 2008/02/13 21:51:51 soxofaan Exp $

require_once('phrase_captcha.inc');

/**
 * Administration form
 */
function phrase_captcha_settings_form() {
  drupal_set_message(t('WARNING: this module is not completely ported to Drupal 6 and does not work yet.'), 'warning');

  $form = array();
  // radio buttons for selecting the kind of words to use
  $form['phrase_captcha_words'] = array(
    '#type' => 'radios',
    '#title' => t('Kind of words to use in the CAPTCHA phrase'),
    '#options' => array(
      PHRASE_CAPTCHA_GENERATE_NONSENSE_WORDS => t('Generate nonsense words'),
      PHRASE_CAPTCHA_USER_DEFINED_WORDS => t('Use user defined words')
    ),
    '#default_value' => variable_get('phrase_captcha_words', PHRASE_CAPTCHA_GENERATE_NONSENSE_WORDS),
    '#required' => TRUE,
  );
  // form elements for the word pools
  _text_captcha_word_pool_form_items($form,
    'phrase_captcha_userdefined_word_pool', t('User defined word pool'),
    t('Enter the words to use in the CAPTCHA phrase (space separated, no punctuation).'),
    ''
  );
  // select form element for the number of words in the CAPTCHA phrase
  $form['phrase_captcha_word_quantity'] = array(
    '#type' => 'select',
    '#title' => t('Number of words in the CAPTCHA phrase'),
    '#default_value' => (int) variable_get('phrase_captcha_word_quantity', 5),
    '#options' => array(4 => 4, 5 => 5, 6 => 6, 7 => 7, 8 => 8, 9 => 9, 10 => 10),
    '#required' => TRUE,
  );
  // select form element for the number of additional words
  $form['phrase_captcha_additional_word_quantity'] = array(
    '#type' => 'select',
    '#title' => t('Maximum number of additional words to let the user choose from'),
    '#default_value' => (int) variable_get('phrase_captcha_additional_word_quantity', 1),
    '#options' => array(0 => 0, 1 => 1, 2 => 2, 3 => 3, 4 => 4, 5 => 5),
    '#required' => TRUE,
  );
  $form['phrase_captcha_word_selection_challenges'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Word selection challenges'),
    '#options' => _phrase_captcha_available_word_challenges(),
    '#default_value' => _phrase_captcha_enabled_word_challenges(),
  );
  // add additional validation handler
  $form['#validate'][] = 'phrase_captcha_settings_form_validate';

  return system_settings_form($form);
}

/**
 * Validate function of the administration form
 */
function phrase_captcha_settings_form_validate($form, &$form_state) {
  if ($form_state['values']['phrase_captcha_words'] == PHRASE_CAPTCHA_USER_DEFINED_WORDS) {
    $word_count_minimum = (int) $form_state['values']['phrase_captcha_word_quantity'] + (int) $form_state['values']['phrase_captcha_additional_word_quantity'] + 2;
    _text_captcha_word_pool_validate('phrase_captcha_userdefined_word_pool', $form_state['values'], $word_count_minimum, NULL, NULL);
  }
  // check word selection
  if (count(array_filter($form_state['values']['phrase_captcha_word_selection_challenges'])) < 1) {
    form_set_error('phrase_captcha_word_selection_challenges', t('You need to select at least one word selection criterium'));
  }
}

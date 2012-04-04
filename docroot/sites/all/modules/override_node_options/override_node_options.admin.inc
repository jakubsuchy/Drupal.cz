<?php

/**
 * @file
 * Admin page callbacks for the override_node_options module.
 */

/**
 * Administration settings form.
 *
 * @see system_settings_form()
 */
function override_node_options_settings_form() {
  $form['attachments'] = array(
    '#type' => 'fieldset',
    '#title' => t('File attachments'),
    '#description' => t('Options for the File attachments fieldset on the node form.'),
    '#access' => module_exists('upload'),
  );
  $form['attachments']['override_node_options_facollapsible'] = array(
    '#type' => 'checkbox',
    '#title' => t('File attachments collapsible'),
    '#default_value' => variable_get('override_node_options_facollapsible', 1),
  );
  $form['attachments']['override_node_options_facollapsed'] = array(
    '#type' => 'checkbox',
    '#title' => t('File attachments collapsed by default'),
    '#default_value' => variable_get('override_node_options_facollapsed', 1),
  );

  $form['scheduler_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Scheduler settings'),
    '#description' => t('Options for the Scheduler settings fieldset on the node form.'),
    '#access' => module_exists('scheduler'),
  );
  $form['scheduler_settings']['override_node_options_sscollapsible'] = array(
    '#type' => 'checkbox',
    '#title' => t('Scheduler settings collapsible'),
    '#default_value' => variable_get('override_node_options_sscollapsible', 1),
  );
  $form['scheduler_settings']['override_node_options_sscollapsed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Scheduler settings collapsed by default'),
    '#default_value' => variable_get('override_node_options_sscollapsed', 1),
  );

  $form['authoring_information'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authoring information'),
    '#description' => t('Options for the Authoring information fieldset on the node form.'),
  );
  $form['authoring_information']['override_node_options_aicollapsible'] = array(
    '#type' => 'checkbox',
    '#title' => t('Authoring information collapsible'),
    '#default_value' => variable_get('override_node_options_aicollapsible', 1),
  );
  $form['authoring_information']['override_node_options_aicollapsed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Authoring information collapsed by default'),
    '#default_value' => variable_get('override_node_options_aicollapsed', 1),
  );

  $form['publishing_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Publishing options'),
    '#description' => t('Options for the Publishing options fieldset on the node form.'),
  );
  $form['publishing_options']['override_node_options_pocollapsible'] = array(
    '#type' => 'checkbox',
    '#title' => t('Publishing options collapsible'),
    '#default_value' => variable_get('override_node_options_pocollapsible', 1),
  );
  $form['publishing_options']['override_node_options_pocollapsed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Publishing options collapsed by default'),
    '#default_value' => variable_get('override_node_options_pocollapsed', 1),
  );

  return system_settings_form($form);
}

<?php
// $Id: texy_image.admin.inc,v 1.1.2.2 2008/02/17 18:01:55 havran Exp $

/**
 * @file
 * Admin page callbacks for the Texy Image.
 */

/**
 * Form builder; Configure Image settings for Texy!.
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function texy_form_image_settings() {
  $form['imageset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => -1,
  );
  $form['imageset']['texy_image_dir'] = array(
    '#type' => 'textfield',
    '#size' => '12',
    '#title' => t('Directory'),
    '#description' => t('Subdirectory of the <code>%dir</code> directory where the pictures are stored. Do not include trailing slash.', array('%dir' => base_path().variable_get('file_directory_path', 'files'))),
    '#default_value' => variable_get('texy_image_dir', 'images'),
    '#field_prefix' => base_path() . file_directory_path() .'/',
  );
  $form['imageset']['texy_image_class_left'] = array(
    '#type' => 'textfield',
    '#size' => '12',
    '#title' => t('Class for images aligned to the left'),
    '#description' => t('Name for a class which will be assigned to all images aligned to the left.'),
    '#default_value' => variable_get('texy_image_class_left', 'left'),
  );
  $form['imageset']['texy_image_class_right'] = array(
    '#type' => 'textfield',
    '#size' => '12',
    '#title' => t('Class for images aligned to the right'),
    '#description' => t('Name for a class which will be assigned to all images aligned to the right.'),
    '#default_value' => variable_get('texy_image_class_right', 'right'),
  );
  $form['imageset']['texy_image_alt_text'] = array(
    '#type' => 'textfield',
    '#size' => '12',
    '#title' => t('Default alternate text'),
    '#description' => t('Default value for the <i>alt</i> attribute. Leave this field blank to specify no alternate text for images.'),
    '#default_value' => variable_get('texy_image_alt_text', ''),
  );

  $form['imagedescset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Images with descriptions'),
    '#description' => t('Texy!\'s built-in module allows user to easily insert images along with their descriptions. Example: <code>[* image.png *] *** This is a short description of the image</code>.'),
    '#collapsible' => TRUE,
  );
  $form['imagedescset']['texy_image_desc_deflist'] = array(
    '#type' => 'checkbox',
    '#title' => t('Images as definition list terms'),
    '#description' => t('By selecting this option you can use definition list term and description elements as containers and output the content in a more semantic way.'),
    '#default_value' => variable_get('texy_image_desc_deflist', FALSE),
  );
  $form['imagedescset']['texy_image_desc_class'] = array(
    '#type' => 'textfield',
    '#size' => '20',
    '#title' => t('Class for images with descriptions'),
    '#description' => t('Name for a class which will be assigned to all images with accompanying descriptions.'),
    '#default_value' => variable_get('texy_image_desc_class', 'image-desc'),
  );
  $form['imagedescset']['texy_image_desc_class_left'] = array(
    '#type' => 'textfield',
    '#size' => '20',
    '#title' => t('Class for images with descriptions aligned to the left'),
    '#description' => t('Name for a class which will be assigned to image containers aligned to the left.'),
    '#default_value' => variable_get('texy_image_desc_class_left', 'image-desc-left'),
  );
  $form['imagedescset']['texy_image_desc_class_right'] = array(
    '#type' => 'textfield',
    '#size' => '20',
    '#title' => t('Class for images with descriptions aligned to the right'),
    '#description' => t('Name for a class which will be assigned to image containers aligned to the right.'),
    '#default_value' => variable_get('texy_image_desc_class_right', 'image-desc-right'),
  );

  return system_settings_form($form);
}

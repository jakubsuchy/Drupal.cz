<?php

/**
 * Implementation of hook_content_default_fields().
 */
function dcz_catalog_content_default_fields() {
  $fields = array();

  // Exported field: field_popis_webu
  $fields['content_katalog-field_popis_webu'] = array(
    'field_name' => 'field_popis_webu',
    'type_name' => 'content_katalog',
    'display_settings' => array(
      'weight' => '-1',
      'parent' => '',
      'label' => array(
        'format' => 'hidden',
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'text',
    'required' => '1',
    'multiple' => '0',
    'module' => 'text',
    'active' => '1',
    'text_processing' => '0',
    'max_length' => '',
    'allowed_values' => '',
    'allowed_values_php' => '',
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'value' => '',
          '_error_element' => 'default_value_widget][field_popis_webu][0][value',
        ),
      ),
      'default_value_php' => NULL,
      'rows' => '4',
      'size' => 60,
      'label' => 'Popis webu',
      'weight' => '-1',
      'description' => 'Toto pole je povinné. Napište minimálně jednu větu - k čemu web slouží, zajímavosti jak byl vytvořen, rozepište se!',
      'type' => 'text_textarea',
      'module' => 'text',
    ),
  );

  // Exported field: field_screenshoty
  $fields['content_katalog-field_screenshoty'] = array(
    'field_name' => 'field_screenshoty',
    'type_name' => 'content_katalog',
    'display_settings' => array(
      'weight' => 0,
      'parent' => '',
      'label' => array(
        'format' => 'above',
      ),
      'teaser' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'image_plain',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'filefield',
    'required' => '1',
    'multiple' => '0',
    'module' => 'filefield',
    'active' => '1',
    'list_field' => '0',
    'list_default' => 1,
    'description_field' => '0',
    'widget' => array(
      'max_resolution' => '1024x768',
      'image_path' => 'katalog/screenshoty',
      'custom_alt' => 0,
      'custom_title' => 0,
      'teaser_preset' => NULL,
      'body_preset' => NULL,
      'file_path' => 'katalog/screenshoty',
      'file_extensions' => NULL,
      'max_filesize_per_file' => NULL,
      'max_filesize_per_node' => NULL,
      'label' => 'Screenshot',
      'weight' => 0,
      'description' => 'Jeden či více screenshotů webu. Maximální rozlišení obrázku je 1024x768, doporučujeme volit právě toto rozlišení.',
      'type' => 'imagefield_widget',
      'module' => 'imagefield',
    ),
  );

  // Exported field: field_url_0
  $fields['content_katalog-field_url_0'] = array(
    'field_name' => 'field_url_0',
    'type_name' => 'content_katalog',
    'display_settings' => array(
      'weight' => '-2',
      'parent' => '',
      'label' => array(
        'format' => 'inline',
      ),
      'teaser' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
      'full' => array(
        'format' => 'default',
        'exclude' => 0,
      ),
    ),
    'widget_active' => '1',
    'type' => 'link',
    'required' => '1',
    'multiple' => '0',
    'module' => 'link',
    'active' => '1',
    'attributes' => array(
      'target' => 'default',
      'rel' => 0,
      'class' => '',
    ),
    'display' => array(
      'url_cutoff' => '80',
    ),
    'url' => '',
    'title' => 'none',
    'title_value' => '',
    'enable_tokens' => '',
    'validate_url' => '',
    'widget' => array(
      'default_value' => array(
        '0' => array(
          'url' => '',
          'title' => '',
          'attributes' => 'N;',
        ),
      ),
      'default_value_php' => '',
      'label' => 'URL',
      'weight' => '-2',
      'description' => 'Odkaz na prezentaci',
      'type' => 'link',
      'module' => 'link',
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Popis webu');
  t('Screenshot');
  t('URL');

  return $fields;
}

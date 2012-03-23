<?php
// $Id: location_handler_filter_location_country.inc,v 1.3 2008/12/02 22:50:21 bdragon Exp $

/**
 * Filter on country.
 */
class location_handler_filter_location_country extends views_handler_filter {

  function option_definition() {
    $options = parent::option_definition();
    $options['operator'] = array('default' => 'IS');

    return $options;
  }


  function admin_summary() {
    return '';
//    $options = $this->operator_options('short');
//    return (!empty($this->options['exposed']) ? t('exposed') : $options[$this->operator]);
  }

  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {
    $options = location_get_iso3166_list();
    $form['value'] = array(
      '#type' => 'select',
      '#title' => t('Country'),
      '#default_value' => $this->value,
      '#options' => $options,
      // Used by province autocompletion js.
      '#attributes' => array('class' => 'location_auto_country'),
      '#multiple' => TRUE, // views will change this as necessary when exposed.
    );

    // Let location_autocomplete.js find the correct fields to attach.
    $form['value']['#attributes']['class'] .= ' location_auto_join_'. $this->options['expose']['identifier'];
  }

  function operator_options() {
    if ($this->options['multiple']) {
      return array(
        'is' => t('Is one of'),
        'is not' => t('Is not one of'),
      );
    }
    else {
      return array(
        'is' => t('Is'),
        'is not' => t('Is not'),
      );
    }
  }

  function query() {
    if (empty($this->value)) {
      return;
    }

    $this->ensure_my_table();
    $field = "$this->table_alias.$this->real_field";

    // Normalize values.
    $value = $this->value;
    if (is_array($value)) {
      $value = array_keys($value);
      if (count($value) == 1) {
        // If multiple is allowed but only one was chosen, use a string instead.
        $value = reset($value);
      }
    }

    if (is_array($value)) {
      // Multiple values
      $placeholders = db_placeholders($value, 'varchar');
      $operator = ($this->operator == 'is') ? 'IN' : 'NOT IN';
      $this->query->add_where($this->options['group'], "$field $operator($placeholders)", $value);
    }
    else {
      // Single value
      $operator = ($this->operator == 'is') ? '=' : '!=';
      $this->query->add_where($this->options['group'], "$field $operator '%s'", $this->value);
    }
  }
}

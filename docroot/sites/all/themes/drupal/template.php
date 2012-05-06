<?php
function drupal_signup_user_form() {
  global $user;

  // This line is required for this form to function -- DO NOT EDIT OR REMOVE.
  $form['signup_form_data']['#tree'] = TRUE;

  $form['signup_form_data']['Name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#size' => 40, '#maxlength' => 64,
      '#required' => true,
      );
  $form['signup_form_data']['Company'] = array(
    '#type' => 'textfield',
    '#title' => t('Společnost'),
    '#size' => 40, '#maxlength' => 64,
  );
  $form['signup_form_data']['Skills'] = array(
      '#type' => 'select',
      '#title' => 'Co s drupalem umím?',
      '#options' => array('Začátečník', 'Pokročilý', 'Programuju moduly', 'Dělám témata', 'Programuju moduly i dělám témata'),
      );
  // If the user is logged in, fill in their name by default.
  if ($user->uid) {
    $form['signup_form_data']['Name']['#default_value'] = check_plain($user->name);
  }
  return $form;
}

function drupal_tagadelic_more($vid) { return; }
function drupal_tagadelic_weighted($terms) {
  $output = '';
  require_once drupal_get_path('module', 'pathauto') . '/pathauto.inc';
  switch($terms[0]->vid) {
    case 4:
      foreach ($terms as $term) {
        $output .= l($term->name, "clanky/" . check_plain(strtolower(pathauto_cleanstring($term->name))), array('attributes' => array('class' => "tagadelic level$term->weight"))) ." \n";
      }
     break;
    case 10:
      $output = l("všechny", "katalog", array('class' => 'tagadelic level1'))."\n";
      foreach ($terms as $term) {
        $output .= l($term->name, "katalog/tagy/" . check_plain(pathauto_cleanstring($term->name)), array('attributes' => array('class' => "tagadelic level$term->weight"))) ." \n";
      }
      break;
  }
  return $output;
}

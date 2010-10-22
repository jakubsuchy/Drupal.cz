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
/*  $form['signup_form_data']['Street'] = array(
    '#type' => 'textfield',
    '#title' => t('Ulice'),
    '#description' => 'Fakturační údaje - ulice',
    '#size' => 40, '#maxlength' => 64,
  );
  $form['signup_form_data']['Mesto'] = array(
    '#type' => 'textfield',
    '#title' => t('Město'),
    '#description' => 'Fakturační údaje - město',
    '#size' => 40, '#maxlength' => 64,
  );
  $form['signup_form_data']['PSC'] = array(
    '#type' => 'textfield',
    '#title' => t('PSČ'),
    '#description' => 'Fakturační údaje - PSČ',
    '#size' => 40, '#maxlength' => 64,
  );
  $form['signup_form_data']['IC'] = array(
    '#type' => 'textfield',
    '#title' => t('IČ'),
    '#description' => 'Fakturační údaje - IČ, pokud máte',
    '#size' => 40, '#maxlength' => 64,
  );
  $form['signup_form_data']['DIC'] = array(
    '#type' => 'textfield',
    '#title' => t('DIČ'),
    '#description' => 'Fakturační údaje - DIČ, pokud máte',
    '#size' => 40, '#maxlength' => 64,
  );
  $form['signup_form_data']['Telefon'] = array(
    '#type' => 'textfield',
    '#title' => t('Telefon'),
    '#description' => 'Telefonní číslo',
    '#size' => 40, '#maxlength' => 64,
  );*/
/*  $form['signup_form_data']['Checkbox'] = array(
    '#type' => 'checkbox',
    '#title' => t('Jsem si vědom toho, že školení je naplněno a já se stávám náhradníkem. Do 1.12.2008 mi pořadatel školení sdělí, zda se školení mohu zúčastnit.'),
    '#required' => TRUE,
    '#return_value' => 1,
  );*/

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


function drupal_vote_up_down_points_alt($cid, $type) {
  $vote_result = votingapi_get_voting_result($type, $cid, 'points', variable_get('vote_up_down_tag', 'vote'), 'sum');
  if ($vote_result) {
    $output = '<div id="vote_points_'. $cid .'" class="vote-points">'. check_plain($vote_result->value);
  }
  else {
    $output = '<div id="vote_points_'. $cid .'" class="vote-points">0';
  }

  $output .= '<div class="vote-points-label">Je tato<br />otázka častá?</div></div>';

  return $output;
}
function drupal_vote_up_down_widget_alt($cid, $type) {
  global $user;

  if (user_access('view up-down vote')) {

    $output = '<div class="vote-up-down-widget-alt">';
    $output .= theme('vote_up_down_points_alt', $cid, $type);

    if (user_access('use up-down vote') && $user->uid) {
      $user_vote = votingapi_get_user_votes($type, $cid);

      if ($user_vote[0]->value > 0) {
        $class = 'vote-up-act';
      }
      else {
        $class = 'vote-up-inact';
      }

      $output .= '<span id="vote_up_'. $cid .'" class="'. $class .'" title="'. url("vote_up_down/$type/$cid/1/1/1") .'">'. l('', "vote_up_down/$type/$cid/1", array('class' => $class, 'title' => t('Vote')), drupal_get_destination(), NULL, FALSE, TRUE) .'</span>';

    }
    else {
      $output .= '<span class="up-inact" title="'. t('You must login to vote.') .'"></span>';
    }

    $output .= '</div>';

    return $output;
  }
}

/*
   function phptemplate_preprocess_page(&$vars) {
    //static $count;
    //$count = is_array($count) ? $count : array();
    //$count[$hook] = is_int($count[$hook]) ? $count[$hook] : 1;
    //$vars['zebra'] = ($count[$hook] % 2) ?'odd' : 'even';
    //$vars['seqid'] = $count[$hook]++;
}
*/


/*function drupal_links($links = array(), $delimiter = ' | ') {
	  return _phptemplate_callback('links', array('links' => $links, 'delimiter' => $delimiter));
}*/
function drupal_book_navigation($node) {
  $output = '';
  $links = '';

  if ($node->nid) {
    $tree = book_tree($node->nid);

    if ($prev = book_prev($node)) {
      drupal_add_link(array('rel' => 'prev', 'href' => url('node/'. $prev->nid)));
      $links .= l(t('‹ ') . $prev->title, 'node/'. $prev->nid, array('class' => 'page-previous', 'title' => t('Go to previous page')));
    }
    if ($node->parent) {
      drupal_add_link(array('rel' => 'up', 'href' => url('node/'. $node->parent)));
      $links .= l(t('up'), 'node/'. $node->parent, array('class' => 'page-up', 'title' => t('Go to parent page')));
    }
    if ($next = book_next($node)) {
      drupal_add_link(array('rel' => 'next', 'href' => url('node/'. $next->nid)));
      $links .= l($next->title . t(' ›'), 'node/'. $next->nid, array('class' => 'page-next', 'title' => t('Go to next page')));
    }

    if (isset($tree) || isset($links)) {
      $output = '<div class="book-navigation" id="book-navigation">';
      $output .= "<div class=\"book-head\">Seriál</div>";
      if (isset($tree)) {
        $output .= $tree;
      }
      if (isset($links)) {
        $output .= '<div class="page-links">'. $links .'</div>';
      }
      $output .= '</div>';
    }
  }

  return $output;
}

function drupal_tagadelic_more($vid) { return; }
function drupal_tagadelic_weighted($terms) {
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


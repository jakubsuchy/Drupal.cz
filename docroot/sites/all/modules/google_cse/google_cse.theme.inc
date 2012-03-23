<?php
// $Id: google_cse.theme.inc,v 1.1.2.7 2008/07/01 19:09:09 mfb Exp $

/**
 * @file
 * Themeable functions for Google Custom Search Engine.
 */

/**
 * The search results page can be themed/customized.
 */
function template_preprocess_google_cse_results(&$variables) {
  $query = array(
    'q' => isset($_GET['query']) ? $_GET['query'] : '',
    'cx' => variable_get('google_cse_cx', ''),
    'cof' => variable_get('google_cse_cof_google', 'FORID:0'),
    'sitesearch' => google_cse_sitesearch_default(),
  ) + google_cse_advanced_settings();

  $variables['results_searchbox_form'] = $variables['form'] ? drupal_get_form('google_cse_results_searchbox_form', $variables['self']) : '';
  $variables['noscript'] = t('!google, or enable JavaScript to view them here.', array('!google' => l(t('View the results at Google'), 'http://'. variable_get('google_cse_domain', 'www.google.com') .'/cse', array('query' => $query))));
  $variables['prefix'] = filter_xss_admin(variable_get('google_cse_results_prefix', ''));
  $variables['suffix'] = filter_xss_admin(variable_get('google_cse_results_suffix', ''));

  $settings = array(
    'googleCSE' => array(
      'resultsWidth' => intval(variable_get('google_cse_results_width', 600)),
      'domain' => variable_get('google_cse_domain', 'www.google.com'),
    ),
  );
  drupal_add_js($settings, 'setting');
}

/**
 * Display an Add-to-Google button.
 */
function template_preprocess_google_cse_results_gadget(&$variables) {
  $cx = explode(':', variable_get('google_cse_cx', ''));
  $variables['creator'] = drupal_urlencode($cx[0]);
  $variables['id'] = isset($cx[1]) ? drupal_urlencode($cx[1]) : '';
}

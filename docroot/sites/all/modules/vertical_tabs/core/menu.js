
Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.menu = function() {
  if ($('#edit-menu-link-title').val()) {
    return Drupal.checkPlain($('#edit-menu-link-title').val());
  }
  else {
    return Drupal.t('Not in menu');
  }
}

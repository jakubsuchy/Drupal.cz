
Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.path = function() {
  var path = $('#edit-path').val();
  if (path) {
    return Drupal.t('Alias: @alias', { '@alias': path });
  }
  else {
    return Drupal.t('No alias');
  }
}

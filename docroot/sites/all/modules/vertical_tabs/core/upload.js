
Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.attachments = function() {
  var size = $('#upload-attachments tbody tr').size();
  if (size) {
    return Drupal.formatPlural(size, '1 attachment', '@count attachments');
  }
  else {
    return Drupal.t('No attachments');
  }
}

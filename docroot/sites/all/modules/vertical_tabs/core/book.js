
Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.book = function() {
  var text = $('#edit-book-bid option[selected]').text();
  if (text == Drupal.t('<none>')) {
    return Drupal.t('Not in book');
  }
  else if (text == Drupal.t('<create a new book>')) {
    return Drupal.t('New book');
  }
  return text;
}

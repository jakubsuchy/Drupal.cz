
Drupal.verticalTabs = Drupal.verticalTabs || {};

Drupal.verticalTabs.revision_information = function() {
  if ($('#edit-revision').length) {
    if ($('#edit-revision').attr('checked')) {
      return Drupal.t('New revision');
    }
    else {
      return Drupal.t('No revision');
    }
  }
  else {
    return '';
  }
}

Drupal.verticalTabs.author = function() {
  var author = $('#edit-name').val() || Drupal.t('Anonymous');
  var date = $('#edit-date').val();
  if (date) {
    return Drupal.t('By @name on @date', { '@name': author, '@date': date });
  }
  else {
    return Drupal.t('By @name', { '@name': author });
  }
}

Drupal.verticalTabs.options = function() {
  var vals = [];
  $('fieldset.vertical-tabs-options input:checked').parent().each(function () {
    vals.push(Drupal.checkPlain($.trim($(this).text())));
  });
  if (!$('#edit-status').is(':checked')) {
    vals.unshift(Drupal.t('Not published'));
  }
  return vals.join(', ');
}

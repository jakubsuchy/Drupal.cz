// $Id: subscriptions_tableselect.js,v 1.3 2008/05/18 20:43:03 salvis Exp $

Drupal.subscriptions_rowToggle = function(ckb) {
  var thisRow = $(ckb).parents('tr:first');
  var controls = $('input, select', thisRow);
  for (var i = 1; i < controls.length; i++) {
    controls[i].style['visibility'] = (ckb.checked ? 'visible' : 'hidden');
  }
}

Drupal.subscriptions_tableSelect = function() {
  // Dynamically hide/show the other columns depending on the checkbox state in the first column.  
  var rows = $('tr', this);
  for (var i = 1; i < rows.length; i++) {
    var row = rows[i];
    var input = $('td:first input:checkbox', row);
    input.click(function(e) {
      Drupal.subscriptions_rowToggle(this);
    });

    Drupal.subscriptions_rowToggle(input[0]);
  }
}

Drupal.behaviors.subscriptions_tableSelect = function (context) {
  $('form table:has(th.subscriptions-table):not(.subscriptions_tableSelect-processed)', context)
  .addClass('subscriptions_tableSelect-processed').each(Drupal.subscriptions_tableSelect);
};

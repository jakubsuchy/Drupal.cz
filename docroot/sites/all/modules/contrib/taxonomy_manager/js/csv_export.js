/**
 * CSV Export
 *
 * adds click event to export button and makes AJAX call to get the CSV data
 */

Drupal.behaviors.TaxonomyManagerCSVExport = function(context) {

  if (!$('#taxonomy-manager-toolbar' + '.tm-csv-processed').size()) {
    $('#taxonomy-manager-toolbar').addClass('tm-csv-processed');
    var url = Drupal.settings.exportCSV['url'];
    var vid = Drupal.settings.taxonomytree['vid'];

    $("#edit-export-submit").click(function() {
      var area = $("#edit-export-csv");
      var param = new Object();
      param['delimiter'] = $("#edit-export-delimiter").val();
      param['depth'] = $("#edit-export-depth").val();
      param['option'] = $("#taxonomy_manager_export_options").find("input[type=radio][checked]").val();
      param['vid'] = vid;
      var tid = 0;
      $('.treeview').find("input[type=checkbox][checked]").each(function() {
       tid = Drupal.getTermId($(this).parents("li").eq(0));
      });
      param['tid'] = tid;

      $.post(url, param, function(data) {
        $(area).val(data);
      });
      return false;
    });
  }
}


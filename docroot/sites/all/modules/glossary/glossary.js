/* $Id: glossary.js,v 1.1.4.3 2008/06/29 18:25:55 nancyw Exp $ */

function glossary_replace_handler(event) {
  // Disable superscript field if not selected.
  if ($("input[@name=glossary_replace]:checked").val() == 'superscript') {
    $("input[@name=glossary_superscript]").parents("div.glossary_superscript").show();
  }
  else {
    $("input[@name=glossary_superscript]").parents("div.glossary_superscript").hide();
  }

  // Disable icon URL field if not selected.
  if ($("input[@name=glossary_replace]:checked").val() == 'icon') {
    $("input[@name=glossary_icon]").parents("div.glossary_icon").show();
  }
  else {
    $("input[@name=glossary_icon]").parents("div.glossary_icon").hide();;
  }
}

function glossary_link_related_handler(event) {
  // Disable one-way field if not selected.
  if ($("input[@name=glossary_link_related]:checked").val() == 1) {
    $("input[@name=glossary_link_related_how]").parents("div.glossary_link_related_how").show();
  }
  else {
    $("input[@name=glossary_link_related_how]").val(0);
    $("input[@name=glossary_link_related_how]").parents("div.glossary_link_related_how").hide();
  }
}

// Run the javascript on page load.
if (Drupal.jsEnabled) {
  $(document).ready(function () {
  // On page load, determine the current settings.
  glossary_replace_handler();
  glossary_link_related_handler();

  // Bind the functions to click events.
  $("input[@name=glossary_replace]").bind("click", glossary_replace_handler);
  $("input[@name=glossary_link_related]").bind("click", glossary_link_related_handler);
  });
}
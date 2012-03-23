/* $Id: macrobuilder.js,v 1.3 2009/02/11 19:59:45 bdragon Exp $ */

/**
 * @file
 * Map ID widget for macro form.
 */

/*global $, Drupal */

Drupal.gmap.addHandler('mapid', function (elem) {
  var obj = this;
  // Respond to incoming map id changes.
  var binding = obj.bind("idchange", function () {
    elem.value = obj.vars.macro_mapid;
  });
  // Send out outgoing map id changes.
  $(elem).change(function () {
    obj.vars.macro_mapid = elem.value;
    obj.change("idchange", binding);
  });
});

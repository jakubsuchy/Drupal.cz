/* $Id: clustermarker_marker.js,v 1.1 2009/02/12 23:45:23 bdragon Exp $ */

/**
 * @file
 * GMap Markers
 * Martin Pearman's ClusterMarker version
 */

/*global ClusterMarker, Drupal, GMarker */

// Replace to override marker creation
Drupal.gmap.factory.marker = function (loc, opts) {
  return new GMarker(loc, opts);
};

Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;

  obj.bind('init', function () {
    obj.clustermarker = 0;
  });

  obj.bind('iconsready', function () {
    if (!obj.clustermarker) {
      var options = Drupal.settings.gmap_markermanager;
      if (options.clusterMarkerIcon.length) {
        options.clusterMarkerIcon = Drupal.gmap.getIcon(options.clusterMarkerIcon, 0);
      }
      else {
        delete options.clusterMarkerIcon;
      }
      options.borderPadding = +options.borderPadding;
      options.fitMapMaxZoom = +options.fitMapMaxZoom;
      options.intersectPadding = +options.intersectPadding;
      obj.clusterMarker = new ClusterMarker(obj.map, options);
    }
  });

  obj.bind('addmarker', function (marker) {
    obj.clusterMarker.addMarkers([marker.marker]);
  });

  obj.bind('delmarker', function (marker) {
    // @@@TODO: Implement this!
  });

  obj.bind('clearmarkers', function () {
    obj.clusterMarker.removeMarkers();
  });
});

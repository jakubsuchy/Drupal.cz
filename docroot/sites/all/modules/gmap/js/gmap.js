/* $Id: gmap.js,v 1.13 2009/02/11 18:08:53 bdragon Exp $ */

/**
 * @file
 * Drupal to Google Maps API bridge.
 */

/*global $, Drupal, GLatLng, GSmallZoomControl, GLargeMapControl, GMap2 */
/*global GMapTypeControl, GSmallMapControl, G_HYBRID_MAP, G_NORMAL_MAP */
/*global G_PHYSICAL_MAP, G_SATELLITE_MAP, GHierarchicalMapTypeControl */
/*global GKeyboardHandler, GLatLngBounds, GMenuMapTypeControl, GEvent */
/*global GOverviewMapControl, GScaleControl, GUnload */

(function () { // BEGIN closure
  var handlers = {};
  var maps = {};

  Drupal.gmap = {

    /**
     * Retrieve a map object for use by a non-widget.
     * Use this if you need to be able to fire events against a certain map
     * which you have the mapid for.
     * Be a good GMap citizen! Remember to send change()s after modifying variables!
     */
    getMap: function (mapid) {
      if (maps[mapid]) {
        return maps[mapid];
      }
      else {
        // Perhaps the user passed a widget id instead?
        mapid = mapid.split('-').slice(1, -1).join('-');
        if (maps[mapid]) {
          return maps[mapid];
        }
      }
      return false;
    },

    unloadMap: function (mapid) {
      delete maps[mapid];
    },

    addHandler: function (handler, callback) {
      if (!handlers[handler]) {
        handlers[handler] = [];
      }
      handlers[handler].push(callback);
    },

    globalChange: function (name, userdata) {
      for (var mapid in Drupal.settings.gmap) {
        if (Drupal.settings.gmap.hasOwnProperty(mapid)) {
          maps[mapid].change(name, -1, userdata);
        }
      }
    },

    setup: function () {
      var obj = this;

      var initcallback = function (mapid) {
        return (function () {
          maps[mapid].change("bootstrap_options", -1);
          maps[mapid].change("boot", -1);
          maps[mapid].change("init", -1);
          // Send some changed events to fire up the rest of the initial settings..
          maps[mapid].change("maptypechange", -1);
          maps[mapid].change("controltypechange", -1);
          maps[mapid].change("alignchange", -1);
          // Set ready to put the event system into action.
          maps[mapid].ready = true;
          maps[mapid].change("ready", -1);
        });
      };

      if (Drupal.settings && Drupal.settings.gmap) {
        var mapid = obj.id.split('-');
        var instanceid = mapid.pop();
        mapid.shift();
        mapid = mapid.join('-');
        var control = instanceid.replace(/\d+$/, '');

        // Lazy init the map object.
        if (!maps[mapid]) {
          maps[mapid] = new Drupal.gmap.map(Drupal.settings.gmap[mapid]);
          // Prepare the initialization callback.
          var callback = initcallback(mapid);
          setTimeout(callback, 0);
        }

        if (handlers[control]) {
          for (var i = 0; i < handlers[control].length; i++) {
            handlers[control][i].call(maps[mapid], obj);
          }
        }
        else {
          // Element with wrong class?
        }
      }
    }
  };
})(); // END closure

Drupal.gmap.factory = {};

Drupal.gmap.map = function (v) {
  this.vars = v;
  this.map = undefined;
  this.ready = false;
  var _bindings = {};

  /**
   * Register interest in a change.
   */
  this.bind = function (name, callback) {
    if (!_bindings[name]) {
      _bindings[name] = [];
    }
    return _bindings[name].push(callback) - 1;
  };

  /**
   * Change notification.
   * Interested parties can act on changes.
   */
  this.change = function (name, id, userdata) {
    var c;
    if (_bindings[name]) {
      for (c = 0; c < _bindings[name].length; c++) {
        if (c !== id) {
          _bindings[name][c](userdata);
        }
      }
    }
    if (name !== 'all') {
      this.change('all', -1, name, userdata);
    }
  };

  /**
   * Deferred change notification.
   * This will cause a change notification to be tacked on to the *end* of the event queue.
   */
  this.deferChange = function (name, id, userdata) {
    var obj = this;
    // This will move the function call to the end of the event loop.
    setTimeout(function () {
      obj.change(name, id, userdata);
    }, 0);
  };
};

////////////////////////////////////////
//             Map widget             //
////////////////////////////////////////
Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;

  var _ib = {};


  // Respond to incoming zooms
  _ib.zoom = obj.bind("zoom", function () {
    obj.map.setZoom(obj.vars.zoom);
  });

  // Respond to incoming moves
  _ib.move = obj.bind("move", function () {
    obj.map.panTo(new GLatLng(obj.vars.latitude, obj.vars.longitude));
  });

  // Respond to incoming map type changes
  _ib.mtc = obj.bind("maptypechange", function () {
    var i;
    for (i = 0; i < obj.opts.mapTypeNames.length; i++) {
      if (obj.opts.mapTypeNames[i] === obj.vars.maptype) {
        obj.map.setMapType(obj.opts.mapTypes[i]);
        break;
      }
    }
  });

  // Respond to incoming width changes.
  _ib.width = obj.bind("widthchange", function (w) {
    obj.map.getContainer().style.width = w;
    obj.map.checkResize();
  });
  // Send out outgoing width changes.
  // N/A
  // Respond to incoming height changes.
  _ib.height = obj.bind("heightchange", function (h) {
    obj.map.getContainer().style.height = h;
    obj.map.checkResize();
  });
  // Send out outgoing height changes.
  // N/A

  // Respond to incoming control type changes.
  _ib.ctc = obj.bind("controltypechange", function () {
    if (obj.currentcontrol) {
      obj.map.removeControl(obj.currentcontrol);
    }
    if (obj.vars.controltype === 'Micro') {
      obj.map.addControl(obj.currentcontrol = new GSmallZoomControl());
    }
    else if (obj.vars.controltype === 'Small') {
      obj.map.addControl(obj.currentcontrol = new GSmallMapControl());
    }
    else if (obj.vars.controltype === 'Large') {
      obj.map.addControl(obj.currentcontrol = new GLargeMapControl());
    }
  });
  // Send out outgoing control type changes.
  // N/A

  obj.bind("bootstrap_options", function () {
    // Bootup options.
    var opts = {}; // Object literal GMapOptions
    obj.opts = opts;

    // Null out the enabled types.
    opts.mapTypes = [];
    opts.mapTypeNames = [];

    // Load google map types.
    if (obj.vars.baselayers.Map) {
      opts.mapTypes.push(G_NORMAL_MAP);
      opts.mapTypeNames.push('Map');
    }
    if (obj.vars.baselayers.Satellite) {
      opts.mapTypes.push(G_SATELLITE_MAP);
      opts.mapTypeNames.push('Satellite');
    }
    if (obj.vars.baselayers.Hybrid) {
      opts.mapTypes.push(G_HYBRID_MAP);
      opts.mapTypeNames.push('Hybrid');
    }
    if (obj.vars.baselayers.Physical) {
      opts.mapTypes.push(G_PHYSICAL_MAP);
      opts.mapTypeNames.push('Physical');
    }

  });

  obj.bind("boot", function () {
    obj.map = new GMap2(elem, obj.opts);
  });

  obj.bind("init", function () {
    var map = obj.map;

    // Map type control
    if (obj.vars.mtc === 'standard') {
      map.addControl(new GMapTypeControl());
    }
    else if (obj.vars.mtc === 'hier') {
      map.addControl(new GHierarchicalMapTypeControl());
    }
    else if (obj.vars.mtc === 'menu') {
      map.addControl(new GMenuMapTypeControl());
    }

    if (obj.vars.behavior.overview) {
      map.addControl(new GOverviewMapControl());
    }
    if (obj.vars.behavior.scale) {
      map.addControl(new GScaleControl());
    }
    if (obj.vars.behavior.nodrag) {
      map.disableDragging();
    }
    else if (!obj.vars.behavior.nokeyboard) {
      obj._kbdhandler = new GKeyboardHandler(map);
    }
    if (obj.vars.extent) {
      var c = obj.vars.extent;
      var extent = new GLatLngBounds(new GLatLng(c[0][0], c[0][1]), new GLatLng(c[1][0], c[1][1]));
      obj.vars.latitude = extent.getCenter().lat();
      obj.vars.longitude = extent.getCenter().lng();
      obj.vars.zoom = map.getBoundsZoomLevel(extent);
    }
    if (obj.vars.behavior.collapsehack) {
      // Modify collapsable fieldsets to make maps check dom state when the resize handle
      // is clicked. This may not necessarily be the correct thing to do in all themes,
      // hence it being a behavior.
      setTimeout(function () {
        var r = function () {
          map.checkResize();
          map.setCenter(new GLatLng(obj.vars.latitude, obj.vars.longitude), obj.vars.zoom);
        };
        $(elem).parents('fieldset.collapsible').children('legend').children('a').click(r);
        // Would be nice, but doesn't work.
        //$(elem).parents('fieldset.collapsible').children('.fieldset-wrapper').scroll(r);
      }, 0);
    }
    map.setCenter(new GLatLng(obj.vars.latitude, obj.vars.longitude), obj.vars.zoom);

    if (!obj.vars.nocontzoom) {
      map.enableDoubleClickZoom();
      map.enableContinuousZoom();
    }
    if (!obj.vars.behavior.nomousezoom) {
      map.enableScrollWheelZoom();
    }

    // Send out outgoing zooms
    GEvent.addListener(map, "zoomend", function (oldzoom, newzoom) {
      obj.vars.zoom = newzoom;
      obj.change("zoom", _ib.zoom);
    });

    // Send out outgoing moves
    GEvent.addListener(map, "moveend", function () {
      var coord = map.getCenter();
      obj.vars.latitude = coord.lat();
      obj.vars.longitude = coord.lng();
      obj.change("move", _ib.move);
    });

    // Send out outgoing map type changes.
    GEvent.addListener(map, "maptypechanged", function () {
      // If the map isn't ready yet, ignore it.
      if (obj.ready) {
        var type = map.getCurrentMapType();
        var i;
        for (i = 0; i < obj.opts.mapTypes.length; i++) {
          if (obj.opts.mapTypes[i] === type) {
            obj.vars.maptype = obj.opts.mapTypeNames[i];
          }
        }
        obj.change("maptypechange", _ib.mtc);
      }
    });

  });
});

////////////////////////////////////////
//            Zoom widget             //
////////////////////////////////////////
Drupal.gmap.addHandler('zoom', function (elem) {
  var obj = this;
  // Respond to incoming zooms
  var binding = obj.bind("zoom", function () {
    elem.value = obj.vars.zoom;
  });
  // Send out outgoing zooms
  $(elem).change(function () {
    obj.vars.zoom = parseInt(elem.value, 10);
    obj.change("zoom", binding);
  });
});

////////////////////////////////////////
//          Latitude widget           //
////////////////////////////////////////
Drupal.gmap.addHandler('latitude', function (elem) {
  var obj = this;
  // Respond to incoming movements.
  var binding = obj.bind("move", function () {
    elem.value = '' + obj.vars.latitude;
  });
  // Send out outgoing movements.
  $(elem).change(function () {
    obj.vars.latitude = Number(this.value);
    obj.change("move", binding);
  });
});

////////////////////////////////////////
//         Longitude widget           //
////////////////////////////////////////
Drupal.gmap.addHandler('longitude', function (elem) {
  var obj = this;
  // Respond to incoming movements.
  var binding = obj.bind("move", function () {
    elem.value = '' + obj.vars.longitude;
  });
  // Send out outgoing movements.
  $(elem).change(function () {
    obj.vars.longitude = Number(this.value);
    obj.change("move", binding);
  });
});

////////////////////////////////////////
//          Latlon widget             //
////////////////////////////////////////
Drupal.gmap.addHandler('latlon', function (elem) {
  var obj = this;
  // Respond to incoming movements.
  var binding = obj.bind("move", function () {
    elem.value = '' + obj.vars.latitude + ',' + obj.vars.longitude;
  });
  // Send out outgoing movements.
  $(elem).change(function () {
    var t = this.value.split(',');
    obj.vars.latitude = Number(t[0]);
    obj.vars.longitude = Number(t[1]);
    obj.change("move", binding);
  });
});

////////////////////////////////////////
//          Maptype widget            //
////////////////////////////////////////
Drupal.gmap.addHandler('maptype', function (elem) {
  var obj = this;
  // Respond to incoming movements.
  var binding = obj.bind("maptypechange", function () {
    elem.value = obj.vars.maptype;
  });
  // Send out outgoing movements.
  $(elem).change(function () {
    obj.vars.maptype = elem.value;
    obj.change("maptypechange", binding);
  });
});

(function () { // BEGIN CLOSURE
  var re = /([0-9.]+)\s*(em|ex|px|in|cm|mm|pt|pc|%)/;
  var normalize = function (str) {
    var ar;
    if ((ar = re.exec(str.toLowerCase()))) {
      return ar[1] + ar[2];
    }
    return null;
  };
  ////////////////////////////////////////
  //           Width widget             //
  ////////////////////////////////////////
  Drupal.gmap.addHandler('width', function (elem) {
    var obj = this;
    // Respond to incoming width changes.
    var binding = obj.bind("widthchange", function (w) {
      elem.value = normalize(w);
    });
    // Send out outgoing width changes.
    $(elem).change(function () {
      var n;
      if ((n = normalize(elem.value))) {
        elem.value = n;
        obj.change('widthchange', binding, n);
      }
    });
    obj.bind('init', function () {
      $(elem).change();
    });
  });

  ////////////////////////////////////////
  //           Height widget            //
  ////////////////////////////////////////
  Drupal.gmap.addHandler('height', function (elem) {
    var obj = this;
    // Respond to incoming height changes.
    var binding = obj.bind("heightchange", function (h) {
      elem.value = normalize(h);
    });
    // Send out outgoing height changes.
    $(elem).change(function () {
      var n;
      if ((n = normalize(elem.value))) {
        elem.value = n;
        obj.change('heightchange', binding, n);
      }
    });
    obj.bind('init', function () {
      $(elem).change();
    });
  });

})(); // END CLOSURE

////////////////////////////////////////
//        Control type widget         //
////////////////////////////////////////
Drupal.gmap.addHandler('controltype', function (elem) {
  var obj = this;
  // Respond to incoming height changes.
  var binding = obj.bind("controltypechange", function () {
    elem.value = obj.vars.controltype;
  });
  // Send out outgoing height changes.
  $(elem).change(function () {
    obj.vars.controltype = elem.value;
    obj.change("controltypechange", binding);
  });
});

// Map cleanup.
if (Drupal.jsEnabled) {
  $(document).unload(GUnload);
}

Drupal.behaviors.GMap = function (context) {
  $('.gmap-control:not(.gmap-processed)', context).addClass('gmap-processed').each(Drupal.gmap.setup);
};

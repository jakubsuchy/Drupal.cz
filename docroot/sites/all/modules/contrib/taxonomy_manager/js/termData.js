
/**
 * @file js support for term editing form for ajax saving and tree updating
 */

//global var that holds the current term link object
var active_term = new Object();

//holds tree objects, useful in double tree interface, when both trees needs to be updated
var trees = new Object();

/**
 * attaches term data form, used after 'Saves changes' ahah submit
 */
Drupal.behaviors.TaxonomyManagerTermData = function(context) {
  if (!$('#taxonomy-manager-toolbar' + '.tm-termData-processed').size()) {
    var vid = $('#edit-term-data-vid').val();
    for (var id in trees) {
      var tree = trees[id];
      if (tree.vocId == vid) {
        Drupal.attachTermDataForm(tree);
        break;
      }
    }
  }
}

/**
 * attaches Term Data functionality, called by tree.js
 */
Drupal.attachTermData = function(ul, tree) {
  trees[tree.treeId] = tree;
  Drupal.attachTermDataLinks(ul, tree);

  if (!$('#taxonomy-manager-toolbar' + '.tm-termData-processed').size()) {
	  Drupal.attachTermDataForm(tree);
  }
}

/**
 * adds click events to the term links in the tree structure
 */
Drupal.attachTermDataLinks = function(ul, tree) {
  $(ul).find('a.term-data-link').click(function() {
    Drupal.activeTermSwapHighlight(this);
    var li = $(this).parents("li");
    var termdata = new Drupal.TermData(Drupal.getTermId(li), this.href +'/true', li, tree);
    termdata.load();
    return false;
  });
}

/**
* hightlights current term
*/
Drupal.activeTermSwapHighlight = function(link) {
  try {
    $(active_term).parent().removeClass('highlightActiveTerm');
  } catch(e) {}
  active_term = link;
  $(active_term).parent().addClass('highlightActiveTerm');
}

/**
 * attaches click events to next siblings
 */
Drupal.attachTermDataToSiblings = function(all, currentIndex, tree) {
  var nextSiblings = $(all).slice(currentIndex);
  $(nextSiblings).find('a.term-data-link').click(function() {
    var li = $(this).parents("li");
    var termdata = new Drupal.TermData(Drupal.getTermId(li), this.href +'/true', li, tree);
    termdata.load();
    return false;
  });
}

/**
 * adds click events to term data form, which is already open, when page gets loaded
 */
Drupal.attachTermDataForm = function(tree) {
  $('#taxonomy-manager-toolbar').addClass('tm-termData-processed');
  var tid = $('#edit-term-data-tid').val();
  if (tid) {
    var li = tree.getLi(tid);
    var termLink = $(li).children("div.term-line").find("a.term-data-link");
    Drupal.activeTermSwapHighlight(termLink);
    var url = Drupal.settings.termData['term_url'] +'/'+ tid +'/true';
    var termdata = new Drupal.TermData(tid, url, li, tree);
    termdata.form();
  }
}

/**
 * TermData Object
 */
Drupal.TermData = function(tid, href, li, tree) {
  this.href = href;
  this.tid = tid;
  this.li = li;
  this.tree = tree
  this.form_build_id = tree.form_build_id;
  this.form_id = tree.form_id;
  this.param = new Object();
  this.param['form_id'] = tree.form_id;
  this.param['form_token'] = tree.form_token;
  this.vid = tree.vocId;
  this.div = $('#taxonomy-term-data');
}


/**
 * loads ahah form from given link and displays it on the right side
 */
Drupal.TermData.prototype.load = function() {
  var url = this.href;
  var termdata = this;

  $.get(url, this.param, function(data) {
    termdata.insertForm(data);
  });
}

/**
 * inserts received html data into form wrapper
 */
Drupal.TermData.prototype.insertForm = function(data) {
  $(this.div).html(data);
  this.form();
}

/**
 * adds events to possible operations
 */
Drupal.TermData.prototype.form = function() {
  var termdata = this;

  try {
    Drupal.behaviors.textarea(this.div);
    Drupal.behaviors.autocomplete(this.div);
    Drupal.behaviors.ahah(this.div);
  } catch(e) {} //autocomplete or textarea js not added to page

  this.param['tid'] = this.tid;
  this.param['vid'] = this.vid;

  $(this.div).find('div.term-data-autocomplete-add > span').click(function() {
    termdata.param['attr_type'] = $(this).attr("class");
    termdata.param['value'] = $(this).parents("tr").find('input:text').attr('value');
    termdata.param['op'] = 'add';
    $('#taxonomy-term-data-fieldset :input').each(function() {
      termdata.param[$(this).attr('id')] = $(this).attr('value');
    });
    termdata.send();
  });

  $(this.div).find('td.taxonomy-term-data-operations > span').click(function() {
    termdata.param['attr_type'] = $(this).attr("class");
    termdata.param['info'] = $(this).attr("id");
    var value = $(this).parent().siblings(".taxonomy-term-data-name").attr("id");
    termdata.param['value'] = value.substring(5);
    termdata.param['op'] = 'delete';
    $('#taxonomy-term-data-fieldset :input').each(function() {
      termdata.param[$(this).attr('id')] = $(this).attr('value');
    });
    termdata.send();
  });

  $(this.div).find('#edit-term-data-weight').change(function() {
    termdata.param['value'] = this.value;
    termdata.param['attr_type'] = 'weight';
    termdata.param['op'] = 'update';
    termdata.send();
  });

  $(this.div).find('#edit-term-data-language').change(function() {
    termdata.param['value'] = this.value;
    termdata.param['attr_type'] = 'language';
    termdata.param['op'] = 'update';
    termdata.send();
  });

  $(this.div).find('#edit-term-data-save').click(function() {
    $('#taxonomy-manager-toolbar').removeClass("tm-termData-processed");
    termdata.param['value'] = $('#edit-term-data-name').attr('value');
    termdata.updateTermName();
  });

  $(this.div).find('#term-data-close span').click(function() {
    termdata.div.children().hide();
  });

  $(this.div).find('a.taxonomy-term-data-name-link').click(function() {
    var url = this.href;
    var tid = url.split("/").pop();
    var li = termdata.tree.getLi(tid);
    termdata.tree.loadRootForm(tid);
    termdata_new = new Drupal.TermData(tid, this.href +'/true', li, termdata.tree);
    termdata_new.load();
    return false;
  });

  $(this.div).find("legend").each(function() {
    var staticOffsetX, staticOffsetY = null;
    var left, top = 0;
    var div = termdata.div;
    var pos = $(div).position();
    $(this).mousedown(startDrag);

    function startDrag(e) {
      if (staticOffsetX == null && staticOffsetY == null) {
        staticOffsetX = e.pageX;
        staticOffsetY = e.pageY;
      }
      $(document).mousemove(performDrag).mouseup(endDrag);
      return false;
    }

    function performDrag(e) {
      left = e.pageX - staticOffsetX;
      top = e.pageY - staticOffsetY;
      $(div).css({position: "absolute", "left": pos.left + left +"px", "top": pos.top + top +"px"});
      return false;
    }

    function endDrag(e) {
      $(document).unbind("mousemove", performDrag).unbind("mouseup", endDrag);
    }
  });
}

/**
 * sends updated data through param
 */
Drupal.TermData.prototype.send = function() {
  var termdata = this;
  var url= Drupal.settings.termData['url'];
  if (this.param['value'] != '' && this.param['attr_type'] != '') {
    $.ajax({
      data: termdata.param,
      type: "POST",
      url: url,
      dataType: 'json',
      success: function(response, status) {
        termdata.update();
        termdata.insertForm(response.data);
      }
    });
  }
}

/**
 * updates term data form and if necessary tree structure
 */
Drupal.TermData.prototype.update = function() {
  for (var id in trees) {
    var tree = trees[id];
    if (tree.vocId == this.vid) {
       this.updateTree(tree);
    }
  }
}

Drupal.TermData.prototype.updateTree = function(tree) {
  if (this.param['attr_type'] == 'parent' || (this.param['attr_type'] == 'related' && this.param['op'] == 'add') || (this.param['attr_type'] == 'language' && this.param['op'] == 'update')) {
    tree.loadRootForm(this.tid);
  }
  else if (this.param['attr_type'] == 'weight') {
    var parentLi = $(this.li).parents("li");
    if ($(parentLi).is("li")) {
      tree.loadChildForm(parentLi, true);
    }
    else {
      //no parent li --> root level terms
      //load whole Tree
      tree.loadRootForm(this.tid);
    }
  }
}

/**
 * updates term name in tree structure
 */
Drupal.TermData.prototype.updateTermName = function() {
  var name = Drupal.checkPlain(this.param['value']);
  $('fieldset#taxonomy-term-data-fieldset legend').html(name);
  $('ul.treeview li input:hidden[class=term-id][value='+ this.tid +']')
    .siblings('div.form-item')
    .find('label.option a').html(name);
}

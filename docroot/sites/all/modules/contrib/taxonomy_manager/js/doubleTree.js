
/**
 * Double Tree Object
 */
Drupal.DoubleTree = function(tree1, tree2) {
  this.leftTree = tree1;
  this.rightTree = tree2;
  this.selected_terms = new Array();
  this.selected_parents = new Array();
  this.direction = "left-to-right";
  this.updateWholeTree = false;
  this.url = Drupal.settings.DoubleTree['url'];
  this.param = new Object();
  this.param['form_id'] = $(':input[name="form_id"]').val();
  this.param['form_token'] = $(':input[name="form_token"]').val();

  this.attachOperations();
}

/**
 * attaches click events to the operations and collects selected terms
 */
Drupal.DoubleTree.prototype.attachOperations = function() {
  var doubleTree = this;
  $('#taxonomy-manager-double-tree-operations :input').click(function() {
    doubleTree.selected_terms = new Array();
    doubleTree.selected_parents = new Array();

    var button_value = $(this).val();
    doubleTree.param['op'] = 'move';

    if (button_value == 'Move right' || button_value == "Switch right") {
      doubleTree.selected_terms = doubleTree.leftTree.getSelectedTerms();
      doubleTree.selected_parents = doubleTree.rightTree.getSelectedTerms();
      doubleTree.param['voc1'] = doubleTree.leftTree.vocId;
      doubleTree.param['voc2'] = doubleTree.rightTree.vocId;
    }
    else {
      doubleTree.direction = "right-to-left";
      doubleTree.selected_parents = doubleTree.leftTree.getSelectedTerms();
      doubleTree.selected_terms = doubleTree.rightTree.getSelectedTerms();
      doubleTree.param['voc1'] = doubleTree.rightTree.vocId;
      doubleTree.param['voc2'] = doubleTree.leftTree.vocId;
    }

    if (button_value == "translation") {
     doubleTree.param['op'] = 'translation';
     if (doubleTree.selected_terms.length != 1 || doubleTree.selected_parents.length != 1) {
       doubleTree.showMsg(Drupal.t("Select one term per tree to add a new translation."), "error");
       return false;
     }
    }
    else if (button_value == "Switch right" || button_value == "Switch left") {
      doubleTree.param['op'] = 'switch';
      doubleTree.updateWholeTree = true;
    }

    if (doubleTree.selected_terms.length == 0) {
      doubleTree.showMsg(Drupal.t("No terms selected."), "error");
      return false;
    }
    doubleTree.send();
    return false;
  });
}

/**
 * sends selected terms to the server and receives the response message
 */
Drupal.DoubleTree.prototype.send = function() {
  var doubleTree = this;

  $(this.selected_parents).each(function() {
    var tid = Drupal.getTermId(this);
    doubleTree.param['selected_parents['+ tid +']'] = tid;
  });

  $(this.selected_terms).each(function() {
    var tid = Drupal.getTermId(this);
    doubleTree.param['selected_terms['+ tid +']'] = tid;
    var parentID = Drupal.getParentId(this);
    if (typeof(parentID) == "undefined") {
      doubleTree.updateWholeTree = true;
    }
    doubleTree.param['selected_terms_parent['+ tid +']'] = parentID;
  });

  $.ajax({
    data: doubleTree.param,
    type: "POST",
    url: this.url,
    dataType: 'json',
    success: function(response, status) {
      doubleTree.showMsg(response.data, response.type);
      if (response.type == "status" && (doubleTree.param['op'] == "move" || doubleTree.param['op'] == "switch")) {
        doubleTree.updateTrees();
      }
    }
  });
}

/**
 * updates both trees
 */
Drupal.DoubleTree.prototype.updateTrees = function() {
  var doubleTree = this;
  if (this.selected_parents.length == 0 || this.updateWholeTree) {
    //selected terms might be moved to root level, so update whole tree
    var term_to_expand = Drupal.getTermId(this.selected_terms.shift());
    doubleTree.leftTree.loadRootForm(term_to_expand);
    doubleTree.rightTree.loadRootForm(term_to_expand);
  }
  else {
    //remove selected terms and update all parents
    var parents_to_update = new Object();
    $(this.selected_terms).each(function() {
      var parent_tid = Drupal.getParentId(this);
      parents_to_update[parent_tid] = parent_tid;
    });
    $(this.selected_parents).each(function() {
      var tid = Drupal.getTermId(this);
      parents_to_update[tid] = tid
    });
    for (var i in parents_to_update) {
      var tid = parents_to_update[i];
      doubleTree.updateTree(tid);
    }
  }
}

/**
 * shows response msg from ajax request
 */
Drupal.DoubleTree.prototype.showMsg = function(msg, type) {
  var msg_box = '#double-tree-msg';
  $(msg_box).remove();
  $('#taxonomy-manager').before('<div id="double-tree-msg" class="messages '+ type +'">'+ msg +'   <a href="">Close</a></div>');
  $(msg_box).find("a").click(function() {
    $(this).parent().remove();
    return false;
  });
}

/**
 * helper function to update a tree node for a given term id
 */
Drupal.DoubleTree.prototype.updateTree = function(tid) {
  var doubleTree = this;
  var left_li = this.leftTree.getLi(tid);
  var right_li = this.rightTree.getLi(tid);

  this.leftTree.loadChildForm(left_li, true, function(li, tree) {
    doubleTree.updateLi(li, tree);
  });
  this.rightTree.loadChildForm(right_li, true, function(li, tree) {
    doubleTree.updateLi(li, tree);
  });
}

/**
 * fixes displaying of li node after loading child form
 */
Drupal.DoubleTree.prototype.updateLi = function(li, tree) {
  var children = $(li).children("ul");
  if (children.text() == "") {
    $(li).attr("class", "");
    $(li).children("div.hitArea").remove();
  }
  else if ($(li).hasClass("expandable") || $(li).hasClass("lastExpandable")) {
    $(children).hide();
  }
  else if (!$(li).hasClass("collapsable") && !$(li).hasClass("lastCollapsable")) {
    $(li).prepend('<div class="hitArea" />');
    $(li).addClass("collapsable");
    $(li).removeClass("last");
    $(li).children("div.hitArea").click(function() {
      tree.toggleTree(li);
      tree.loadChildForm(li);
    });
  }
}



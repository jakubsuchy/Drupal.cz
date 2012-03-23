/* $Id: faq.js,v 1.1.2.6.2.13 2008/12/22 01:34:51 snpower Exp $ */

function teaser_handler(event) {
  if ($("input[@name=faq_display]:checked").val() != "new_page") {
    if ($("input[@name=faq_use_teaser]:checked").val() == 1) {
      $("input[@name=faq_more_link]").removeAttr("disabled");
    }
    else {
      $("input[@name=faq_more_link]").attr("disabled", "disabled");
    }
  }
}

function faq_display_handler(event) {
  // enable / disable "questions_inline" and "questions_top" only settings
  if ($("input[@name=faq_display]:checked").val() == "questions_inline" || $("input[@name=faq_display]:checked").val() == "questions_top") {
    $("input[@name=faq_back_to_top]").removeAttr("disabled");
  }
  else {
    $("input[@name=faq_back_to_top]").attr("disabled", "disabled");
  }

  // enable / disable "new_page" only settings
  if ($("input[@name=faq_display]:checked").val() != "new_page") {
    $("input[@name=faq_use_teaser]").removeAttr("disabled");
    $("input[@name=faq_more_link]").removeAttr("disabled");
    $("input[@name=faq_disable_node_links]").removeAttr("disabled");
  }
  else {
    $("input[@name=faq_use_teaser]").attr("disabled", "disabled");
    $("input[@name=faq_more_link]").attr("disabled", "disabled");
    $("input[@name=faq_disable_node_links]").attr("disabled", "disabled");
  }
  teaser_handler(event);

  // enable / disable "new_page" and "questions_top" only settings
  if ($("input[@name=faq_display]:checked").val() == "new_page" ||
    $("input[@name=faq_display]:checked").val() == "questions_top") {
    $("select[@name=faq_question_listing]").removeAttr("disabled");
  }
  else {
    $("select[@name=faq_question_listing]").attr("disabled", "disabled");
  }

  // enable / disable "questions_inline" only settings
  if ($("input[@name=faq_display]:checked").val() == "questions_inline") {
    $("input[@name=faq_qa_mark]").removeAttr("disabled");
    // enable / disable label settings according to "qa_mark" setting
    if ($("input[@name=faq_qa_mark]:checked").val() == 1) {
      $("input[@name=faq_question_label]").removeAttr("disabled");
      $("input[@name=faq_answer_label]").removeAttr("disabled");
    }
    else {
      $("input[@name=faq_question_label]").attr("disabled", "disabled");
      $("input[@name=faq_answer_label]").attr("disabled", "disabled");
    }
  }
  else {
    $("input[@name=faq_qa_mark]").attr("disabled", "disabled");
    $("input[@name=faq_question_label]").attr("disabled", "disabled");
    $("input[@name=faq_answer_label]").attr("disabled", "disabled");
  }
}

function qa_mark_handler(event) {
  if ($("input[@name=faq_display]:checked").val() == "questions_inline") {
    // enable / disable label settings according to "qa_mark" setting
    if ($("input[@name=faq_qa_mark]:checked").val() == 1) {
      $("input[@name=faq_question_label]").removeAttr("disabled");
      $("input[@name=faq_answer_label]").removeAttr("disabled");
    }
    else {
      $("input[@name=faq_question_label]").attr("disabled", "disabled");
      $("input[@name=faq_answer_label]").attr("disabled", "disabled");
    }
  }
}

function questions_top_handler(event) {
  $("input[@name=faq_display]").val() == "questions_top" ?
    $("input[@name=faq_group_questions_top]").removeAttr("disabled"):
    $("input[@name=faq_group_questions_top]").attr("disabled", "disabled");

  $("input[@name=faq_display]").val() == "questions_top" ?
    $("input[@name=faq_answer_category_name]").removeAttr("disabled"):
    $("input[@name=faq_answer_category_name]").attr("disabled", "disabled");
}


function child_term_handler(event) {
  if ($("input[@name=faq_hide_child_terms]:checked").val() == 1) {
    $("input[@name=faq_show_term_page_children]").attr("disabled", "disabled");
  }
  else if ($("input[@name=faq_category_display]:checked").val() != "categories_inline") {
    $("input[@name=faq_show_term_page_children]").removeAttr("disabled");
  }
}


function categories_handler(event) {
  if ($("input[@name=faq_display]").val() == "questions_top") {
    $("input[@name=faq_category_display]:checked").val() == "categories_inline" ?
      $("input[@name=faq_group_questions_top]").removeAttr("disabled"):
      $("input[@name=faq_group_questions_top]").attr("disabled", "disabled");
    $("input[@name=faq_category_display]:checked").val() == "new_page" ?
      $("input[@name=faq_answer_category_name]").attr("disabled", "disabled"):
      $("input[@name=faq_answer_category_name]").removeAttr("disabled");
  }
  else {
    $("input[@name=faq_group_questions_top]").attr("disabled", "disabled");
  }

  $("input[@name=faq_category_display]:checked").val() == "categories_inline" ?
    $("input[@name=faq_hide_child_terms]").attr("disabled", "disabled"):
    $("input[@name=faq_hide_child_terms]").removeAttr("disabled");
  $("input[@name=faq_category_display]:checked").val() == "categories_inline" ?
    $("input[@name=faq_show_term_page_children]").attr("disabled", "disabled"):
    $("input[@name=faq_show_term_page_children]").removeAttr("disabled");
  $("input[@name=faq_category_display]:checked").val() == "new_page" ?
    $("select[@name=faq_category_listing]").removeAttr("disabled"):
    $("select[@name=faq_category_listing]").attr("disabled", "disabled");

  child_term_handler();
}

function faq_has_options(obj) {
  if (obj !== null && obj.options !== null) {
    return true;
  }
  return false;
}

function faq_swap_options(obj, i, j) {
  var o = obj.options;
  var i_selected = o[i].selected;
  var j_selected = o[j].selected;
  var temp = new Option(o[i].text, o[i].value, o[i].defaultSelected, o[i].selected);
  var temp2= new Option(o[j].text, o[j].value, o[j].defaultSelected, o[j].selected);
  o[i] = temp2;
  o[j] = temp;
  o[i].selected = j_selected;
  o[j].selected = i_selected;
}

function faq_move_selected_item_up() {
  var obj = document.getElementById("edit-order-no-cats");
  if (!faq_has_options(obj)) {
    return;
  }
  for (i = 0; i < obj.options.length; i++) {
    if (obj.options[i].selected) {
      if (i !== 0 && !obj.options[i-1].selected) {
        faq_swap_options(obj, i, i-1);obj.options[i-1].selected = true;
      }
    }
  }
}

function faq_move_selected_item_down() {
  var obj = document.getElementById("edit-order-no-cats");
  if (!faq_has_options(obj)) {
    return;
  }
  for (i = obj.options.length-1; i >= 0; i--) {
    if (obj.options[i].selected) {
      if (i != (obj.options.length-1) && ! obj.options[i+1].selected) {
        faq_swap_options(obj, i, i+1);
        obj.options[i+1].selected = true;
      }
    }
  }
}

function faq_update_order() {
  var obj = document.getElementById("edit-order-no-cats");
  var ids = [];
  for (var i = 0; i < obj.length; i++) {
    ids[i] = obj.options[i].value;
  }
  var new_order = new String(ids.join(","));
  var form = document.getElementById('faq-weight-settings-form');
  form.faq_node_order.value = new_order;
}

function faq_order_by_date(order) {
  var obj = document.getElementById("edit-order-no-cats");
  if (!faq_has_options(obj)) {
    return;
  }

  var form = document.getElementById('faq-weight-settings-form');
  var date_order = form.faq_node_date_order.value;

  var newIds = date_order.split(",");
  if (order == "ASC") {
    newIds.reverse();
    date_order = new String(newIds.join(","));
  }
  form.faq_node_order.value = date_order;

  var opt_text = [];
  var opts = obj.options;
  var id = 0;
  var i = 0;

  for (i = 0; i < opts.length; i++) {
    id = opts[i].value;
    opt_text[id] = opts[i].text;
  }
  for (i = 0; i < newIds.length; i++) {
    id = newIds[i];
    var opt = new Option(opt_text[id], id);
    obj.options[i] = opt;
  }

}

if (Drupal.jsEnabled) {
  $(document).ready(function () {
    // hide/show answer to question
    $('div.faq-dd-hide-answer').hide();
    $('div.faq-dt-hide-answer').click(function() {
      $(this).next('div.faq-dd-hide-answer').slideToggle();
      return false;
    });


    // hide/show q/a for a category
    $('div.faq-qa-hide').hide();
    $('div.faq-qa-header .faq-header').click(function() {
      $(this).parent().next('div.faq-qa-hide').slideToggle();
      return false;
    });



    // handle faq_category_settings_form
    faq_display_handler();
    questions_top_handler();
    categories_handler();
    teaser_handler();
    $("input[@name=faq_display]").bind("click", faq_display_handler);
    $("input[@name=faq_qa_mark]").bind("click", qa_mark_handler);
    $("input[@name=faq_use_teaser]").bind("click", teaser_handler);
    $("input[@name=faq_category_display]").bind("click", categories_handler);
    $("input[@name=faq_hide_child_terms]").bind("click", child_term_handler);


  });
}



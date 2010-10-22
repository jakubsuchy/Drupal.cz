var commentbox = ".comment";
var ctrl = false;
var last_submit;
var speed = 'fast';
var ahah = false;
var firsttime_init = true;

/**
 * Attaches the ahah behavior to each ahah form element.
 */
Drupal.behaviors.ajax_comments = function(context) {
  $('#comment-form:not(.ajax-comments-processed)', context).addClass('ajax-comments-processed').each(function() {
    form = $(this);
    // prepare the form when the DOM is ready
    if ((Drupal.settings.rows_default == undefined) || (!Drupal.settings.rows_default)) {
      Drupal.settings.rows_default = $('textarea', form).attr('rows');
    }
    $('textarea', form).attr('rows', Drupal.settings.rows_default);
    if ((Drupal.settings.rows_in_reply == undefined) || (!Drupal.settings.rows_in_reply)) {
      Drupal.settings.rows_in_reply = Drupal.settings.rows_default;
    }
    if (Drupal.settings.always_expand_main_form == undefined) {
      Drupal.settings.always_expand_main_form = true;
    }
    if (Drupal.settings.blink_new == undefined) {
      Drupal.settings.blink_new = true;
    }

    $('#edit-upload', form).bind('change', function(){
      $('#ajax-comments-submit,#ajax-comments-preview', form).attr('disabled', 1);
    });
    
    // It's not possible to use 'click' or 'submit' events for ahah sumits, so
    // we should emulate it by up-down events. We need to check which elements
    // are actually clicked pressed, to make everything work correct.
    $('#ajax-comments-submit,#ajax-comments-preview', form).bind('mousedown keydown', function() { last_submit = $(this).attr('id'); });
    $('#ajax-comments-submit,#ajax-comments-preview', form).bind('mouseup', function() {
      if (last_submit == $(this).attr('id')) {
        ajax_comments_show_progress(context);
        ajax_comments_update_editors();
      }
    });
    $('#ajax-comments-submit,#ajax-comments-preview', form).bind('keyup', function(event) {
      if (last_submit == $(this).attr('id') && event.keyCode == 13) {
        ajax_comments_show_progress(context);
        ajax_comments_update_editors();
      }
    });
    
    // enable comments buttons back when attachement is uploaded
    $('#edit-attach', form).bind('mousedown keydown', function() {
      if (last_submit == $(this).attr('id')) {
        $('#ajax-comments-submit,#ajax-comments-preview', form).removeAttr('disabled');
      }
    });

    // initializing main form
    action = form.attr('action');

    // Creating title link
    form.parents(".box").find("h2:not(.ajax-comments-processed),h3:not(.ajax-comments-processed),h4:not(.ajax-comments-processed)").addClass('ajax-comments-processed').each(function(){
      title = $(this).html();
      $(this).html('<a href="'+action+'" id="comment-form-title">'+title+'</a>');
      $(this).parents(".box").find(".content").attr('id','comment-form-content').removeClass("content");
    });

    // Expanding form if needed
    page_url = document.location.toString();
    fragment = '';
    if (page_url.match('#')) {
      fragment = page_url.split('#')[1];
    }

    if ((fragment == 'comment-form'  || Drupal.settings.always_expand_main_form) && firsttime_init) {
      $('#comment-form-title', context).addClass('pressed');
      $('#comment-form-content').attr('cid', 0);
    }
    else {
      // fast hide form
      $('#comment-form-content', context).hide();
    }
    
    // Attaching event to title link
    $('#comment-form-title:not(.ajax-comments-processed)', context).addClass('ajax-comments-processed').click(reply_click);
    //moving preview in a proper place
    $('#comment-form-content').parents('.box').before($('#comment-preview'));
    if (!$('#comment-form-content').attr('cid')) {
      $('#comment-form-content').attr('cid', -1);
    }
    
    if(typeof(fix_control_size)!='undefined'){ fix_control_size(); }
  });
  
  $('.comment_reply a:not(.ajax-comments-processed)', context).addClass('ajax-comments-processed').click(reply_click);
  $('.quote a:not(.ajax-comments-processed)', context).addClass('ajax-comments-processed').each(function(){
    href = $(this).attr('href');
    if (ajax_comments_is_reply_to_node(href)) {
      $(this).click(function(){ $('#comment-form-title', context).click(); return false; });
    } else {
      $(this).click(reply_click);
    }
  });
  
  // We should only bind ajax deletion on links with tokens to avoid CSRF attacks
  $('.comment_delete a:not(.ajax-comments-processed)', context).each(function (){
    href = $(this).attr('href');
    if (href.indexOf('token=') > -1) {
      $(this).addClass('ajax-comments-processed').click(delete_click);
    }
  });

  // add Ctrl key listener for deletion feature
  $(window).keydown(function(e) {
    if(e.keyCode == 17) {
      ctrl = true;
    }
  });
  $(window).keyup(function(e) {
    ctrl = false;
  });
  
  firsttime_init = false;
};



// Reply link handler
function reply_click() {
  // We should only handle non presed links
  if (!$(this).is('.pressed')){
    action = $(this).attr('href');
    link_cid = ajax_comments_get_cid_from_href(action);
    rows = Drupal.settings.rows_default;
    if ($('#comment-form-content').attr('cid') != link_cid) {
      // We should remove any WYSIWYG before moving controls
      ajax_comments_remove_editors();
      
      // move form from old position
      if (ajax_comments_is_reply_to_node(action)) {
        $('#comment-form').removeClass('indented');
        if ($('#comment-form-content:visible').length) {
          $('#comment-form-content').after('<div style="height:' + $('#comment-form-content').height() + 'px;" class="sizer"></div>');
          $('.sizer').slideUp(speed, function(){ $(this).remove(); });
        }
        $(this).parents('h2,h3,h4').after($('#comment-form-content'));
        rows = Drupal.settings.rows_default;
        $('#comment-form-content').parents('.box').before($('#comment-preview'));
      }
      else {
       $('#comment-form').addClass('indented');
        if ($('#comment-form-content:visible').length) {
          $('#comment-form-content').after('<div style="height:' + $('#comment-form-content').height() + 'px;" class="sizer"></div>');
          $('.sizer').slideUp(speed, function(){ $(this).remove(); });
        }
        $(this).parents(commentbox).after($('#comment-form-content'));
        rows = Drupal.settings.rows_in_reply;
        $('#comment-form-content').prepend($('#comment-preview'));
      }
      $('#comment-form-content').hide();
    }

    // We don't need to load everything twice
    if (!$(this).is('.last-clicked')) {
      // Going further
      initForm(action, false, rows);
    }
    // ...and show the form after everything is done
    ajax_comments_expand_form();
    
    $('.pressed').removeClass('pressed');
    $(this).addClass('pressed');
    $('.last-clicked').removeClass('last-clicked');
    $(this).addClass('last-clicked');
    $('#comment-form-content').attr('cid', link_cid);
  }
  else {
    // handling double click
    if ((!$(this).is('#comment-form-title')) && (Drupal.settings.always_expand_main_form)) {
      $('#comment-form-title').click();
    } else {
      ajax_comments_close_form();
    }
  }

  if (typeof(fix_control_size) != 'undefined'){ fix_control_size(); }
  return false;
}

// Helper fnction for reply handler
function initForm(action, needs_reload, rows){
  // resizing and clearing textarea
  $('#comment-form textarea').attr('rows', rows);
  $('#comment-form textarea').attr('value','');

  // clearing form
  $('#comment-preview').empty();
  $('#comment-form .error').removeClass('error');

  // * getting proper form tokens

  // specially for Opera browser
  action = action.replace('http:// ','');
  fragments = action.split('#');
  queries = fragments[0].split('?');
  
  fragment = '';
  query = '';
  if (fragments[1]) { fragment = '#' + fragments[1]; }
  if (queries[1]) { query = '?' + queries[1]; }
  
  cid = ajax_comments_get_cid_from_href(action);
  nid = ajax_comments_get_nid_from_href(action);

  if (!needs_reload) {
    needs_reload = (action.indexOf('ajaxreload=1') != -1);
    needs_reload = needs_reload || (action.indexOf('quote=') != -1);
  }
  
  // disabling buttons while loading tokens
  $('#comment-form .form-submit').addClass('ajax-comments-disabled').attr('disabled', 1);
  // if will be realoaded, we should disable everything
  if (needs_reload) {
    ajax_comments_show_progress();
    $('#comment-form input, #comment-form textarea').addClass('ajax-comments-disabled').attr('disabled', 1);
  }

  // sending ajax call to get the token
  var token = 0;
  $.ajax({
    type: "GET",
    url: Drupal.settings.basePath + "ajax_comments/get_form_token/" + nid + '/' + cid + query + fragment,
    success: function(form){
      f = $('<div></div>');
      f.html(form);
      form = $('form', f);

      // Going further
      initForm_setTokens(form, needs_reload, rows);
    }
  });
}

// Second helper function for Reply handler
function initForm_setTokens(varform, needs_reload, rows){
  action = varform.attr('action');
  token = $("#edit-form-token", varform).val();
  bid = $("input[name=form_build_id]", varform).val();
  captcha = $(".captcha", varform).html();

  form = $('#comment-form-content > #comment-form');

  // Refresh form tokens
  if (token) {
    $('#edit-form-token', form).attr('value', token);
  }
  // ...and build ids
  if (bid) {
    $('input[name=form_build_id]', form).val(bid);
    $('input[name=form_build_id]', form).attr('id', bid);
  }
  // ...and captcha
  if (captcha) {
    $('.captcha', form).html(captcha);
    Drupal.attachBehaviors($('.captcha', form));
  }
  // ...and action
  if (action) {
    form.attr('action', action);
  }

  if (needs_reload) {
    // ensure that editors were removed
    ajax_comments_remove_editors();
    
    ajax_comments_hide_progress();
    form.html('<div>' + $("div", varform).html() + '</div>');
    form.removeClass('ajax-comments-processed');

    Drupal.attachBehaviors($('#comment-form-content'));

    // resizing textarea
    $('textarea', form).attr('rows', rows);
  }
  // now we can attach previously removed editors
  ajax_comments_attach_editors();

  // enabling form controls again
  $('.ajax-comments-disabled', form).removeAttr('disabled');

  // ensure that attachment is uploaded
  if ($('#edit-upload').length && $('#edit-upload', form).val()) {
    $('#ajax-comments-submit,#ajax-comments-preview', form).attr('disabled', 1);
    parent_fieldset = $('#edit-upload', form).parents('fieldset');
    if (parent_fieldset.is('.collapsed')) {
      Drupal.toggleFieldset(parent_fieldset);
    }
  }
}


// delete links handler
function delete_click() {
  if ((ctrl) || (confirm(Drupal.t('Are you sure you want to delete the comment? Any replies to this comment will be lost. This action cannot be undone.')))) {
    // taking link's href as AJAX url
    comment = $(this).parents(commentbox);
    action = $(this).attr('href');
    action = action.replace(/comment\/delete\//, 'ajax_comments/instant_delete/');
    if (action) {
      $(this).parents(commentbox).fadeTo(speed, 0.5);
      $.ajax({
        type: "GET",
        url: action,
        success: function(result){
          if (result == 'OK') {
            ajax_comments_close_form();

            // if comment form is expanded on this module, we should collapse it first
            if (comment.next().is('#comment-form-content')) {
              thread = comment.next().next('.indented, div > .indented');
            } else {
              thread = comment.next('.indented, div > .indented');
            }
            thread.animate({height:'hide', opacity:'hide'}, speed);
            comment.animate({height:'hide', opacity:'hide'}, speed, function(){
              thread.remove();
              comment.remove();
              if (!$(commentbox).length) {
                $('#comment-controls').animate({height:'hide', opacity:'hide'}, speed, function(){ $(this).remove(); });
              }
            });
          }
          else {
            alert('Sorry, token error.');
          }
        }
      });
    }
  }
  return false;
}



/*
$('#comments .pager a').bind('click', function(){
  href = $(this).attr('href');
  page = href.split('?');
  alert(123);
})*/




// ====================================
// Misc. functions
// ====================================

function ajax_comments_expand_form(focus) {
  $('#comment-form-content').animate({height:'show'}, speed, function() {
    if (focus) {
      $('#comment-form textarea').focus();
    }
    if ($.browser.msie) this.style.removeAttribute('filter'); 
  });
}

function ajax_comments_close_form(reinit) {
  $('#comment-form-content').animate({height:'hide'}, speed, function(){
    if (reinit) {
      initForm($('#comment-form-title').attr('href'), true, Drupal.settings.rows_default);
    }
  });
  $('.pressed').removeClass('pressed');
  $('#comment-form-content').attr('cid', -1);
  ajax_comments_hide_progress();
}




// AHAH effect for comment previews
jQuery.fn.ajaxCommentsPreviewToggle = function() {
  var obj = $(this[0]);

  // hiding previous previews
  $('#comment-preview > div:visible').animate({height:'hide', opacity:'hide'}, speed, function() { $(this).remove(); } );
  // showing fresh preview
  $('#comment-preview').show();
  obj.animate({height:'show', opacity:'show'}, speed);
  ajax_comments_hide_progress();

  // Add submit button if it doesn't added yet
  if (!$('#ajax-comments-submit').length && $('.preview-item').length) {
    $('#ajax-comments-preview').after('<input name="op" id="ajax-comments-submit" value="'+ Drupal.t("Save") +'" class="form-submit" type="submit">');
    // re-attaching to new comment
    Drupal.attachBehaviors($('#ajax-comments-submit'));
  }
};

// AHAH effect for comment submits
jQuery.fn.ajaxCommentsSubmitToggle = function() {
  var obj = $(this[0]);

  html = obj.html();
  if (html.indexOf('comment-new-success') > -1) {
    
    // empty any preview before output comment
    $('#comment-preview').slideUp(speed, function(){ $(this).empty(); });
    
    // place new comment in proper place
    insert_new_comment(obj);
    
    // at last - showing it up
    obj.animate({height:'show', opacity:'show'}, speed, function () {
      if ($.browser.msie) {
        height = document.documentElement.offsetHeight ;
      } else if (window.innerWidth && window.innerHeight) {
        height = window.innerHeight;
      }
      height = height / 2;
      offset = obj.offset();
      if ((offset.top > $('html').scrollTop() + height) || (offset.top < $('html').scrollTop() - 20)) {
        $('html').animate({scrollTop: offset.top - height}, 'slow', function(){
          // Blink a little bit to user, so he know where's his comment
          if (Drupal.settings.blink_new) {
            obj.fadeTo('fast', 0.2).fadeTo('fast', 1).fadeTo('fast', 0.5).fadeTo('fast', 1).fadeTo('fast', 0.7).fadeTo('fast', 1, function() { if ($.browser.msie) this.style.removeAttribute('filter'); });
          }
        });
      }
      else {
        if (Drupal.settings.blink_new) {
          obj.fadeTo('fast', 0.2).fadeTo('fast', 1).fadeTo('fast', 0.5).fadeTo('fast', 1).fadeTo('fast', 0.7).fadeTo('fast', 1, function() { if ($.browser.msie) this.style.removeAttribute('filter'); });
        }
      }
      if ($.browser.msie) this.style.removeAttribute('filter');
    });

    // re-attaching to new comment
    Drupal.attachBehaviors(html);
    
    // hiding comment form
    ajax_comments_close_form(true);
  } else {
    $('#comment-preview').append(obj);
    obj.ajaxCommentsPreviewToggle(speed);
  }
};

// remove editors from comments textarea (mostly to re-attach it)
function ajax_comments_remove_editors() {
  ajax_comments_update_editors();
  if (typeof(Drupal.wysiwyg) != undefined) {
    $('#comment-form input.wysiwyg-processed:checked').each(function() {
      var params = Drupal.wysiwyg.getParams(this);
      Drupal.wysiwygDetach($(this), params);
    });
    return;
  }
  
  if (typeof(tinyMCE) != 'undefined') {
    if (tinyMCE.getInstanceById("edit-comment")) {
      tinyMCE.execCommand('mceRemoveControl', false, "edit-comment");
    }
  }
}

// attach editors to comments textarea if needed
function ajax_comments_attach_editors() {
  if (typeof(Drupal.wysiwyg) != undefined) {
    $('#comment-form input.wysiwyg-processed:checked').each(function() {
      var params = Drupal.wysiwyg.getParams(this);
      Drupal.wysiwygAttach($(this), params);
    });
    return;
  }

  if (typeof(tinyMCE) != 'undefined') {
    // ugly hack to get invisible element's width
    /*height = $('#comment-form-content').css('height');
    overflow = $('#comment-form-content').css('overflow');
    $('#comment-form-content').css('height', '0px');
    $('#comment-form-content').css('overflow', 'hidden');
    $('#comment-form-content').show();*/
    
    tinyMCE.execCommand('mceAddControl', false, "edit-comment");
    
    // returning old values
   /* $('#comment-form-content').css('height', height);
    $('#comment-form-content').css('overflow', overflow);
    $('#comment-form-content').hide();*/
  }
}

// update editors text to their textareas. Need to be done befor submits
function ajax_comments_update_editors() {
  // update tinyMCE
  if (typeof(tinyMCE) != 'undefined') {
    tinyMCE.triggerSave();
  }
  
  // update FCKeditor
  if (typeof(doFCKeditorSave) != 'undefined') {
    doFCKeditorSave();
  }
  if(typeof(FCKeditor_OnAfterLinkedFieldUpdate) != 'undefined'){
    FCKeditor_OnAfterLinkedFieldUpdate(FCKeditorAPI.GetInstance('edit-comment'));
  }
}

function ajax_comments_get_cid_from_href(action) {
  args = get_args(action);

  // getting token params (/comment/delete/!cid!)
  if (args[1] == 'delete') {
    cid = args[2];
  }
  // getting token params (/comment/reply/nid/!cid!)
  else {
    if (typeof(args[3]) == 'undefined') {
      cid = 0;
    }
    else {
      cid = args[3];
    }
  }
  return cid;
}

function ajax_comments_get_nid_from_href(action) {
  args = get_args(action);

  if (typeof(args[2]) == 'undefined') {
    nid = 0;
  }
  else {
    nid = args[2];
  }
  return nid;
}

function ajax_comments_is_reply_to_node(href) {
  args = get_args(href);
  result = args[1] == 'reply' && args[2] && (typeof(args[3]) == 'undefined');
  return result;
}

function get_args(url) {
  if (Drupal.settings.clean_url == '1') {
    var regexS = "(http(s)*:\/\/)*([^/]*)"+ Drupal.settings.basePath +"([^?#]*)";
    var regex = new RegExp( regexS );
    var results = regex.exec( url );
    args = results[4];
  }
  else {
    var regexS = "([&?])q=([^#&?]*)";
    var regex = new RegExp( regexS );
    var results = regex.exec( url );
    args = results[2];
  }
  args = args.split('/');
  if (Drupal.settings.language_mode == 1 || Drupal.settings.language_mode == 2) {
    for (l in Drupal.settings.language_list) {
      if (args[0] == Drupal.settings.language_list[l].language) {
        args.shift();
        break;
      }
    }
  }
  return args;
}

function ajax_comments_show_progress(context) {
  if (!context) {
    context = '#comment-form-content';
  }
  if (!$('#comment-form .ajax-comments-loader', context).length) {
    $('#comment-form', context).append('<div class="ajax-comments-loader"></div>');
  }
}
function ajax_comments_hide_progress(context) {
  if (!context) {
    context = '#comment-form-content';
  }
  $('#comment-form .ajax-comments-loader', context).fadeOut(speed, function(){ $(this).remove(); });
}

function insert_new_comment(comment) {
  if ($('#comment-form-content').attr('cid') == 0) {
    $('#comment-preview').before(comment);
  }
  else {
    if ($('#comment-form-content').next().is('.indented')) {
      $('#comment-form-content').next().append(comment);
    }
    else {
      $('#comment-form-content').before(comment);
      comment.wrap('<div class="indented"></div>');
    }
  }
}


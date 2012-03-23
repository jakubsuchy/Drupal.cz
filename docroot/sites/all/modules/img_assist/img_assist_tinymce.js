/*
 This javascript file allows img_assist to work with TinyMCE via the drupalimage plugin
 for TinyMCE.  This file will be used instead of img_assist_textarea.js when img_assist is called
 from the drupalimage plugin.  Additional JS files similar to img_assist_textarea.js and img_assist_tinymce.js
 could be created for using img_assist with other WYSIWYG editors.  Some minor code changes
 to the menu function in img_assist.module will be necessary, at least in img_assist_menu()
 and img_assist_loader().
 */

// This doesn't work right.  The tiny_mce_popup.js file needs to be loaded BEFORE any setWindowArg() commands are issued
//document.write('<sc'+'ript language="javascript" type="text/javascript" src="' + BASE_URL + 'modules/tinymce/tinymce/jscripts/tiny_mce/tiny_mce_popup.js"><\/script>'); 

// get variables that were passed to this window from the tinyMCE editor
var nid;
var captionTitle;
var captionDesc;
var link;
var align;
var width;
var height;

function preinit() {
  // Initialize
  tinyMCE.setWindowArg('mce_windowresize', false);
  tinyMCE.setWindowArg('mce_replacevariables', false);
}

function initLoader() {
  nid          =      tinyMCE.getWindowArg('nid');
  captionTitle = '' + tinyMCE.getWindowArg('captionTitle');
  captionDesc  = '' + tinyMCE.getWindowArg('captionDesc');
  link         = '' + tinyMCE.getWindowArg('link');
  align        = '' + tinyMCE.getWindowArg('align');
  width        = '' + tinyMCE.getWindowArg('width');
  height       = '' + tinyMCE.getWindowArg('height');

  if (nid > 0) {
    frames['img_assist_main'].window.location.href = BASE_URL + 'index.php?q=img_assist/properties/' + nid + '/edit';
  } else {
    frames['img_assist_main'].window.location.href = BASE_URL + 'index.php?q=img_assist/thumbs/myimages';
  }
}

function initProperties() {
  var formObj = frames['img_assist_main'].document.forms[0];
  if (formObj['edit[edit]'].value == 1) {
    formObj['edit[title]'].value  = captionTitle;
    formObj['edit[desc]'].value   = captionDesc;
    link = link.split(',');
    formObj['edit[link]'].value = link[0];
    if (link[0] == 'url') {
      formObj['edit[url]'].value = link[1];
    }
    formObj['edit[align]'].value  = align;
    
    // When editing the properties of an image placed with 
    // img_assist, it's not easy to figure out what standard
    // size was used.  Until such code is written we will 
    // just set the size to "other".  Of course, if custom
    // isn't an option then I guess the image size will default
    // back to thumbnail.
    formObj['edit[size_label]'].value = "other";
    formObj['edit[width]'].value  = width;
    formObj['edit[height]'].value = height;
    
    //formObj.insert.value = 'Update'; //tinyMCE.getWindowArg('action');
  }
  setHeader('properties');
  updateCaption();
  onChangeLink();
  onChangeSizeLabel();
}

function initThumbs() {
  setHeader('browse');
}

function initHeader() {
}

function initUpload() {
  setHeader('uploading');
}

function getFilterTag(formObj) {
  nid = formObj['edit[nid]'].value
  captionTitle = formObj['edit[title]'].value;
  captionDesc  = formObj['edit[desc]'].value;
  link         = formObj['edit[link]'].value;
  if (link == 'url') {
    link = link + ',' + formObj['edit[url]'].value;
  }
  align        = formObj['edit[align]'].value;
  width        = formObj['edit[width]'].value;
  height       = formObj['edit[height]'].value;
  
  // Create the image placeholder tag
  //  (see the TinyMCE_drupalimage_cleanup() function in the drupalimage plugin for documentation)
  var miscAttribs = 'nid=' + nid + '|title=' + captionTitle + '|desc=' + captionDesc + '|link=' + link;
  miscAttribs = encodeURIComponent(miscAttribs);
  var html    = '';
  html += ''
       + '<img src="' + (tinyMCE.getParam("theme_href") + "/images/spacer.gif") + '"'
       + ' width="' + width + '" height="' + height + '" align="' + align + '"'
       + ' alt="' + miscAttribs + '" title="' + miscAttribs + '" name="mceItemDrupalImage" class="mceItemDrupalImage" />';
  
  return html;
}

function insertToEditor(content) {
  // Insert the image
  tinyMCE.execCommand("mceInsertContent", true, content);
  tinyMCE.selectedInstance.repaint();
  
  // Close the dialog
  tinyMCEPopup.close();
  return false;
}

function cancelAction() {
  // Close the dialog
  tinyMCEPopup.close();
}

// While loading
preinit();
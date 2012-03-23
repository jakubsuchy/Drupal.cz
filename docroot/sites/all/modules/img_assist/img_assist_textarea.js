/*
 This javascript file allows img_assist to work with a plain textarea.  This file will be 
 used instead of tinymce.js.  Additional JS files similar to textarea.js and tinymce.js
 could be created for using img_assist with other WYSIWYG editors.  Some minor code changes
 to the menu function in img_assist.module will be necessary, at least in img_assist_menu()
 and img_assist_loader().
 */

// Declare global variables
var myDoc;
var myForm;
var myTextarea;
var hasInputFormat;

function initLoader() {
  // Save the references to the parent form and textarea to be used later. 
  myDoc      = window.opener.document; // global (so don't use var keyword)
  myForm     = '';
  myTextarea = '';
  hasInputFormat = false;
  
  var args = getArgs(); // get the querystring arguments
  var textarea = args.textarea;
  
  // Reference the form object for this textarea.
  if (myDoc.getElementsByTagName) {
    var f = myDoc.getElementsByTagName('form');
    for (var i=0; i<f.length; i++) {
      // Is this textarea is using an input format?
      if (f[i]['edit[format]']) {
        hasInputFormat = true;
      }
      if (f[i][textarea]) {
        myForm = f[i];
        myTextarea = f[i][textarea];
        break;
      }
    }
  }
  frames['img_assist_main'].window.location.href = BASE_URL + 'index.php?q=img_assist/thumbs/myimages';
}

function initProperties() {
  setHeader('properties');
  updateCaption();
  onChangeLink();
  onChangeSizeLabel();
}

function initThumbs() {
  setHeader('browse');
}

function initHeader() {}

function initUpload() {
  setHeader('uploading');
}

function getFilterTag(formObj) {
  var nid          = formObj['edit[nid]'].value
  var captionTitle = formObj['edit[title]'].value;
  var captionDesc  = formObj['edit[desc]'].value;
  var link         = formObj['edit[link]'].value;
  if (link == 'url') {
    link = link + ',' + formObj['edit[url]'].value;
  }
  var align         = formObj['edit[align]'].value;
  var width        = formObj['edit[width]'].value;
  var height       = formObj['edit[height]'].value;
  
  // Create the image tag
  var miscAttribs = 'nid=' + nid + '|title=' + captionTitle + '|desc=' + captionDesc + '|link=' + link;
  var content = '[img_assist|' + miscAttribs + '' + '|align=' + align + '|width=' + width + '|height=' + height + ']';
  
  return content;
}

function insertToEditor(content) {
  // Insert the image
  /* IE */
  if (myDoc.selection) {
    myTextarea.focus();
    cursor = myDoc.selection.createRange();
    cursor.text = content;
    //myForm.insert.focus();
    
  } else if (myTextarea.selectionStart || myTextarea.selectionStart == "0") { /* Gecko-based engines: Mozilla, Camino, Firefox, Netscape */
    var startPos  = myTextarea.selectionStart;
    var endPos    = myTextarea.selectionEnd;
    var body      = myTextarea.value;  
    myTextarea.value = body.substring(0, startPos) + content + body.substring(endPos, body.length);
    
  } else { /* Worst case scenario: browsers that don't know about cursor position, Safari, OmniWeb, Konqueror */
    myTextarea.value += content;
  }
  
  // Close the dialog
  cancelAction();
  return false;
}

function cancelAction() {
  // Close the dialog
  window.close();
}

/*
 * getArgs() by Jim K - From Orielly JSB pp 244
 *
 * This function parses comma separated name=value 
 * argument pairs from the query string of the URL. 
 * It stores the name=value pairs in 
 * properties of an object and then returns that object
 * 
 * ex usage: 
 * var args = getArgs(); //Get the arguments
 * alert(args.CSSPATH);
*/
function getArgs() {
  var args = new Object();

  var query = location.search.substring(1); // Get Query String
  var pairs = query.split("&"); // Split query at the ampersand
  
  for(var i = 0; i < pairs.length; i++) { // Begin loop through the querystring
    var pos = pairs[i].indexOf('='); // Look for "name=value"
    if (pos == -1) continue; // if not found, skip to next
    
    var argname = pairs[i].substring(0,pos); // Extract the name
    var value = pairs[i].substring(pos+1); // Extract the value
    args[argname] = unescape(value); // Store as a property
  }
  return args; // Return the Object
}

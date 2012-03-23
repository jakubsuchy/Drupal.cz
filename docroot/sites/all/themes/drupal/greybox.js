/* Greybox Redux
 * jQuery required: http://jquery.com/
 * Written by: John Resig
 * License: GPL
 */

var GB_ANIMATION = true;
$(document).ready(function(){
    $("a.greybox").click(function(){
      var t = this.title || $(this).text() || this.href;
      GB_show(t,this.href,470,900);
      return false;
      });
    });
var GB_DONE = false;
var GB_HEIGHT = 600;
var GB_WIDTH = 900;

function GB_show(caption, url, height, width) {
  GB_HEIGHT = height || 600;
  GB_WIDTH = width || 900;
  if(!GB_DONE) {
    $(document.body)
      .append("<div id='GB_overlay'></div><div id='GB_window'><div id='GB_caption'></div>"
        + "<img src='" + Drupal.settings.greybox.sitepath +"/images/close.gif' alt='Close window'/></div>");
    $("#GB_window img").click(GB_hide);
    $("#GB_overlay").click(GB_hide);
    $(window).resize(GB_position);
    GB_DONE = true;
  }

  $("#GB_frame").remove();
  $("#GB_window").append("<iframe id='GB_frame' src='"+url+"'></iframe>");

  $("#GB_caption").html(caption);
  $("#GB_overlay").show();
  GB_position();

  if(GB_ANIMATION)
    $("#GB_window").slideDown("slow");
  else
    $("#GB_window").show();
}

function GB_hide() {
  $("#GB_window,#GB_overlay").hide();
}

function GB_position() {
  var de = document.documentElement;
  var w = self.innerWidth || (de&&de.clientWidth) || document.body.clientWidth;
  $("#GB_window").css({width:GB_WIDTH+"px",height:GB_HEIGHT+"px",
    left: ((w - GB_WIDTH)/2)+"px" });
  $("#GB_frame").css("height",GB_HEIGHT - 32 +"px");
}

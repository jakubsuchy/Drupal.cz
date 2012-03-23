// $Id: username_check_auto.js,v 1.5 2009/01/23 09:24:03 duke Exp $

Drupal.behaviors.usernameCheckAutoBehavior = function (context) {
  $('#username-check-wrapper input:not(.username-check-behavior-processed)', context)
    .addClass('username-check-behavior-processed')
    .each(function () {
      var loginField = $(this);
      
      var usernamePos = loginField.position();
      var usernameWidth = loginField.width();
      
      Drupal.usernameCheckUsername = '';
      $('#username-check-informer')
        .css({left: (usernamePos.left+usernameWidth+10)+'px', top: (usernamePos.top)+'px'})
        .show(); 
      
      loginField
        .keyup(function () {
          if(loginField.val() != Drupal.usernameCheckUsername) {
            clearTimeout(Drupal.usernameCheckTimer);
            Drupal.usernameCheckTimer = setTimeout(function () {usernameCheck(loginField)}, Drupal.settings.usernameCheck.delay*1000);
          
            if(!$("#username-check-informer").hasClass('username-check-informer-progress')) {
              $("#username-check-informer")
                .removeClass('username-check-informer-accepted')
                .removeClass('username-check-informer-rejected');
            }
              
            $("#username-check-message")
              .hide();
          }
        })
        .blur(function () {
          if(loginField.val() != Drupal.usernameCheckUsername) {
            usernameCheck(loginField);
          }
        });    
    });
}

function usernameCheck(loginField) {
  clearTimeout(Drupal.usernameCheckTimer);
  Drupal.usernameCheckUsername = loginField.val();
  
  $.ajax({
    url: Drupal.settings.usernameCheck.ajaxUrl,
    data: {username: Drupal.usernameCheckUsername},
    dataType: 'json',
    beforeSend: function() {
      $("#username-check-informer")
        .removeClass('username-check-informer-accepted')
        .removeClass('username-check-informer-rejected')
        .addClass('username-check-informer-progress');
    },
    success: function(ret){
      if(ret['allowed']){
        $("#username-check-informer")
          .removeClass('username-check-informer-progress')
          .addClass('username-check-informer-accepted');
        
        loginField
          .removeClass('error');
      }
      else {
        $("#username-check-informer")
          .removeClass('username-check-informer-progress')
          .addClass('username-check-informer-rejected');
        
        $("#username-check-message")
          .addClass('username-check-message-rejected')
          .html(ret['msg'])
          .show();
      }
    }
   });
}
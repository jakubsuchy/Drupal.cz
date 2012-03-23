// $Id: username_check_manual.js,v 1.4 2009/01/23 09:24:03 duke Exp $

Drupal.behaviors.usernameCheckManualBehavior = function (context) {
  $('#edit-username-check-button:not(.username-check-behavior-processed)', context)
    .addClass('username-check-behavior-processed')
    .show()
    .click(function () {
      var loginField = $("#username-check-wrapper input");
      
      $.ajax({
        url: Drupal.settings.usernameCheck.ajaxUrl,
        data: {username: loginField.val()},
        dataType: 'json',
        beforeSend: function() {
          $("#username-check-message")
            .removeClass('username-check-message-accepted')
            .removeClass('username-check-message-rejected')
            .addClass('username-check-message-progress')
            .html(Drupal.settings.usernameCheck.msgWait)
            .show();
        },
        success: function(ret){
          if(ret['allowed']){
            $("#username-check-message")
              .removeClass('username-check-message-progress')
              .addClass('username-check-message-accepted');
            
            loginField
              .removeClass('error');
          }
          else {
            $("#username-check-message")
              .removeClass('username-check-message-progress')
              .addClass('username-check-message-rejected');
          }
          $("#username-check-message")
            .removeClass('username-check-message-progress')
            .html(ret['msg']);
        }
      });
      
      return false;
    });
}
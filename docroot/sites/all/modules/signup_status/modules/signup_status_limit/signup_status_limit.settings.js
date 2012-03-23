/* $Id: signup_status_limit.settings.js,v 1.1 2009/09/16 18:04:32 dww Exp $ */

/**
 * Hide/show the right limit fields on the node signup settings page.
 */
Drupal.behaviors.signupStatusLimitShowLimitNodeSettings = function () {
  $('div.signup-limit-radios input[type=radio]').click(function () {
    if (this.value == 'global') {
      $('div.signup_global_limit').show();
      $('div.signup_status_limit_node_limits').hide();
    }
    else if (this.value == 'status') {
      $('div.signup_global_limit').hide();
      $('div.signup_status_limit_node_limits').show();
    }
  });
};


/* $Id: signup_status_mailer.settings.js,v 1.1 2009/08/23 16:02:32 dww Exp $ */

/**
 * @file
 * JavaScript to help the Signup Status Mailer settings UI not suck so much.
 */

/**
 * Attach to the "E-mail notifications" select boxes and change other elements.
 *
 * - "Disabled" ('none'): Hide the e-mail fields and help text.
 * - "Enabled" on the side-wide settings page ('default'): Show the
 *   e-mail fields.
 * - "Enabled (custom)" ('custom'): Show the e-mail fields and enable
 *   them, hide the help text.
 * - "Use default (currently 'Enabled')" ('default-enabled'): Show but
 *   disable the e-mail fields, and show the help text.
 * - "Use default (currently 'Disabled')" ('default-disabled'): Hide
 *   the e-mail fields but show the help text.
 */
Drupal.behaviors.signupStatusMailerNotificationSetting = function() {
  $('.signup_status_mailer_notification_select').click(function() {
    /* Remember which select box form element was clicked. */
    var self = this;
    /* Find the DOM element that holds all the related elements for the same
       action and status that belong to the select box that was clicked. */
    var self_action = $(self).parent().parent();
    /* Depending on the value of the select box, hide/show the right things. */
    if (self.value == 'none') {
      $(self_action).find('div.notification-description').hide();
      $(self_action).find('div.email-settings').hide();
    }
    else if (self.value == 'default') {
      $(self_action).find('div.email-settings').show();
    }
    else if (self.value == 'custom') {
      $(self_action).find('div.notification-description').hide();
      $(self_action).find('div.email-settings').show();
      $(self_action).find('div.email-settings input[type=text]').removeAttr("disabled");
      $(self_action).find('div.email-settings .form-textarea').removeAttr("disabled");
    }
    else if (self.value == 'default-enabled') {
      $(self_action).find('div.notification-description').show();
      $(self_action).find('div.email-settings').show();
      $(self_action).find('div.email-settings input[type=text]').attr("disabled", "disabled");
      $(self_action).find('div.email-settings .form-textarea').attr("disabled", "disabled");
    }
    else if (self.value == 'default-disabled') {
      $(self_action).find('div.notification-description').show();
      $(self_action).find('div.email-settings').hide();
    }
  });
};


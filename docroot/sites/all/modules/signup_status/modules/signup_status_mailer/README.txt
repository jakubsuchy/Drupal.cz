$Id: README.txt,v 1.1 2009/08/17 23:16:09 dww Exp $

=== SUMMARY ===

The Signup Status Mailer submodule allows you to configure e-mail
messages that are sent to users whenever their signup status changes
for a given node.


=== DETAILS ===

Installing the Token module (http://drupal.org/project/token) is
highly recommended so that these messsages can include small
placeholder strings that are dynamically converted to the appropriate
values for the user signing up, the content they sign up for, their
signup data, and your current site configuration.

There are site-wide default templates that any user with the
'administer signup status mailer settings' permission can configure at
the "Mailer defaults" tab on the Signup status settings administration
page (admin/settings/signup_status/mailer).  There are two sets of
settings available for every signup status you have configured on your
site: one for when the status is first added on a signup (e.g. during
a new signup if users can select their own status) and another for
whenever the status is changed.  For each status and action, you can
configure if an email should be sent, and if so, what the subject and
body of the message should be.

Users with the 'administer signup status mailer settings' permission
or the 'administer signup status mailer settings for own content' when
viewing a signup-enabled node that they own will be able to configure
per-node customizations to the site-wide settings.  These settings are
found at the "Signup status e-mail" subtab under the "Signups" tab on
the node (e.g. "node/N/signups/status-email-settings).  Again, for
each status and action (new vs. update), there are a set of options to
control the notification messages.  You can select to disable the
notifications, use the currently active site-wide defaults, or to
force a message to be sent and override the subject and body text.


=== UPGRADING FROM DRUPAL 5 ===

Users upgrading from Drupal 5 will need to completely re-customize all
the message templates.  The Drupal 5 version only supported a single
site-wide template, which is no longer used.  The variables that hold
the Drupal 5 site-wide template will be removed when you run
update.php after upgrading to the Drupal 6 version, so if you'd like
to re-use any of that text, be sure to save a copy before upgrading
your site.


=== SUPPORT ===

Bug reports, feature requests and support questions should use the
"Signup status mailer" component in the Signup status issue queue on
drupal.org:

http://drupal.org/project/issues/signup_status


=== CREDITS ===

The Drupal 5 version of this module was written by Jeffery Beeman
(http://drupal.org/user/16734 -- "jrbeeman").

The Drupal 6 version was a complete rewrite by Derek Wright
(http://drupal.org/user/46549 -- "dww").



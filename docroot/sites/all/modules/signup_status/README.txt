/* $Id: README.txt,v 1.4 2009/08/23 17:11:36 dww Exp $ */

The Signup Status module extends the Signup module's functionality by
adding a feature-rich field to all signups.  This can be used so that
potential attendees indicate if they can come or not ('yes', 'no',
'maybe').  A site administrator can add any values they want, and
control which values (if any) are visible to end-users on the signup
form.  Users with permission to administer signups on signup-enabled
nodes can also set the status of a user's signup. This can be used,
for example, to mark users as "Paid" or "Completed."

This module features:
* Administrative interface for defining site-specific status values.
* Modifications to the Signup broadcast form that allows users who can
  send broadcast messages to restrict them based on signup status.
* Views module support through several fields and filters.
* Views Bulk Operations (VBO) support to alter signup status values in
  bulk on the "Administer" subtab of the "Signups" tab on a node.

Several optional modules are included in this package:

* Signup Status Certificates: Grant certificates to users based on their 
* Signup Status Invite: Allows users with sufficient permissions to
  invite other users to a specific event, in which case they are
  automatically signed up to a given status and an e-mail is sent.
* Signup Status Log: An administrative log of signup status changes.
* Signup Status Mailer: Email users when their signup status has changed.
* Signup Status Notification: Automatically subscribe users to
  notifications about nodes when they are in a certain status.

Be sure to look for a README.txt file in each directory that would
contain additional instructions and information.


------------------------
Requirements
------------------------

* Signup module (6.x-1.0-RC5 or greater): http://drupal.org/project/signup


------------------------
Recommendations
------------------------

* Views: http://drupal.org/project/views
* Views Bulk Operations (VBO): http://drupal.org/project/views_bulk_operations


------------------------
Setup
------------------------

* Enable the module and setup new permissions, if necessary:
  - manage signup status codes: Add, edit, delete signup status codes

* Navigate to Administer > Settings > Signup Status
  (admin/settings/signup_status) to add or remove custom status codes.

For an administrator to modify the status of an existing signup, there
are two options:

-- Edit each signup individually (e.g. using the 'Edit signup' link on
   the node/N/signups/admin tab on a signup-enabled node).

-- Update signup status in bulk using Views Bulk Operations (VBO).


To enable and configure VBO, you must:

1) Download, install and enable Views and VBO.

2) Go to /admin/settings/signup and in the "Advanced settings" for
   "How to display the administrative list of signed-up users" choose
   "embed a view".  In the "View to embed for the signup administrative list"
   drop down, select "signup_status_user_vbo_admin ..." to use the default
   Views Bulk Operations-enabled view for the administrative list of signed
   up users provided by Signup Status.  There are several similarly named
   views, so be SURE you are using "signup_status_user_vbo_admin".

3) Visit /node/N/signups/admin (where N is the node ID of a signup-enabled
   node).  Now, where the "Signup details" operations area used to be,
   you'll have a "Bulk operations" area that looks similar.  This is the
   Views Bulk Operations view, and it allows you to perform operations
   (from the drop down) on selected rows of this table.  You can also edit
   the View style options so that there are other operations in the list
   including canceling a Signup.

4) To alter the signup status on any signup records, click the
   checkbox next to the desired signups on this page and choose the
   "Alter signup status" operation.  Click execute.  This will bring
   you to a page where you can select the new status to give these
   signups and confirm the operation.


------------------------
Support
------------------------

Please submit any feature requests or bug reports to this project's issue
queue at http://drupal.org/project/issues/signup_status


------------------------
Credits
------------------------

The Drupal 5 version of this module was written by Jeffery Beeman
(http://drupal.org/user/16734 -- "jrbeeman").

The Drupal 6 version was fairly major rewrite and refactoring by:
- Miglius Alaburda (http://drupal.org/user/18741 -- "miglius")
- Derek Wright (http://drupal.org/user/46549 -- "dww")


mail.module
README.txt

Description
------------

This module is for sending emails to registered users.  It includes a 
custom "mail" content type, but can also be used to add emailing to any
existing content type.  See INSTALL file for installation.

Usage
------------

After installing the module, enable "Send emails to users" permission
for roles you wish to be able to send emails.  Then to send an email,
select create content > mail.

To enable emailing for mail or other content types, go to the 
configuration page for the particular content type, see

administer > content > configure > content types

or, in url format,

admin/node/configure/settings

and select "enabled" for the Email option.

Then to send an email, select create content > mail 
(or another content type with emailing enabled).

Accepting Emails
------------

This module adds a new field to the user registration and editing 
forms through which users can opt in to receiving site emails.  Emails
send from non-admin accounts will go out only to users who have opted
to receive emails.  However, mails sent from user accounts with the
"administer users" permission will go out to all users, whether or not
they have opted in.

Settings
------------

Use the mail module settings page to set default sending options.
These include format (plain text or HTML), priority, character set,
and receipt request.  These options can be overridden for a particular
mail post on the node editing page.

Attachments
------------

Mail module is integrated with the core upload module.  To enable
sending of uploads, enable the upload module and ensure that
uploads are enabled for mail posts.  Any files uploaded with a 
mail post will then be sent as attachments.

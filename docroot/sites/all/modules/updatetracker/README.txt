nodeauthor README

(C) Copyright 2006 Pavel Prochazka & Jakub Suchy <redakce@drupal.cz>

Description
------------

Nodeauthor module is very simple module which allows users to edit additional
information about them in user-edit form. This information is then displayed
below nodes created by such users.

Administrators are allowed to restrict displaying info to particular node types.

Installation
------------

To install, you move this directory and all its subdirectories
to your /modules directory. Enable the nodeauthor.module, then go to
http://www.your-drupal-site.com/admin/settings/nodeauthor and check node types
which will display author information.

Then go to http://www.your-drupal-site.com/user/YOUR-ID/edit and edit form
field named Additional user info -> Short info.

Then go to http://www.your-drupal-site.com/admin/access and edit permissions for nodeauthor module:
- edit own info - If checked for a role, user will be allowed to edit own author information. You should allow it for anonymous users if you want to allow editing authoring information at register form
- view author info - If checked for a role, user will be allowed to view authoring information for nodes. You would probably want to permit it for ALL roles (including anonymous)

Everything done, enjoy.

Theming
-------

Info is by default displayed as:
<div class="nodeauthor-info">
 <span>About author</span>
 ...your information...
</div>

You can theme the display by editing nodeauthor.css in this directory

Bugs
----
Please feel free to report any bugs you find to Drupal.org issue tracker.

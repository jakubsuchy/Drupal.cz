cs_social_networks README

(C) Copyright 2006 Jakub Suchy <redakce@drupal.cz>
Support: http://www.drupal.cz

Description
------------

Czech & Slovak social networks is module which displays links to add current displayed node to czech and slovak social networks such as: Jagg.cz, Linkuj.cz, Asdf.sk, CoJeNoveho.cz
Administrators are allowed to restrict displaying links to particular node types and selected networks.

Installation
------------

To install, move this directory and all its subdirectories
to your /modules directory. Enable the cs_social_networks module, then go to
http://www.your-drupal-site.com/admin/settings/cs_social_networks and check enabled node types and enabled networks.
Warning: no networks and no node types are enabled after installation!

Then go to http://www.your-drupal-site.com/admin/access and edit permissions for cs_social_networks module:
- view links - If checked for a role, user will be allowed to view links in node. You would probably want to permit it for ALL roles (including anonymous)

Everything done, enjoy.

Theming
-------
Every links has cs_social_networks class and $NETWORKNAME-link class

Bugs
----
Please feel free to report any bugs you find to Drupal.org issue tracker.
http://www.drupal.org/project/cs_social_networks

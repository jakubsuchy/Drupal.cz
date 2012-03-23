Overview
--------
The banner.module allows you to display ads on your Drupal website. It
randomly displays the banners, and automatically tracks how many times each
is displayed and clicked.

Users of your website can be given ownership of banners, and be allowed to
modify certain settings and view statistics.


Requirements
------------
Drupal 4.7.x
PHP 4.3.3 or greater


Features
--------
 - Administrative features:
   o Supports many image types, flash animations, and text-based ads
   o Supports displaying multiple banners on one page
   o Supports scheduling when a banner is auto-enabled/disabled
   o Provides 'chance' mechanism to increase odds a given banner is displayed
   o Banners can be categorized with taxonomy
   o Counts banner views/clicks
   o Provides overview page w/ statistics
   o Limit banner views and/or clicks (then auto-disable)
   o Permissions (not view banners, administer, user edit)
   o Provides filecaching for optimal performance
   o Utilizes javascript to rotate banners on cached pages
   o Multiple banner status, only 'enabled' are displayed.
     (Others: disabled, blocked, pending, day limit reached, etc...)
   o Must approve user-uploaded banners
   o Can customize daily/weekly banner notification emails
   o Can send automatic renewal reminder for ads soon to expire

 - User features:
   o Can view/edit own banners if have 'manage banners' permission
   o Can view/edit all banners if have 'administer banners' permission
   o Can upload new banners in 'pending' state
   o Can manually enable/disable administratively approved banners
   o Provides daily banner statistics (views, clicks, %)
   o Can limit maximum daily views for each banner
   o Can enable daily notification email
   o Provides weekly banner statistics (views, clicks, %)
   o Can limit maximum weekly views for each banner
   o Can enable weekly notification email


Installation
------------
Please refer to the INSTALL.txt file for installation directions.



Banner publishing status
------------------------

Banner workflow     Published
-----------------------------
BANNER_PENDING      No
BANNER_ENABLED      Yes
BANNER_DAY_LIMIT    Yes
BANNER_WEEK_LIMIT   Yes
BANNER_DISABLED     Yes
BANNER_BLOCKED      Yes
BANNER_DENIED       No


Todo
----

Docs: PHP has trouble determining dimensions of .swf files if the Flash version
used is later than MX. (Thanks to Liktor Tibor for looking into this.) This
means that width and height fields should be editable by the banner owner.


Credits
-------
 - Original author: Marco Molonari
 - Previous maintainer: Jeremy Andrews
 - Current maintainer: Morten Wulff


Bugs and Suggestions
--------------------
Bug reports, support requests, feature requests, etc, should be posted to
banner module project page:

http://drupal.org/project/banner


$Id: README.txt,v 1.9.2.1 2006/09/23 14:39:43 wulff Exp $
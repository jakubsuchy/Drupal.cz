// $Id: README.txt,v 1.1.2.3 2008/07/03 00:56:22 mfb Exp $

CONTENTS OF THIS FILE
---------------------

 * Overview
 * Quick setup
 * Requirements
 * Blocks
 * SiteSearch
 * Search module integration
 * Advanced settings
 * Installation
 * Maintainers

OVERVIEW
--------

Google Custom Search Engine (CSE) is an embedded search engine that can 
be used to search any set of one or more sites.  No Google API key is 
required.  More info at http://www.google.com/coop/cse/

QUICK SETUP
-----------

After installing this module, configure it by entering Google's 
alphanumeric ID for your CSE.  Once you have granted permission for one 
or more roles to search the Google CSE, the search page can be found at 
search/google, and a search block can also be enabled.

REQUIREMENTS
------------

Due to Google and Drupal both making use of "q", this module requires 
that clean URLs be enabled.

BLOCKS
------

The two included blocks can be enabled at admin/build/block.  The 
"Google CSE" block provides a typical search box and redirects to the 
search/google path.  An additional "Google CSE results" block allows any 
page on the site to host a self-contained CSE search form and results 
display.  After entering search terms, the user will be returned to the 
same page (via GET request) and the results will be displayed.

SITESEARCH
----------

In addition to the CSE functionality, SiteSearch on one or more domains 
or URL paths can optionally be configured.  Radio buttons allow users to 
search on either the SiteSearch option(s) or the CSE, and searches can 
default to either option.

SEARCH MODULE INTEGRATION
-------------------------

The "Google CSE search" module is an optional glue module that 
integrates Google CSE with the Drupal core search API.  After enabling 
this module, search queries will be logged by the Search module and 
users can click between available search tabs such as search/node.

ADVANCED SETTINGS
-----------------

The collapsed advanced settings on the settings page provide various 
customizations such as country and language preferences.  For example, 
with the Locale module enabled, the Google CSE user interface language 
can be selected dynamically based on the current user's language.

INSTALLATION
------------

Place the google_cse directory in your sites/all/modules directory.  
Enable the Google CSE module at admin/build/modules, configure it at 
admin/settings/google_cse, assign permissions for "search Google CSE" at 
admin/user/permissions, and enable the "Google CSE" block at 
admin/build/block.

To configure this module, you will need your CSE's alphanumeric ID 
("cx").  Go to your CSE manage page on Google.com, click on control 
panel and then click on code.  The only part you need is the cx value; 
to isolate it you'll have to copy/paste the code into a text editor.

MAINTAINERS
-----------

Authored and maintained by mfb: http://drupal.org/user/12302

The current maintainer does not plan to add new features to this module, 
such as support for multiple CSEs; however, patches providing new 
features are welcome and will be reviewed.

For bugs, feature requests and support requests, please use the issue 
queue at http://drupal.org/project/issues/google_cse

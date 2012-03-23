/* $Id: README.txt,v 1.4.2.1 2008/02/17 18:01:55 havran Exp $ */
Texy! 2 Drupal module

Author: Juraj Chlebec <havran (at) gmail.com>
Copyright 2006-2007 Juraj Chlebec aka Havran
Portions copyright 2007 archetwist <arche (at) letwist.net>

Licensed under the GPL v2.0

About
-----------------------------
The Texy! module allows you to integrate the Texy! text-to-XHTML formatter
and converter library into Drupal.

The library allows you to enter content using an easy to read syntax which is
filtered into structurally valid XHTML. No knowledge of HTML is required.

Texy! is one of the most complex formatting tools. It allows adding of images,
links, nested lists, tables and has a full support for CSS.

Installation
-----------------------------
1) Extract the 'texy/' directory from inside the module package to the
   'sites/all/modules/' directory of your Drupal installation. This package
   includes also the needed Texy! library, and auto-detects the correct
   version, so no additional downloads needed. But however, you might want
   to check the official website http://texy.info/en/download for possible
   updates (replace files inside the 'sites/all/modules/texy/texy'
   subdirectory).

2) Enable the module at admin/build/modules.

3) Create a new data format at admin/settings/filters/add and assign the Texy!
   filter to it. Give your users permission to use the new format.

You're done!

Documentation
------------------------------
A quick reference of the syntax can be found at your Drupal's filter/tips page.
The http://texy.info/en/syntax page provides some more elaborate examples.

Texy! modules
------------------------------
For better modularity is Texy! module now expandable with own modules. You can
look at Image settings module and Syntax highlighting module for overview.

Texy! module now provide two hooks:

  hook_texy_handler(&texy)
  hook_texy_settings(&texy)

With this hooks you can expand Texy! module with your own modifications (look
at directory examples in Texy! distribution).

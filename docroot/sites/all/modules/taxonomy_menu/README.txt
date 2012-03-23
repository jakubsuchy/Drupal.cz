TAXONOMY MENU
=============

INTRO
=====
This module adds links to taxonomy terms to the global navigation menu.


INSTALLATION
============
1) Place this module directory into your Drupal modules directory.

2) Enable the taxonomy_menu module in Drupal, at:
   administration -> site configuration -> modules (admin/build/modules)

3) Choose which vocabularies to appear in the menu at:
   administration -> content management -> taxonomy
   (admin/content/taxonomy)

4) Edit the vocabulary or create a new one.

CONFIGURATION
=============
All configuration options are on the vocabulary's edit screen 
admin/content/taxonomy/edit/vocabulary/$vid)
Options:
 Menu: Select which menu the vocabulary's terms should appear under
 
 Menu Path Type: Select how the url for the term path should be created. 
  Included are Default, hierarcy and custom. 
  This is extendable using hook_taxonomy_menu_path().
  See developers documentation for more information. (http://drupal.org/node/380652)
 
 Syncronise changes to this vocabulary: If selected, the menu will auto update when you
  change a node or term.  Recommened to always have this selected.
 
 Display Number of Nodes: Displays the number of nodes next to the term in the menu.
 
 Hide Empty Terms: Does not create menu links for terms with no nodes attached to them.
 
 Item for Vocabulary: Create a menu link for the vocabulary.  This will be the parent menu item.
 
 Auto Expand Menu Item: Enables the 'Expand' option when creating the menu links.  
  This is useful if using suckerfish menus in the primary links.
 
 Display Descendants:  Alters the URL to display all of child terms.  <base path>/$tid $tid $tid $tid
  When this is set, the Path Alias is not applied.
 
 Select to rebuild the menu on submit: Deletes all of menu items and relationships between the menu and terms
  and recreates them from the vocabulary's terms.  This will create new mlid's for each item, so be careful
  if using other modules to extend the menu functionality.
 
HIERARCHY PATH
==============
This should only be used if you have custom code or a block that relies on the category/vid/tid/tid/tid.
If you would like the url to be this path, the recomendation is to use PathAuto with 'category/[vocab-raw]/[copath-raw]'.
Use the field "Base Path for Hierarchy Path" to see the base URL that will match the veiw or page callback.
The view or pagecall back MUST be created before the taxonomy menu.

CUSTOM PATH
===========


"Base Path for Hierarchy Path:"
VIEWS
=====
VIEWS WITH MENU PATH TYPE: DEFAULT
 The default view is 'taxonomy_term (default)'. 
 The path of the view is 'taxonomy/term/%', the argument is 'Term ID (with depth)' and 'Depth Modifier' - 
 but only TERM ID will be passed as an argument.
 This view can be changed - but it is recommended to use the option MENU PATH TYPE: CUSTOM for individual views.

VIEWS WITH MENU PATH TYPE: CUSTOM
 You need to have a view with path 'custom path/%' and an argument 'Term ID' before you create the taxonomy menu.
 To use the 'Display Depth in Custom Path:' option, the path in the view has to be 'custom path/%/%'" 
 The two arguments must be 'Term ID (with depth)' and 'Depth Modifier'. Have this view setup before you create the taxonomy menu.
   
NOTES
=====
 * Menu Items are Path Alias aware and compatible with PATHAUTO.
 * Taxonomy Menu does not handle the menu call backs.  It only creates the links to the menus.
 * The router item must be created before Taxonomy Menu creates the links.  Failure to so so 
    will cause the menu items to not be created.
 * Router items can be created by either a view or another modules hook_menu.
 * If using Path Auto, the URL passed to the code it taxonomy/term/$tid.
 * Advanced Breadcrumbs can be controled by Taxonomy Breadcrumb (http://drupal.org/project/taxonomy_breadcrumb)
 * Changing the taxonomy default URL to match the custom Taxonomy Menu Path can
    be controled by Taxonomy Redirect (http://drupal.org/project/taxonomy_redirect)
 * When using the CCK 'Content Taxonomy' module the field option Save values additionally to the core taxonomy system 
    (into the 'term_node' table)' has to be enabled. 
	Otherwise nodes with taxonomy terms linked through this field will not be shown within the 
	menu structure that Taxonomy Menu creates.

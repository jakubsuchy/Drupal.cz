$Id: README.txt,v 1.6 2008/12/31 12:37:41 gpdinoz Exp $

CONTACT_FORMS.MODULE

Overview
--------
This module expands the features of the site wide
contact form. It eliminates the need for the drop
down category menu by generating a form, and a
unique path, for each of the contact form categories.

The path 'contact/{category}' generates a contact form
for that category with a title = 'Contact {category}'.
e.g. path 'contact/Jill Jones' generates a contact form
for the category 'Jil Jones' with a title = 'Contact jill Jones'.

If a path is entered that does not have a category you
can specify a page to redirect to. The default fall back
path is contact.

To define the subject of the generated form the path is as follows
'contact/{category}/Subject goes here'
e.g. path 'contact/Jill Jones/Enquiry about Sales' generates a contact form
for the category 'Jil Jones' with a title = 'Contact jill Jones' and the subject
'Enquiry about Sales'.



Requirements
------------
contact.module must be enabled.


Installation
------------
  1. Copy contact_forms folder to sites/all/modules/.
  2. Check contact.module is enabled
  3. Enable Contact Forms module


Upgrading
---------
If you are upgrading from an older version of Contact Lists
Disable and uninstall the old version then delete the contact_list folder
before you upload and enable the new version.


Usage
-----
This module was written for sites that have many email
contacts where you want a page with information about
the people / departments etc with links to their individual
contact forms.

Create a "Contact Directory" page with a path like 'directory'
and lay it out how you would like it. Links to the forms can
be made with the following code [using the drupal link function]
<?php
print l('Email Jill Jones', 'contact/Jill Jones');
?>
This page can be set as the fall back page if a contact/category
path is entered that doesn't exist.


Snippets
--------
1. The "old" contact/list page
This will give you a list of links to the contact forms.

<!-- start snippet -->
<p>Who would you like to contact?</p>
<?php
  $result = db_query('SELECT * FROM {contact} ORDER BY weight, category');
  print '<div id="contact-list">';
  print '<div class="item-list"><ul>';
  while ($contact = db_fetch_object($result)) {
    $cleanurl = str_replace(' ', '_' ,$contact->category);
    print '<li>'. l($contact->category , 'contact/'. $cleanurl) .'</li>';
  }
  print '</div>';
  print '</div>';
?>
<!-- end snippet -->


Credits
-------
Thanks to the following people who have helped with suggestions and code:
nancyw
alienbrain
incrn8
joachim
mfredrickson
jandd
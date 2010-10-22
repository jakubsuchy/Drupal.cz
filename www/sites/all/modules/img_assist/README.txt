********************************************************************
                     D R U P A L    M O D U L E
********************************************************************
Name: img_assist.module
Author: Benjamin Shell <http://www.benjaminshell.com>
Original Author: Matt Westgate <drupal at asitis dot org>

DESCRIPTION
********************************************************************
An interface for adding images to any textarea that's aware of input
formats.

INSTALLATION
********************************************************************
See the INSTALL.txt file in this directory.

HOW TO USE THIS MODULES ONCE INSTALLED
********************************************************************
Read this after installing the module.

Using this module with TinyMCE:
  1. Click the camera icon on the TinyMCE toolbar
  2. Upload a new photo or choose an existing image.
  3. Set the properties for how you want the image to display.
  4. Click the Insert button.
  
Using this module with any textarea:
  1. Click the "add image" link under any textarea box. 
  2. Upload a new photo or choose an existing image.
  3. Set the properties for how you want the image to display.
  4. Click the Insert button.

Most browsers such as Internet Explorer and Mozilla clones will
insert the image exactly where you place your cursor in the textarea of
your content form.  Otherwise the image will be appended to the end of
your entry.

Adding images with image module
===============================
Users with the 'access img_assist' permission will see the 'add image'
link/icon (if it is enabled in the settings).  Access to img_assist
via the TinyMCE plugin is controlled by the TinyMCE module.  

Users with the 'create images' permission will be able to upload images using
img_assist.  All users will be able to see and insert their own pictures, 
even if the image nodes are unpublished.  Users with the 'access all images' 
permission will also be able to use other images, but only if they are 
published.

One possible workflow would be to set images to be UNPUBLISHED by default.
That way users can upload, categorize, and use images in img_assist without
the images showing up anyway else on the site.  Images that should 
also be shown elsewhere on the site can manually be published by going to
administer > content.

IMAGE PRESENTATION
********************************************************************
A full img_assist tag looks like this:

[img_assist|nid=2|title=My Image Title|desc=Description of the image|align=right|width=200|height=150|link=url,http://www.google.com]

Img_assist tags generated with this version of img_assist are 
NOT compatible with older versions of img_assist because the
tags use the image node ID (nid) instead of the file ID (fid).

However, this module is backwords compatible with older img_assist filter tags.

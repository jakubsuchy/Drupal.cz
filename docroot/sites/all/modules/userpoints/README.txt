$Id: README.txt,v 1.2.4.2 2006/08/28 17:04:44 kbahey Exp $

Copyright 2005 http://2bits.com

Description
-----------
This module provides the ability for users to gain points when they
do certain actions, such as:

- posting a node (different points can be awarded for different
  node types, e.g. page, story, forum, image, ...etc.)
- posting a comment
- moderating a comment
- voting on a node (requires the nodevote module)
- inviting a person to register on the site
- invited person actually registers on the site
- purchasing from your e-commerce store (reward points)

Using real money, users can purchase points from your ecommerce store
as well.

Moreover, the points can be used as currency for ecommerce as well,
as in a form of payment

Upon deleting a node or a comment the number of points is subtracted.

The number of points for each of the above actions is configurable by
the site adminsitrator.

A block displays the number of points the user gained. Another block 
displays the top 5 users who earned points.

This module is useful in providing an incentive for users to participate
in the site, and be more active.

Initally sponsored by: http://artalyst.com

Extended Version
----------------
A commercial extended version of this module allows users to join new
roles with more permissions as they gain points.

Contact the author for details.

Installation
------------
To install this module, do the following:

1. Extract the tar ball that you downloaded from Drupal.org.

2. Upload the userpoints directory and all its contents to your
   modules directory.

Configuration
-------------
To enable this module do the following:

1. Go to Admin -> Modules, and enable userpoints.
   Check the messages to make sure that you did not get any errors
   on database creation.

2. Go to Admin -> Settings -> userpoints.

   Configure the options as per your requirements

3. Go to Admin -> Access Control and enable viewing for the roles you want.

For configuring with e-commerce, you have to have the ecommerce modules
installed and configured.

- User points can be used as a form of payment, with an admin defined
  multiplier

- Users gain points when purchasing items via e-commerce for every dollar
  they spend.

This is useful as a reward system.

This also allows purchasing of points for real money. You have to setup
a non-shippable product, and adjust the multiplier accordingly.

API
---
This modules provides a callable interface for adding or subtracting points
to a user account, as well as querying a user's account for how much points
he currently has:

The functions are:

 hook_userpoints($points, $uid) 

 userpoints_get_current_points($uid) 

Bugs/Features/Patches:
----------------------
If you want to report bugs, feature requests, or submit a patch, please do so
at the project page on the Drupal web site.
http://drupal.org/project/userpoints

Author
------
Khalid Baheyeldin (http://baheyeldin.com/khalid and http://2bits.com)

If you use this module, find it useful, and want to send the author
a thank you note, then use the Feedback/Contact page at the URL above.

The author can also be contacted for paid customizations of this
and other modules.

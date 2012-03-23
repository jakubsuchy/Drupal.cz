$Id: README.txt,v 1.2 2008/11/27 16:46:00 kbahey Exp $

Copyright 2007 http://2bits.com

Description
-----------
Expires rows from the session table older than a certain time.

On busy sites, the sessions table can grow to be very large, and that can
cause slow accesses to it, as well as slow writes due to locking, leading
to performance bottlenecks.

By trimming the table regularly, the above bottlenecks are avoided.

Drupal uses the PHP garbage collection mechanism to cleanup the sessions
table, but this mechanism depends on PHP's configuration, and can fire for
any session.

This module moves this functionality to cron, and hence is a background
process, and is consistent and predictable regardless of PHP's garbage
collection configuration.

Installation
------------
To install, copy the module's directory and all its contents to your modules
directory, normall sites/all/modules.

Configuration
-------------
To enable this module, visit Administer -> Site building -> Modules.

To configure it, go to Administer -> Site configuration -> Session Expire.

The default settings are suitable for most sites, but you can adjust them
to your particular needs. The instructions there should be self explanatory.

Bugs/Features/Patches:
----------------------
If you want to report bugs, feature requests, or submit a patch, please do so
at the project page on the Drupal web site.
http://drupal.org/project/session_expire

Author
------
Khalid Baheyeldin (http://baheyeldin.com/khalid and http://2bits.com)

If you use this module, find it useful, and want to send the author
a thank you note, then use the Feedback/Contact page at the URL above.

The author can also be contacted for paid customizations of this
and other modules.

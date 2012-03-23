// $Id: README.txt,v 1.2.2.1.2.5.2.7 2009/09/08 07:22:48 mikeytown2 Exp $

DESCRIPTION
-----------
This module provides static page caching for Drupal 6.x, enabling a
potentially very significant performance and scalability boost for
heavily-trafficked Drupal sites.

For an introduction, read the original blog post at:
  http://bendiken.net/2006/05/28/static-page-caching-for-drupal


FEATURES
--------
* Maximally fast page serving for the anonymous visitors to your Drupal
  site, reducing web server load and boosting your site's scalability.
* On-demand page caching (static file created after first page request).
* Full support for multi-site Drupal installations.


REQUIREMENTS
------------
This version of Boost is designed for Drupal 6.x running with Apache on any platform.
Drupal's clean URLs MUST be enabled and working properly.

The following modules are recommended.
  path
  pathauto
  globalredirect
  transliteration

In order for the static files to be correctly expired, the Drupal cron job
must be correctly setup to execute more often than, or as often as, the
cache lifetime interval you specify.

Since the static page caching is implemented with mod_rewrite directives,
Apache version 1.3 or 2.x with mod_rewrite enabled is required (if Drupal's
clean URLs work for you, you're fine; if not, get them working first).
Other web servers, such as Lighttpd, are NOT officially supported at present.
Lighttpd: http://drupal.org/node/150909
Nginx: http://drupal.org/node/244072

INSTALLATION
------------
  1. Go to [Administer >> Site configuration >> Clean URLs] and ensure that
     Drupal's clean URLs are enabled and working correctly on your site.

  2. Copy all the module files into a subdirectory called
     sites/all/modules/boost/ under your Drupal installation directory.

  3. Go to [Administer >> Site building >> Modules] and enable the Boost
     module. You will find it in the section labeled "Caching".

  4. Go to [Administer >> Site configuration >> Performance >> Boost]
     Specify the cache directory, which should be something like
     cache/www.example.com (keeping the default directory is highly recommended)
     and must be writable by the web server: you may need to create the
     directory, and set the permissions so it is writable. If you choose to use
     gzip, for compression of files cached by Boost, you also specify and may
     need to create the folder for this; the default is of the format
     cache/gz/www.example.com

     On the [Administer >> Site configuration >> Performance] Page is the
     Default minimum cache lifetime setting. As cached pages are created, they
     are given an expire by date and time, which is the current date and time
     plus the minimum cache lifetime. These dates and times are checked on each
     cron run; and if a page is expired, the cache is cleared, and a new cached
     version will be created the next time the page is created by an anonymous
     user (including bots).

  5. IMPORTANT: after backing up the original .htaccess file in your Drupal
     installation directory, add the rules found in boosted1.txt
     right below
       # RewriteBase /
     and above
       # Rewrite URLs of the form 'x' to the form 'index.php?q=x'.
     boosted1.txt is found in the sites/all/modules/boost/htaccess/ directory.
     The default.txt file shows you the exact placement of where the rules go,
     in case your not sure. If you fail to do this step, static page caching
     will NOT work! Rules from boosted1.txt can also be found on the Boost
     Settings page (admin/settings/performance/boost) in the expandable field
     group called "Boost Apache .htaccess settings generation.". If Drupal is
     installed in a subdirectory, you have to get the rules from the Boost
     Settings page.

     Should you have any trouble getting static pages served using the
     boosted1.txt configuration, try the boosted2.txt file before posting a
     support issue on drupal.org; or try the different radio buttons on the
     admin/settings/performance/boost page under "Boost Apache .htaccess
     settings generation." field group.

     (For the technically inclined, the difference between the two supplied
     .htaccess templates is due to boosted1.txt relying on SERVER_NAME
     versus boosted2.txt using HTTP_HOST. There exist valid use cases for
     both, especially in more advanced, multi-site Drupal installations.)

  6. Log out from Drupal (or use another browser) to browse around your site
     as the anonymous user. Ensure that static files are indeed being
     generated into the Boost cache directory you specified above (#4); and if
     you opt to use gzip, likewise check gzipped files are being generated in
     the directory you specified for this. The performance settings page shows
     how many pages are being cached by Boost, and (for pages Boost cannot
     cache) by Drupal core.

  7. Set $base_url variable in /sites/default/settings.php (line 125 or so)
     so cron runs error free and clears the cache properly when invoked like
     'php /path/to/cron.php' or 'drush cron'. This should be something like
     http://www.example.com

     Guide for editing settings.php
     http://drupal.org/node/367081#comment-1504894

  8. Check the status page [Administer >> Reports >> Status report] for any
     errors or notices.


HOW IT WORKS
------------
Once Boost has been installed and enabled, page requests by anonymous
visitors will be cached as static HTML pages on the server's file system.
Periodically (when the Drupal cron job runs) stale pages (i.e. files
exceeding the maximum cache lifetime setting) will be purged, allowing them
to be recreated the first time that the next anonymous visitor requests that
page again.

New rewrite rules are added to the .htaccess file supplied with Drupal,
directing the web server to try and fulfill page requests by anonymous
visitors first and foremost from the static page cache, and to only pass the
request through to Drupal if the requested page is not cacheable, hasn't yet
been cached, or the cached copy is stale.


FILE SYSTEM CACHE
-----------------
The cached files are stored (by default) in the cache/ directory under your
Drupal installation directory. The Drupal pages' URL paths are translated
into file system names in the following manner:

  http://mysite.com/
  => cache/mysite.com/index.html

  http://mysite.com/about
  => cache/mysite.com/about.html

  http://mysite.com/about/staff
  => cache/mysite.com/about/staff.html

  http://mysite.com/node/42
  => cache/mysite.com/node/42.html

You'll note that the directory path includes the Drupal site name, enabling
support for multi-site Drupal installations.


DISPATCH MECHANISM
------------------
For each incoming page request, the new Apache mod_rewrite directives in
.htaccess will check if a cached version of the requested page should be
served as per the following simple rules:

  1. First, we check that the HTTP request method being used is GET.
     POST requests are not cacheable, and are passed through to Drupal.

  2. Since only anonymous visitors can benefit from the static page cache at
     present, we check that the page request doesn't include a cookie that
     is set when a user logs in to the Drupal site. If the cookie is
     present, we simply let Drupal handle the page request dynamically.

  3. Now, for the important bit: we check whether we actually have a cached
     HTML file for the request URL path available in the file system cache.
     If we do, we direct the web server to serve that file directly and to
     terminate the request immediately after; in this case, Drupal (and
     indeed PHP) is never invoked, meaning the page request will be served
     by the web server itself at full speed.

  4. If, however, we couldn't locate a cached version of the page, we just
     pass the request on to Drupal, which will serve it dynamically in the
     normal manner.


IMPORTANT NOTES
---------------
* To check whether you got a static or dynamic version of a page, look at
  the very end of the page's HTML source. You have the static version if the
  last line looks like this:
    <!-- Page cached by Boost @ 2009-08-07 12:34:56 -->
* If your Drupal URL paths contain non-ASCII characters, you may have to
  tweak your locate settings on the server in order to ensure the URL paths
  get correctly translated into directory paths on the file system. You can also
  turn off the ASCII filter under Cache -> Advanced on the performance >> boost page.
* Drupal's core stats is supported. Configure the "Popular content" block, but
  then disable it. Place the "Boost: AJAX core statistics" in its place. If ajax
  stats are loading too slowly, copy stats/boost_stats.php to your webroot and
  enable "Cache Statistics Block". The cache gets updated on cron runs.


LIMITATIONS
-----------
* Only anonymous visitors will be served cached versions of pages;
  authenticated users will get dynamic content. This will limit the
  usefulness of this module for those community sites that require user
  registration and login for active participation.
* In contrast to Drupal's built-in caching, static caching will lose any
  additional HTTP headers set for an HTML page by a module. This is unlikely
  to be problem except for some very specific modules and rare use cases.
* Web server software other than Apache is not supported at the moment.
  Adding Lighttpd support would be desirable but is not a high priority for
  the author at present (see TODO.txt). (Note that while the LiteSpeed web
  server has not been specifically tested by the author, it may, in fact,
  work, since they claim to support .htaccess files and to have mod_rewrite
  compatibility. Feedback on this would be appreciated.)
* At the moment, Windows is untested. It *should* work, but it's untested ATM.


BUG REPORTS
-----------
Post feature requests and bug reports to the issue tracking system at:

  <http://drupal.org/node/add/project-issue/boost>


CREDITS
-------
Developed and maintained by Arto Bendiken <http://bendiken.net/>
Drupal 6.x maintained by Mike Carper <http://316solutions.net>
Ported to Drupal 6.x by Ben Lavender <http://bhuga.net/>
Ported to Drupal 5.x by Alexander I. Grafov <http://drupal.ru/>
Miscellaneous contributions by: Jacob Peddicord, Justin Miller, Barry
Jaspan.

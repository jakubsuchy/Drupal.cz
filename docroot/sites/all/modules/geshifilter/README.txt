****************************
GeSHi Filter (Drupal Module)
****************************


DESCRIPTION
-----------
The GeShi Filter is a Drupal module for syntax highlighting of pieces of
source code. It implements a filter that formats and highlights the syntax of
source code between for example <code>...</code>.


DEPENDENCY
----------
This module requires the third-party library: GeShi (Generic Syntax
Highlighter, written by Nigel McNie) which can be found at
  http://qbnz.com/highlighter
See installation procedure below for more information.


INSTALLATION
------------
1. Extract the GeSHi Filter module tarball and place the entire geshifilter
  directory into your Drupal setup (e.g. in sites/all/modules).

2. Download the GeSHi library (version 1.0.x) from  http://qbnz.com/highlighter
  and place the entire extracted 'geshi' folder (which contains geshi.php)
  in the geshifilter directory (e.g. as /sites/all/modules/geshifilter/geshi)

3. Enable this module as any other Drupal module by navigating to
  administer > site building > modules


CONFIGURATION
-------------
1. The general GeSHi Filter settings can be found by navigating to:
  administer > site configuration > geshifilter
2. Further configuration instructions can be found by following the
  "more help..." link at the top of that general settings page, which leads
  to www.example.com/?q=admin/help/geshifilter .


USAGE
-----
The basic usage (with the default settings) is:
  <code language="java">
  for (int i; i<10; ++i) {
    dothisdothat(i);
  }
  </code>
When language tags are enabled (like "<java>" for Java) you can also do
  <java>
  for (int i; i<10; ++i) {
    dothisdothat(i);
  }
  </java>
More options and tricks can be found in the filter tips of the input format at
www.example.com/?q=filter/tips .


AUTHORS
-------
Original module by:
  Vincent Filby <vfilby at gmail dot com>

Drupal.org hosted version for Drupal 4.7:
  Vincent Filby <vfilby at gmail dot com>
  Michael Hutchinson (http://compsoc.dur.ac.uk/~mjh/contact)
  Damien Pitard <dpdev00 at gmail dot com>

Port to Drupal 5:
  rötzi (http://drupal.org/user/73064)
  Stefaan Lippens (http://drupal.org/user/41478)

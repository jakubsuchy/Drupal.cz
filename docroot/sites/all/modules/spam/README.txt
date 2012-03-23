--------
Upgrades
--------
At this point, the upgrade from 1.0 or 5.x to 1.1 and 1.2 is not well
supported. It seems to be mainly due to the handling of the spam_filters
table, but there could be other things in the way.

See: http://drupal.org/node/1137648
     http://drupal.org/node/1185292

---------
Overview:
---------
The Spam module provides numerous tools to auto-detect and deal with spam
content that is posted to your site, without having to rely on third-party
services.

The Spam module provides a trainable Bayesian filter, detection of content
posted from open email relays, flagging of content with an excessive amount of 
links, the ability to create custom filters, and more.

Features:
   * Can be used completely independently of any third-party service.
   * Automatically learns and blocks spammer URLs and IPs.
   * Detects repeated postings of the same identical content, or content
     containing too many links.
   * Can notify the user and/or administrator that content was determined to be
     spam, preventing confusion over why their content doesn't show up.
   * Allows filtered users to provide feedback when their postings are
     incorrectly flagged as spam.
   * Provides comprehensive logging to offer an understanding as to how and why
     content is determined to be or not to be spam.
   * Language-independent: Automatically learns to detect spam in any language
     using Bayesian logic.
   * Supports the creation of custom filters using powerful regular expressions.
   * Written in PHP specifically for Drupal.
   * Highly configurable and extendable (includes hooks for writing custom
     filters).

-------------
Spam filters:
-------------
The spam api module includes several spam filter modules, all of which work
together to try and determine if a given piece of content is spam.  Each module
will review the content and return a score between 1 and 99, where 1 means there
is a 1% chance that the scanned content is spam and 99 means there is a 99%
chance that the scanned content is spam.  The spam api module takes a weighted
average of all of these scores and assigns a final overall score for the
content.  Based on this final score, the content may or may not be allowed to
be posted on your website.

To see a list of all enabled spam filter modules, log in as a website
administrator and visit "Administer >> Site configuration >> Spam >> Filters".
On this page, filters are listed according to their weight, with lighter weights
floating to the top.  The filters are run in the order they are listed, but at
this time all filters are always run so order is not important.  It is possible
to disable individual modules on this page.  Finally, you can also set a "gain"
for each module.  


  Gain:
  -----
  The gain can be set to any value from 0 to 250.  The gain is a %, so a gain
  of 100 is a 100% gain, and a gain of 250 is a 250% gain.  Each spam filter
  module is assigned a gain.  The spam api module uses this gain to weight
  the spam score returned by that spam filter module.  Thus, if a module is
  given a gain of 0%, this effectively disables the module as any score it
  returns is ignored. (It is much more efficient to actually disable the module,
  as there is overhead from running the filters even if the final score is
  ignored.)

  The more confident you are of a given spam filter's score, the higher the
  gain should be.  The less confident you are of a given spam filter's score,
  the lower the gain should be.  The score returned by a filter with a gain of
  250 has two and a half times the effect of a score returned by a filter with
  a gain of 100.

  When first training your Bayesian filter, it will be inherently be wrong much
  of the time.  Thus, when you first enable the Bayesian filter you should
  set the module's gain to a low value.  After it has been sufficiently trained,
  can then increase the gain to a higher value.


  Duplicate filter:
  -----------------
  The duplicate filter calculates a hexidecimal "hash" for content as it is
  posted to your website.  If the same exact content is posted again, it will
  generate the same "hash" and be detected as duplicate content.  This module
  can then prevent this duplicate content from being posted, and can
  automatically unpublish the previous duplicate posts.

  The duplicate filter also tracks how many times the same IP address has been
  used to post spam.  If the same IP address posts spam more than a configurable
  number of times, the IP address can be automatically banned from posting any
  further content to your website.
  
  This spam filter module can be configured by visiting "Administer >> Site
  configuration >> Spam >> Filters >> Duplicate".  By default, if the same
  identical content is posted twice it is flagged as spam and unpublished.  If
  the same IP address is found to have posted more than three pieces of spam
  content the IP is blacklisted and prevented from posting any further content.

  IP addresses are blacklisted only as long as the spam exists on your website.
  Once the spam is deleted, the IP is no longer blacklisted.


  SURBL filter:
  -------------
  SURBLs are lists of web sites that have appeared in unsolicited messages.
  Unlike most blacklists, SURBLs are _not_ lists of message senders.

  The SURBL filter is integrated with several online SURBL lists, checking if
  any of the URLs found in new content exists in these lists.  If no URLs
  match, the filter does not return any score and the filter is ignored.  If
  one or more URLs match, the filter flags the content as highly probably spam.

  There is currently no configuration possible for the SURBL module.


  URL filter:
  -----------
  The URL filter scans all new content for URLs.  It then remembers if this
  URL was found in spam content or non-spam content.  If the URL is more often
  found in spam content than non-spam content, then the new content is flagged
  as being highly probably spam.

  There is currently no configuration possible for the URL filter.


  Custom filter:
  --------------
  The custom filter allows you to manually define one or more text strings or
  regular expressions to try and match against new site content.  If no custom
  filter matches, then the module will not return a score and the filter will
  be ignored.

  All existing filters will be listed on this page.  One or more filters can
  be quickly disabled or deleted through this interface.  Statistics are
  provided as to how frequently each filter is matching content, and when the
  last match occurred.  To re-enable or otherwise reconfigure a specific filter
  click the "edit" link.

  To create custom filters, visit "Administer >> Site configuration >> Spam >>
  Filters >> Custom".  To create a new filter, click the 'create custom filter'
  link at the bottom of that page.

  New filters can be a simple text string, or a more complex regular expression.
  For example, your filter may simply be the word 'spam'.  Or, if a regular
  expression your filter may be '/spam/i'.  For more information on creating
  valid regular expressions visit this page:
    http://www.php.net/manual/en/ref.pcre.php

  Custom filters can scan any combination of the content itself, the referrer
  URL associated with the posted content, and the user agent that was used to
  post the content.

  Matching filters can be used to detect spam content as well as to detect non-
  spam content.  For other filters you may simply want to note that a match
  means that probably is or probably is not spam.


  Node age filter:
  ----------------
  The node age filter only affects comments.  It ignores new nodes and users.
  When comments are posted, the node age filter looks at how long ago the
  node was posted to your website.  The older the node, the more likely the
  filter considers the comment to be spam.

  This module can be configured by visiting "Administer >> Site configuration >>
  Spam >> Filters >> Node age".  Here you can define what qualifies as "Old
  content", and what qualfies as "Really old content".  By default, "old
  content" is content that was posted more than 4 weeks ago, and comments
  posted on old content are considered 85% likely to be spam.  "Really old
  content" is content that was posted more than 8 weeks ago, and comments
  posted on really old content are considerd 99% likely to be spam.


  Bayesian filter:
  ----------------
  The Bayesian filter performs simple statistical analysis on content, learning
  from spam and non-spam that it sees to determine the liklihood that new
  content is or is not spam. The filter starts out knowing nothing, and has to
  be trained every time it makes a mistake. This is done by marking spam
  content on your site as spam when you see it. Each word of the spam content
  will be remembered and assigned a probability. The more often a word shows up
  in spam content, the higher the probability that future content with the same
  word is also spam.

  When first enabling the Bayesian filter, it is recommended that you visit
  "Administer >> Site configuration >> Spam >> Filters" and set the Gain for
  this module to a low value.  This is because until the module is trained, it
  will assume that all words have a 40% liklihood of being spam.

  As spam is posted to your website, simply click the 'Mark as spam' link to
  start training your Bayesian filter.  You should also regularly visit 
  "Administer >>  Content management >> Comments" and put a checkmark next to
  new comments that you know are valid and are not spam, then select "Teach
  filters selected comments are not spam" and click the "Update" button.  This
  step is critical to teaching your Bayesian filter what is and what is not
  spam.

  The Bayesian filter is language agnostic.  It does not have any configuration
  options at this time.


---------------
Reviewing Spam:
---------------
All content that has been marked as spam can be reviewed by visiting "Administer
>> Content management >> Spam".  You can optionally choose to filter this
listing by content type and/or IP address.  Controls are provided to easily
mark the content as not spam, or to simply publish or unpublish it.

Comment spam can also be found by visiting "Administer >> Content management >>
Comments >> Spam".  From this page, spam comments can be marked as not-spam or
simply deleted.


---------
Feedback:
---------
The spam filter is a useful collection of tools, but it can certainly make
mistakes, marking valid content as spam.  Users of your website can help you
to better train your filters by providing feedback when their content is
incorrectly blocked by your spam filters.

As an administrater, you should regularly go to "Administer >> Content
management >> Spam >> feedback" to review any feedback provided by your
visitors.  Carefully review the content and their feedback before
deciding whether or not to post the blocked content.  If you publish the
content, your filters will automatically learn that this content should not
have been blocked.  If you do not publish the content, it will be permanently
deleted from your website.


--------
Reports:
--------
The spam module implements its own custom logging facility.  These logs can be
reviewed by visiting "Administer >> Reports >> Spam logs".  Your log level will
determine just how much information is logged about each piece of content that
is scanned with the spam module.  If significant information is being logged,
you may find it useful to click the 'trace' link to trace through all actions
taken by the spam module.  You can also click the 'detail' link to see more
information about each log entry.

At the top of this page, click the "Statistics" link to see learn more about
how the spam filter is performing.  At this time only raw data is collected,
but at a future time we plan to provide useful reports showing the effectiveness
of the spam filter modules.

Finally, click the "Blocked IPs" tab to see a list of all IP addresses that
are currently being blocked by the spam filter.  This page will also show how
many times a given IP address has been blocked from posting content, as well
as the last time the IP address was blocked.


--------------
Configuration:
--------------
Initial configuration of this module is documented in INSTALL.txt.

Configuration of the module is done at "Administer >> Site configuration >>
Spam".  On this page, you can tell the module which types of content should
be scanned.  You can also tell the module which actions it should take when
spam is detected.


  Advanced configuration:
  -----------------------
  It is generally recommended that you do not make any changes to the advanced
  configuration options.

  The spam threshold is used to decide what content is spam.  All content is
  assigned a score from 1 to 99.  Any content with a score that is equal to or
  greater than the spam threshold is considered to be spam.  Any content with a
  score that is less than the spam threshold is considered to not be spam.
  Changing the spam threshold can have negative consequences, especially on
  websites that have been operating for a long time with a different spam
  threshold.  Old content that has already been scanned will not be affected
  when you change the spam threshold -- this setting only affects new content.

  When trying to learn how the spam filters work, or trying to understand why
  content is incorrectly slipping through the filters or being marked as spam,
  it can be helpful to change the log level.  The debug log level will provide
  you with a huge amount of information about each piece of content that is
  scanned by your filters, but it will also result in a large database load
  from writing all of these logs.

  Many individual spam filters also have their own configuration which is
  already defined earlier in this document.


------
Other:
------
TODO: Describe how to add custom CSS tags to your theme (override theme_comment)

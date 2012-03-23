README.txt file
===============

Settings
========

Enable module checkbox. 
  If this is unchecked the module is NOT activated and no 
  automated logouts will occur.

Timeout value in seconds.
  The is the "inactivity" timeout value. It's important to note
  that the server's only way of knowing about activity is when
  a browser requests a page from it. If the user is happily sitting
  there, say typing an enormous blog entry, then there's no way for the
  server to know the actual user is very busy until he/she submits the
  page. The server checks for any submissions (input) by the user.
  If there is content being added, the timeout is effectivly
  cancelled and a new session starts.

Browser refresh delta:
  If this is set to a non zero value, this time will be added to the
  "Timeout value in seconds" variable and be used to set a :-
    <meta http-equiv="refresh" content="xxx">
  line in the <head> section of the page. It's likely that clocks
  won't be perfect so make the browser wait sometime longer than the
  actual time out value. This effectively sends the browser back to
  the homepage with the user logged out. Set this to 0 (zero) to disable
  the <meta> tag.
 
  Note, as stated above, we test for content submission before doing
  a logout so that users do not loose work. However, if you force a
  meta refresh, this is not possible as the browser will refresh the
  page without doing a submit or a preview. In this case, users may
  loose work so use this option carefully. There's nothing more
  annoying than loosing your work (except maybe hemorrhoids! ;)

Enable watchdog auto-logout logging:
  Check on to have the module record when it expires a user.

Set-up your site policy by role:
  You can specify, by role, how users are treated regarding timeout
  issues. The three types of policy are :

    Enforce: All users in this role have the timeout enforced on them
    Exclude: All users in this role do not have the timeout enforced on them
    By user: All users in this role have the choice to disable the timeout.

Important note! The module by default uses Enforce. However, if a user is
found to have any other exclusion (by role or by choice) this takes
precidence over any Enforce that might have been in place.

Finally, this module provides a block that displays the h,m,s left in this
session (hours minutes seconds). It can be enabled in Admin > blocks and
the title of the block can be set in the standard settings page. This block 
is only displayed if you have the countdowntimer module installed.

If a policy settings means a user is not subject to autologout (in a role
of Exclude or By user and user set in profile not to autologout) then the
block is not displayed. The block is only displayed to users who will be
logged out if appropiate.




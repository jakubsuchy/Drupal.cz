
This module is an enhancement of the Cookie Cache Bypass module that is a part
of the Drupal 6 Pressflow distribution.

The purpose of the module is to set a short-lived NO_CACHE cookie on form
submissions so that when using a reverse-proxy (such as Varnish) you could be
certain that your site visitors would get dynamic pages after form submissions.

This module performs the same function, but adds some customizations for users.

1. Rather than taking the minimum cache lifetime and adding 300 seconds to it as
the lifetime for the NO_CACHE cookie (this could be anywhere from 10 minutes to
1 day + 5 minutes) we allow the user to set the time they would like that cookie
to persist for.

2. Rather than setting the cookie path for / (the entire site) we allow users to
choose to set for / or the value of request_uri() followed by a trailing slash.
This means that if your form is at URL /foo and your post-form pages are always
/foo/something-else, then this setting will increase your cache hits. See
http://php.net/setcookie and read the information on the 'path' parameter for
more information

3. The original Cookie Cache Bypass module set the cookie as the (possibly) last
submit function run after a form submission. Sometimes, if previous submit or
validate functions redirected before the cookie cache bypass submit function was
run, the NO_CACHE cookie would not get set. This module allows you to pick from
4 possible times to have that cookie be set.

  a). Before all other validate functions. This is the most aggressive setting,
but it ensures your cookie will always be set. The downside is that if
spammers are hitting your forms, they will always also get non-cached pages
and you could see more cache misses than you would like with your reverse proxy.

  b). After all other validate functions. This means that all form input has
been validated and is either good (and the user is proceeding to the submit
function) or it is bad and they will be sent back to the form. This is OK, so
long as you have no other custom validate functions that may redirect elsewhere.

  c). Before all other submit functions. This is essentially the same as b and
is an option in case some other custom submit action are configured to run
between validate and submit (though I honestly can't think of a scenario when
this happens).

  d). After all other submit functions. This is typically when the original
pressflow version of this module ran the cookie addition. This is the time that
is best for reverse-proxy cache hits, but the worst time if there is any
dynamic reaction in validate functions, or if there are redirects in validate
or submit functions that run before this one.



If you have questions about this module, contact Michael.Cooper@acquia.com.

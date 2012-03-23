<?php
// $Id: subscriptions_mail.templates.inc,v 1.5 2008/08/30 12:42:40 salvis Exp $

/**
 * @file
 * Subscriptions module mail templates constants.
 */

define('SUBSCRIPTIONS_COMMENT_MAILKEY', 'subscriptions_content_node-nid+comment');
define('SUBSCRIPTIONS_DIGEST_MAILKEY', 'subscriptions_mail_digest');

function subscriptions_mail_template_raw($key, $langcode) {
  switch ($key) {

    case 'SEP':      return // SUBSCRIPTIONS_DEFAULT_SEPARATOR
t('| --------------------------------------------------------------------
', array(), $langcode);

    case 'SUBJ':     return // SUBSCRIPTIONS_DEFAULT_SUBJECT
t('[!site] !subs_type subscription for !name: !title', array(), $langcode);

    case 'FOOT':     return // SUBSCRIPTIONS_DEFAULT_FOOTER
t('| This is an automated message. Please do NOT reply to the sender address!
| To manage your subscriptions go to
| !manage_url
', array(), $langcode);

    case 'BODY':     return // SUBSCRIPTIONS_DEFAULT_BODY
t('| Greetings, !name.
|
| Your subscription on !site
| notifies you of {{!is_new==0?:a new post:
}}{{!is_updated==0?:an updated post:
}}{{!is_old==0?:new comments:
}}|
{{!is_published==1?:| ***** This post is unpublished! *****
}}{{!term_name==!term_name?:| Category: !term_name
}}{{!sender_name==!sender_name?:| Author: !sender_name
}}{{!sender_has_contact_page==0?:| Contact: !sender_contact_page
}}| Title: !title
{{!is_old==1?:
!teaser

}}| Link: !url
| Direct unsubscribe link (!subs_type):
| !unsubscribe_url
{{!has_new_comments==1?!SEPARATOR!comments:}}!SEPARATOR!FOOTER', array('!SEPARATOR' => subscriptions_mail_template_raw('SEP', $langcode), '!FOOTER' => subscriptions_mail_template_raw('FOOT', $langcode)), $langcode);

    case 'CITEM':    return // SUBSCRIPTIONS_DEFAULT_COMMENT_ITEM
t('| {{!comment_is_new==1?New:Updated}} {{!comment_is_published==1?:UNPUBLISHED }}comment:
| Author: !comment_name
| Title: !comment_title

!comment_text

| Link: !comment_url
', array(), $langcode);

    case 'DSUBJ':    return // SUBSCRIPTIONS_DEFAULT_DIGEST_SUBJECT
t('[!site] Subscription Digest for !name', array(), $langcode);

    case 'DBODY':    return // SUBSCRIPTIONS_DEFAULT_DIGEST_BODY
t('| Greetings, !name.
|
| Your subscriptions on !site
| notify you of the following changes since the previous digest:
|
!bodies!SEPARATOR!FOOTER', array('!SEPARATOR' => subscriptions_mail_template_raw('SEP', $langcode), '!FOOTER' => subscriptions_mail_template_raw('FOOT', $langcode)), $langcode);

    case 'DITEM':    return // SUBSCRIPTIONS_DEFAULT_DIGEST_ITEM_BODY
t('{{!sender_name==!sender_name?:| Author: !sender_name
}}| Title: !title
{{!is_new==0?:!teaser
}}| Link: !url
| Direct unsubscribe link (!subs_type):
| !unsubscribe_url
{{!has_new_comments!=1?:!comments}}', array(), $langcode);

    case 'DITEMCMT': return // SUBSCRIPTIONS_DEFAULT_DIGEST_ITEM_BODY_COMMENT
t('| Comment: !comment_title
', array(), $langcode);

  }
}

function subscriptions_mail_template($key, $langcode) {
  static $templates;
  if (!isset($templates[$langcode][$key])) {
    $templates[$langcode][$key] = subscriptions_mail_template_raw($key, $langcode);
  }
  return $templates[$langcode][$key];
}

/**
 * Retrieve a mail template, either from the mail_edit table or take
 * our default template in this file, passing it through t().
 * 
 * @param string $mailkey
 *  the full mailkey, used as id in the mail_edit table
 * @param string $langcode
 *  the language code, used as language in the mail_edit table or for
 *  translating out default template
 * @param string $part
 *  either 'subject' or 'body', used to indicate the column to retrieve
 *  from the mail_edit table
 * @param string $key
 *  the key for calling subscriptions_mail_template() to retrieve the
 *  default template, if there's no match in the mail_edit table
 */
function subscriptions_mail_template_load($mailkey, $langcode, $part, $key)
{
  if (module_exists('mail_edit')) {
    static $cache = array();
    if (!isset($cache[$langcode][$mailkey]))
    {
      $cache[$langcode][$mailkey] = db_fetch_object(db_query("SELECT * FROM {mail_edit} WHERE id = '%s' and language = '%s'", $mailkey, $langcode));
    }
    if (isset($cache[$langcode][$mailkey]->$part)) {
      return $cache[$langcode][$mailkey]->$part;
    }
  }
  return subscriptions_mail_template($key, $langcode);
}


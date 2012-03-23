<?php

/**
 * @file
 * Custom Upgrade include file.
 */

define('SPAM_FILTER_CUSTOM_PLAIN', 0);
define('SPAM_FILTER_CUSTOM_REGEX', 1);
define('SPAM_FILTER_CUSTOM_URL', 2);

define('SPAM_FILTER_CUSTOM_ALWAYS_SPAM', 0);
define('SPAM_FILTER_CUSTOM_USUALLY_SPAM', 1);
define('SPAM_FILTER_CUSTOM_USUALLY_NOTSPAM', 2);
define('SPAM_FILTER_CUSTOM_NEVER_SPAM', 3);
define('SPAM_FILTER_CUSTOM_DISABLED', 4);
define('SPAM_FILTER_CUSTOM_MAYBE_SPAM', 5);
define('SPAM_FILTER_CUSTOM_MAYBE_NOTSPAM', 6);

define('SPAM_FILTER_CUSTOM_ACTION_HEADER', 0x4);
define('SPAM_FILTER_CUSTOM_ACTION_BODY', 0x8);

function spam_filter_custom_upgrade() {
  if ($result = db_result(db_query("SHOW TABLES LIKE 'old_spam_custom'"))) {
    drupal_set_message('Upgrading custom spam filter rules.');
    $result = db_query('SELECT * FROM {old_spam_custom}');
    while ($old = db_fetch_object($result)) {
      switch ($old->style) {
        case SPAM_FILTER_CUSTOM_PLAIN:
        case SPAM_FILTER_CUSTOM_URL:
          $style = SPAM_FILTER_CUSTOM_STYLE_PLAIN;
          break;
        case SPAM_FILTER_CUSTOM_REGEX:
          $style = SPAM_FILTER_CUSTOM_STYLE_REGEX;
          break;
      }
      switch ($old->effect) {
        case SPAM_FILTER_CUSTOM_DISABLED:
        default:
          $status = SPAM_FILTER_CUSTOM_STATUS_DISABLED;
          break;
        case SPAM_FILTER_CUSTOM_MAYBE_SPAM:
        case SPAM_FILTER_CUSTOM_USUALLY_SPAM:
          $status = SPAM_FILTER_CUSTOM_STATUS_PROBABLY;
          break;
        case SPAM_FILTER_CUSTOM_USUALLY_NOTSPAM:
        case SPAM_FILTER_CUSTOM_MAYBE_NOTSPAM:
          $status = SPAM_FILTER_CUSTOM_STATUS_PROBABLYNOT;
          break;
        case SPAM_FILTER_CUSTOM_NEVER_SPAM:
          $status = SPAM_FILTER_CUSTOM_STATUS_NOTSPAM;
          break;
        case SPAM_FILTER_CUSTOM_ALWAYS_SPAM:
          $status = SPAM_FILTER_CUSTOM_STATUS_SPAM;
          break;
      }
      if ($old->action & SPAM_FILTER_CUSTOM_ACTION_HEADER || $old->action & SPAM_FILTER_CUSTOM_ACTION_BODY) {
        $scan = SPAM_FILTER_CUSTOM_SCAN_CONTENT;
      }
      else {
        $scan = SPAM_FILTER_CUSTOM_SCAN_CONTENT;
        $status = SPAM_FILTER_CUSTOM_STATUS_DISABLED;
      }
      db_query("INSERT INTO {spam_filter_custom} (filter, style, status, scan, matches, last) VALUES('%s', %d, %d, %d, %d, %d)", $old->filter, $style, $status, $scan, $old->matches, $old->last);
    }
    // Done with upgrade, drop old table.
    $ret = array();
    db_drop_table($ret, 'old_spam_custom');
  }
}


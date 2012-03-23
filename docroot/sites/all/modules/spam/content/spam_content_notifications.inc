<?php

/**
 * @file
 * Include file for integration with the contact form.
 */

/**
 * Implementation of hook_notifications()
 */
function spam_notifications($op, &$event, $arg1 = NULL, $arg2 = NULL) {
  if (!spam_filter_content_type('notifications', 'notifications', array())) return;

  switch ($op) {
    case 'event trigger':
      // Check for spam
      switch($event->action) {
        case 'update':
        case 'insert':
          // Spam node that should not trigger a notification
          if (isset($event->node->is_spam) && $event->node->is_spam) {
            unset($event->save, $event->queue);
          }
          break;
        case 'comment':
          // Spam comment that should not trigger a notification
          if (isset($event->comment->is_spam) && $event->comment->is_spam) {
            unset($event->save, $event->queue);
          }
          break;
      } 
  }
}


/**
 * Spam module _spamapi() hook.
 */
function notifications_spamapi($op, $arg1 = NULL, $arg2 = NULL, $arg3 = NULL) {
  switch ($op) {
    case 'content_module':
      // Register with the spam api as a content type module.
      return 'notifications';

    case 'content_id':
      // Notifications are aren't scanned by themselves.
      return 0;

    case 'content_types':
      $types = array();
      $types[] = array(
        'name' => 'notifications',
        'module' => 'notifications',
        'title' => t('Notifications'),
        'description' => t("Allows users to get notified about updates on your website. Messages won't be sent if the content triggering them is found to be spam. Messages will get sent later if the content is marked 'not spam' and published."),
        'default_value' => 0,
      );

      return $types;

    case 'filter_content_type':
      return variable_get('spam_filter_notifications', FALSE);

    case 'filter_fields':
      // Tell spam filter which fields should be scanned for spam.
      // We do not want to actually filter notifications. We rely on the
      // filtering of the content (node/comment/...) that created the notification.
      return array();

    case 'feedback_form':
    case 'error_form':
      // We do not need a separate form. See above.
      return array();

    case 'mark_as_not_spam':
    case 'feedback_approved':
    case 'publish':
    case 'load':
    case 'title':
    case 'status':
    case 'edit_link':
    case 'link':
    case 'hostname':
    case 'redirect':
    case 'overview_filter_join':
    case 'overview_filter_where':
    case 'unpublish':
    case 'hold':
      break;
  }
}


#!/bin/sh
#
# db-copy Cloud hook: db-scrub
#
# Scrub important information from a Drupal database.
#
# Usage: db-scrub.sh site target-env db-name source-env

site="$1"
target_env="$2"
db_name="$3"
source_env="$4"

echo "$site.$target_env: Scrubbing database $db_name"

(cat <<EOF
-- 
-- Scrub important information from a Drupal database.
-- 

UPDATE users SET name=CONCAT('user', uid), pass=md5('heslo'), init=CONCAT('user', uid, '@example.com') WHERE uid != 0;
UPDATE users SET mail=CONCAT('user', uid, '@example.com') WHERE uid != 0;
UPDATE comments SET name='Anonymous', mail='', homepage='http://example.com', hostname='1.1.1.1' WHERE uid=0;
UPDATE profile_values SET value = '1';
DELETE FROM watchdog;
DELETE FROM sessions;
DELETE FROM signup;
DELETE FROM signup_log;
UPDATE location SET city = 'Schneekoppe', latitude = '50.735942477452205', longitude = '15.739728212356567';
UPDATE variable SET value = 's:32:"aff4833333333m7a2363233333333333";' WHERE name = 'mollom_public_key';
UPDATE variable SET value = 's:32:"aff4833333333m7a2363233333333332";' WHERE name = 'mollom_private_key';

TRUNCATE accesslog;
TRUNCATE access;
TRUNCATE cache;
TRUNCATE cache_filter;
TRUNCATE cache_menu;
TRUNCATE cache_page;
TRUNCATE cache_views;
TRUNCATE cache_views_data;
TRUNCATE devel_queries;
TRUNCATE devel_times;
TRUNCATE flood;
TRUNCATE history;
TRUNCATE search_dataset;
TRUNCATE search_index;
TRUNCATE search_total;
TRUNCATE sessions;
TRUNCATE watchdog;


EOF
) | drush @$site.$target_env ah-sql-cli --db=$db_name

mkdir -p /home/www/drupal/database
cd /home/www/drupal/database

# Dump live DB
/opt/drush/drush --root=/home/www/drupal/public_html/www/ sql-dump --skip-tables-key=common > /home/www/drupal/database/live.sql
# Delete all tables
mysqldump -u d_cz_scrub --password=$1 --add-drop-table --no-data drupal_cz_scrubbed | grep ^DROP | mysql -u d_cz_scrub --password=$1 drupal_cz_scrubbed
# Load live DB
mysql drupal_cz_scrubbed -u d_cz_scrub --password=$1 --default-character-set=utf8 < /home/www/drupal/database/live.sql
# Delete live DB
rm -f /home/www/drupal/database/live.sql
# Scrub
mysql drupal_cz_scrubbed -u d_cz_scrub --password=$1 --default-character-set=utf8 < /home/www/drupal/live/scrub.sql
# Dump scrubbed DB to live site
mysqldump -u d_cz_scrub --password=$1 --add-drop-table drupal_cz_scrubbed > /home/www/drupal/live/www/drupal_cz.sql
bzip2 /home/www/drupal/live/www/drupal_cz.sql

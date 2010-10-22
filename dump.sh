cd /home/www/drupal/public_html/www
drush sql dump > /home/www/drupal/database/dump.sql
cd /home/www/drupal/database
mysql drupal_cz_scrapped -u drupal_cz_scrapped --password=$1 --default-character-set=utf8 < /home/www/drupal/database/dump.sql
mysql drupal_cz_scrapped -u drupal_cz_scrapped --password=$1 --default-character-set=utf8 < /home/www/drupal/code/scrub.sql


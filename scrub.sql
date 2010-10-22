# Will Scrub a drupal.cz database.
# Jakub Suchy, 22.10.2010

UPDATE users SET name=CONCAT('user', uid), pass='heslo', init=CONCAT('user', uid, '@example.com') WHERE uid != 0;
UPDATE users SET mail=CONCAT('user', uid, '@example.com') WHERE uid != 0;
UPDATE comments SET name='Anonymous', mail='', homepage='http://example.com' WHERE uid=0;
UPDATE profile_values SET value = '1';




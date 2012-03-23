# Will Scrub a Drupal.cz database.
# Jakub Suchy, 22.10.2010

UPDATE users SET name=CONCAT('user', uid), pass='heslo', init=CONCAT('user', uid, '@example.com') WHERE uid != 0;
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

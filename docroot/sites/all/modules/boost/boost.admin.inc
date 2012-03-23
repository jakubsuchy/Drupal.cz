<?php
// $Id: boost.admin.inc,v 1.1.2.1.2.3.2.79 2009/09/30 09:17:31 mikeytown2 Exp $

/**
 * @file
 * All the code for the Boost module's administrative interface.
 */

//////////////////////////////////////////////////////////////////////////////
// Boost settings form

/**
 * Performs alterations to the system settings form before it is rendered.
 *
 * @see hook_form_alter()
 *   in boost.module
 */
function boost_admin_performance_page($form = array()) {
  $form['#title'] = t('Anonymous page caching');
  $form['page_compression']['#title'] = t('Gzip page compression (Boost & Core)');
  $form['#description'] .= ' ' . t('The page must first be visited by an anonymous user in order for the cache to work on subsequent requests for that page. Boost & Core caching do not work for logged in users.');
  $form['boost_core_clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear core page cached data: !count pages', array('!count' => boost_count_core_db())),
    '#submit' => array('boost_clear_core_page_cache_submit'),
  );

  return $form;
}

/**
 * Process system_themes_form form submissions.
 */
function boost_admin_performance_page_submit($form, &$form_state) {
  variable_del('boost_previously');
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);

  if (empty($boost_enabled) && !empty($boost_previously)) { // the cache was previously enabled
    variable_set('boost_ignore_flush', 0);
    if (boost_cache_clear_all()) {
      drupal_set_message(t('Boost: Static page cache cleared.'));
    }
  }
  else if ($boost_ignore_flush == 0 && (variable_get('preprocess_css', FALSE)==TRUE || variable_get('preprocess_js', FALSE)==TRUE)) {
    if (boost_cache_clear_all()) {
      drupal_set_message(t('Boost: Static page cache cleared.'), 'warning');
    }
  }
}

/**
 * Submit callback; clear system caches.
 */
function boost_admin_clear_cache_submit($form, &$form_state) {
  drupal_flush_all_caches();
  if (boost_cache_clear_all()) {
    drupal_set_message(t('Static page cache (boost) and all other caches cleared.'));
  }
  else {
    drupal_set_message(
      t('Boost: Static page cache NOT cleared. To clear Boosts cache set "Ignore cache flushing:" to \'Disabled\' in the <a href="@link">boost advanced settings</a> & try again.', array('@link' => url('admin/settings/performance/boost', array('fragment' => 'edit-boost-ignore-flush-0-wrapper')))), 'warning');
    drupal_set_message(t('All other caches cleared.'));
  }
}

/**
 * Form builder; Displays Boost's configuration page.
 *
 * @see system_settings_form()
 */
function boost_admin_boost_performance_page() {
  Global $base_path;

  $period = drupal_map_assoc(array(0, 60, 180, 300, 600, 900, 1800, 2700, 3600, 10800, 21600, 32400, 43200, 64800, 86400, 2*86400, 3*86400, 4*86400, 5*86400, 6*86400, 604800, 2*604800, 3*604800, 4*604800, 8*604800, 16*604800, 52*604800), 'format_interval');
  $period[0] = '<' . t('none') . '>';

  $form['boost'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Boost File Cache'),
  );
  $form['boost']['boost_enabled'] = array(
    '#type'          => 'radios',
    '#title'         => t('Boost - Static page cache'),
    '#default_value' => variable_get('boost_enabled', CACHE_DISABLED),
    '#options'       => array(
      CACHE_DISABLED => t('Disabled'),
      CACHE_NORMAL => t('Enabled'),
      CACHE_AGGRESSIVE => t('(Not Recommended) Set Boost & Core (if enabled) cache for each page'),
    ),
    '#description'   => t('Static page caching is a mechanism that stores dynamically generated web pages as HTML files in a special cache directory located under the Drupal installation directory. By caching a web page in this manner, the web server can serve it out in the fastest possible manner, without invoking PHP or Drupal at all. While this does provide a significant performance and scalability boost, you should note that it could have negative usability side-effects unless your site is targeted at an audience consisting mostly of "anonymous" visitors.'),
  );
  $form['boost']['page_compression'] = array(
    '#type' => 'radios',
    '#title' => t('Gzip page compression (Boost & Core)'),
    '#default_value' => variable_get('page_compression', TRUE),
    '#options' => array(t('Disabled'), t('Enabled')),
    '#description' => t("By default, Drupal compresses the pages it caches in order to save bandwidth and improve download times. This option should be disabled when using a webserver that performs compression."),
  );
  $form['boost']['boost_cache_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Boost - HTML - Default minimum cache lifetime'),
    '#default_value' => BOOST_CACHE_LIFETIME,
    '#options' => $period,
    '#description' => !BOOST_CACHE_HTML ? t('Enable the caching of this content type to enable this selection box.') : t('The minimum cache lifetime is the minimum amount of time that will elapse before the cache is emptied. Cache lifetime gets checked on cron runs.'),
    '#disabled' => !BOOST_CACHE_HTML,
  );
  $form['boost']['boost_cache_xml_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Boost - XML - Default minimum cache lifetime'),
    '#default_value' => BOOST_CACHE_XML_LIFETIME,
    '#options' => $period,
    '#description' => !BOOST_CACHE_XML ? t('Enable the caching of this content type to enable this selection box.') : t('The minimum cache lifetime is the minimum amount of time that will elapse before the cache is emptied. Cache lifetime gets checked on cron runs.'),
    '#disabled' => !BOOST_CACHE_XML,
  );
  $form['boost']['boost_cache_json_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Boost - JSON - Default minimum cache lifetime'),
    '#default_value' => BOOST_CACHE_JSON_LIFETIME,
    '#options' => $period,
    '#description' => !BOOST_CACHE_JSON ? t('Enable the caching of this content type to enable this selection box.') : t('The minimum cache lifetime is the minimum amount of time that will elapse before the cache is emptied. Cache lifetime gets checked on cron runs.'),
    '#disabled' => !BOOST_CACHE_JSON,
  );
  $form['boost']['boost_clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear ALL Boost cached data: !count pages', array('!count' => boost_count_db(0))),
    '#submit' => array('boost_clear_cache_submit'),
  );
  $form['boost']['boost_clear_expired'] = array(
    '#type' => 'submit',
    '#value' => t('Clear Boost expired data: !count pages', array('!count' => boost_count_db(1))),
    '#submit' => array('boost_clear_expired_cache_submit'),
  );

  // Cacheability settings
  $options = array(t('Cache every page except the listed pages.'), t('Cache only the listed pages.'));
  $description = t("Enter one page per line as Drupal paths. The '*' character is a wild-card. Example paths are '%blog' for the blog page and %blog-wild-card for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wild-card' => 'blog/*', '%front' => '<front>'));
  if (user_access('use PHP for block visibility')) {
    $options[] = t('Cache pages for which the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).');
    $description .= t('If the PHP-mode is chosen, enter PHP code between %php. Note that executing incorrect PHP-code can severely break your Drupal site.', array('%php' => '<?php ?>'));
  }
  $form['cacheability'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Boost cacheability settings'),
  );
  $form['cacheability']['boost_cache_query'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Cache pages that contain URL Variables'),
    '#default_value' => BOOST_CACHE_QUERY,
    '#description'   => t('Boost will cache pages that end with "?page=1" among others (anything with a "?" in the url).'),
  );
  $form['cacheability']['boost_cache_html'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Cache html documents/pages'),
    '#default_value' => BOOST_CACHE_HTML,
    '#description'   => t('Boost will cache most drupal pages.'),
  );
  $form['cacheability']['boost_cache_xml'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Cache .xml & /feed'),
    '#default_value' => BOOST_CACHE_XML,
    '#description'   => t('Boost will cache .xml and /feed urls as xml data.'),
  );
  $form['cacheability']['boost_cache_json'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Cache ajax/json'),
    '#default_value' => BOOST_CACHE_JSON,
    '#description'   => t('Boost will cache ajax/json responses.'),
  );
  $form['cacheability']['boost_cache_css'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Cache .css'),
    '#default_value' => BOOST_CACHE_CSS,
    '#description'   => t('Boost will cache CSS files.'),
  );
  $form['cacheability']['boost_cache_js'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Cache .js'),
    '#default_value' => BOOST_CACHE_JS,
    '#description'   => t('Boost will cache javascript files.'),
  );
  $form['cacheability']['boost_cacheability_option'] = array(
    '#type'          => 'radios',
    '#title'         => t('Statically cache specific pages'),
    '#options'       => $options,
    '#default_value' => BOOST_CACHEABILITY_OPTION,
  );
  $form['cacheability']['boost_cacheability_pages'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Pages'),
    '#default_value' => BOOST_CACHEABILITY_PAGES,
    '#description'   => $description,
  );


  // directories
  $form['directories'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Boost directories and file extensions'),
  );
  $form['directories']['boost_root_cache_dir'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Cache Dir'),
    '#default_value' => BOOST_ROOT_CACHE_DIR,
    '#size'          => 15,
    '#maxlength'     => 255,
    '#required'      => TRUE,
    '#description'   => t(''),
  );
  $form['directories']['boost_multisite_single_db'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Do not store the cache file path in the database'),
    '#default_value' => BOOST_MULTISITE_SINGLE_DB,
    '#description'   => t('Enabling will allow for correct multi-site caching, in cases where different content is served from the same Drupal installation, based on domain. Examples: Multi-site with a single/shared database, site translation detection based on domain, and the contributed "Domain Access" module.'),
  );
  if (!BOOST_MULTISITE_SINGLE_DB) {
//     $form['directories']['boost_host'] = array(
//       '#type'          => 'textfield',
//       '#title'         => t('Hostname'),
//       '#default_value' => BOOST_HOST,
//       '#size'          => 60,
//       '#maxlength'     => 255,
//       '#description'   => t(''),
//     );
    $form['directories']['boost_file_path'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Cache file path'),
      '#default_value' => BOOST_FILE_PATH,
      '#size'          => 60,
      '#maxlength'     => 255,
      '#required'      => TRUE,
      '#description'   => t('A file system path where the static cache files will be stored. This directory has to exist and be writable by Drupal. The default setting is to store the files in a directory named %default-path under the Drupal installation directory. If you change this, you must also change the URL rewrite rules in your web server configuration (.htaccess for Apache, lighttpd.conf for Lighttpd), or caching will not work.', array('%default-path' => boost_cache_directory(BOOST_HOST, FALSE))),
    );
  }
  $form['directories']['generated'] = array(
    '#type'          => 'fieldset',
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
    '#title'         => t('Generated output storage (HTML, XML, AJAX)'),
  );
  $form['directories']['generated']['boost_normal_dir'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Dir for non compressed files'),
    '#default_value' => BOOST_NORMAL_DIR,
    '#size'          => 15,
    '#maxlength'     => 255,
    '#description'   => t('Leave blank to not use a subdir'),
  );
  $form['directories']['generated']['boost_gzip_dir'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Dir for compressed files'),
    '#default_value' => BOOST_GZIP_DIR,
    '#size'          => 15,
    '#maxlength'     => 255,
    '#description'   => t('Leave blank to not use a subdir'),
  );
  $form['directories']['generated']['boost_char'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Character used to replace "?"'),
    '#default_value' => BOOST_CHAR,
    '#size'          => 15,
    '#maxlength'     => 255,
    '#required'      => TRUE,
    '#description'   => t('Best to leave at _'),
  );
  if (BOOST_CACHE_HTML) {
    $form['directories']['generated']['boost_file_extension'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Cache html file extension'),
      '#default_value' => BOOST_FILE_EXTENSION,
      '#size'          => 10,
      '#maxlength'     => 32,
      '#required'      => TRUE,
      '#description'   => t('The file extension to append to the file name of the generated cache files. Note that this setting is of no relevance to any public URLs, and it is strongly recommended to leave this as the default \'.html\' unless you know what you are doing. If you change this, you must also change the URL rewrite rules in your web server configuration (.htaccess for Apache, lighttpd.conf for Lighttpd), or caching will not work.'),
    );
  }
  if (BOOST_CACHE_XML) {
    $form['directories']['generated']['boost_xml_extension'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Cache xml file extension'),
      '#default_value' => BOOST_XML_EXTENSION,
      '#size'          => 10,
      '#maxlength'     => 32,
      '#required'      => TRUE,
      '#description'   => t('The file extension to append to the file name of the generated cache files. Note that this setting is of no relevance to any public URLs, and it is strongly recommended to leave this as the default \'.xml\' unless you know what you are doing. If you change this, you must also change the URL rewrite rules in your web server configuration (.htaccess for Apache, lighttpd.conf for Lighttpd), or caching will not work.'),
    );
  }
  if (BOOST_CACHE_JSON) {
    $form['directories']['generated']['boost_json_extension'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Cache ajax/json file extension'),
      '#default_value' => BOOST_JSON_EXTENSION,
      '#size'          => 10,
      '#maxlength'     => 32,
      '#required'      => TRUE,
      '#description'   => t('The file extension to append to the file name of the generated cache files. Note that this setting is of no relevance to any public URLs, and it is strongly recommended to leave this as the default \'.js\' unless you know what you are doing. If you change this, you must also change the URL rewrite rules in your web server configuration (.htaccess for Apache, lighttpd.conf for Lighttpd), or caching will not work.'),
    );
  }
  $form['directories']['static'] = array(
    '#type'          => 'fieldset',
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
    '#title'         => t('Static storage (CSS, JS)'),
  );
  $form['directories']['static']['boost_perm_normal_dir'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Dir for more permanent files (css/js)'),
    '#default_value' => BOOST_PERM_NORMAL_DIR,
    '#size'          => 15,
    '#maxlength'     => 255,
    '#description'   => t('Leave blank to not use a subdir'),
  );
  $form['directories']['static']['boost_perm_gz_dir'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Dir for compressed more permanent files (css/js)'),
    '#default_value' => BOOST_PERM_GZ_DIR,
    '#size'          => 15,
    '#maxlength'     => 255,
    '#description'   => t('Leave blank to not use a subdir'),
  );
  $form['directories']['static']['boost_perm_char'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Character used to replace "?" in the permanent dir'),
    '#default_value' => BOOST_PERM_CHAR,
    '#size'          => 15,
    '#maxlength'     => 255,
    '#required'      => TRUE,
    '#description'   => t('Best to leave at _'),
  );
  if (BOOST_CACHE_CSS) {
    $form['directories']['static']['boost_css_extension'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Cache css file extension'),
      '#default_value' => BOOST_CSS_EXTENSION,
      '#size'          => 10,
      '#maxlength'     => 32,
      '#required'      => TRUE,
      '#description'   => t('The file extension to append to the file name of the generated cache files. Note that this setting is of no relevance to any public URLs, and it is strongly recommended to leave this as the default \'.css\' unless you know what you are doing. If you change this, you must also change the URL rewrite rules in your web server configuration (.htaccess for Apache, lighttpd.conf for Lighttpd), or caching will not work.'),
    );
  }
  if (BOOST_CACHE_JS) {
    $form['directories']['static']['boost_js_extension'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Cache javascript file extension'),
      '#default_value' => BOOST_JS_EXTENSION,
      '#size'          => 10,
      '#maxlength'     => 32,
      '#required'      => TRUE,
      '#description'   => t('The file extension to append to the file name of the generated cache files. Note that this setting is of no relevance to any public URLs, and it is strongly recommended to leave this as the default \'.js\' unless you know what you are doing. If you change this, you must also change the URL rewrite rules in your web server configuration (.htaccess for Apache, lighttpd.conf for Lighttpd), or caching will not work.'),
    );
  }

  // Advanced settings
  $form['advanced'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Boost advanced settings'),
  );
  $form['advanced']['boost_expire_cron'] = array(
    '#type'          => 'radios',
    '#title'         => t('Clear expired pages on cron runs'),
    '#default_value' => (int)variable_get('boost_expire_cron', TRUE),
    '#options'       => array(
      0 => t('Disabled'),
      1 => t('Enabled'),
     ),
    '#description'   => t('Caution - If disabled you will need some other method of clearing cached pages. If Disabled, files have to be manually deleted from server or cleared using the administrative interface (<a href="@blocks">Enable</a> the <a href="@status">Boost: Pages cache status</a> block).', array('@blocks' => url('admin/build/block'), '@status' => url('admin/build/block/configure/boost/status'))),
  );
  $form['advanced']['boost_check_before_cron_expire'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Check database timestamps for any site changes. Only if theres been a change will boost flush the expired content on cron.'),
    '#default_value' => BOOST_CHECK_BEFORE_CRON_EXPIRE,
  );
  $form['advanced']['boost_pre_process_function'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Pre-process function'),
    '#default_value' => BOOST_PRE_PROCESS_FUNCTION,
    '#maxlength'     => 255,
    '#description'   => t('The name of a PHP function used to pre-process the contents of each page before writing them out to static files. The function is called with the contents of the page passed as a string argument, and its return value is used as the data written out to the disk.'),
  );
  $form['advanced']['boost_only_ascii_path'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Only allow ASCII characters in path'),
    '#default_value' => BOOST_ONLY_ASCII_PATH,
    '#description'   => t('Only allowing ACSII characters is a safe way to cache pages. It severely limits i18n support so this can be turned off. Fair warning, disabling this may cause "page not found" errors depending on your url structure (spaces are bad, ect...). If you follow RFC 3986 you should be ok.'),
  );
  $form['advanced']['boost_asynchronous_output'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Asynchronous Operation: output HTML, close connection, then store static file.'),
    '#default_value' => BOOST_ASYNCHRONOUS_OUTPUT,
    '#description'   => t('Run php in the background. When a cached page is generated, this will allow for faster page generation; downside is the headers are not the standard ones outputted by drupal; sends "Connection: close" instead of "Connection: Keep-Alive".'),
  );
  $form['advanced']['boost_flush_dir'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Clear all empty folders from cache.'),
    '#default_value' => BOOST_FLUSH_DIR,
    '#description'   => t('Disable this if you have to set settings for each dir/subdir, due to the way your server operates (permissions, etc...).'),
  );
  $form['advanced']['boost_flush_node_terms'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Clear all cached terms pages associated with a node on insert/update/delete'),
    '#default_value' => BOOST_FLUSH_NODE_TERMS,
    '#description'   => t('Works with view\'s taxonomy/term/% path as well as core.'),
  );
  $form['advanced']['boost_clear_cache_offline'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Clear Boosts cache when site goes offline'),
    '#default_value' => BOOST_CLEAR_CACHE_OFFLINE,
    '#description'   => t('Under site maintenance when the status is set to offline, boost clears its cache. If you do not want this to happen, clear this checkbox. Pages that are not cached will still send out a Site off-line message, so be smart if turning this off.'),
  );
  $form['advanced']['boost_overwrite_file'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Overwrite the cached file if it already exits'),
    '#default_value' => BOOST_OVERWRITE_FILE,
    '#description'   => t('This is useful if crawling a site before it goes live.'),
  );
  $form['advanced']['boost_halt_on_errors'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Do not cache if php error on page'),
    '#default_value' => BOOST_HALT_ON_ERRORS,
    '#description'   => t('Selected - Do not cache the page if there are PHP errors. Not Selected - Cache pages even if it might contain errors.'),
  );
  $form['advanced']['boost_halt_on_messages'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Do not cache if a message is on the page'),
    '#default_value' => BOOST_HALT_ON_MESSAGES,
    '#description'   => t('Selected - Do not cache the page if there are drupal messages. Not Selected - Cache pages even if it might contain a drupal message.'),
  );
  $form['advanced']['boost_disable_clean_url'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Turn off clean url\'s for logged in users'),
    '#default_value' => BOOST_DISABLE_CLEAN_URL,
    '#description'   => t('Drupal will output non clean url\'s for non anonymous users. This allows for the browser to cache the page and still have logging in work. This is more on the extreme side of tweaks.'),
  );
  $form['advanced']['boost_aggressive_gzip'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Aggressive Gzip: Deliver gzipped content independent of the request header.'),
    '#default_value' => BOOST_AGGRESSIVE_GZIP,
    '#description'   => t('In order to deliver gzipped content independent of the header, this will test for gzip compression in a small iframe by sending it compressed content. This compressed content is javascript which creates a cookie with a note of gzip support. On the server side it checks for the cookie and then sends out gzipped content accordingly. See <a href="@url">Website Performance - Activate Gzip</a>. In short some firewalls/proxies mangle the gzip header; this gets around that. iframe is on non compressed version of the frontpage only.', array('@url' => 'http://actionable-stats.com/website-performance-activate-gzip')),
  );
  $form['advanced']['boost_permissions_file'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Files: Enter in a 4 digit number (octal) that will be used by !link. Example 0664', array('!link' => l(t('chmod()'), 'http://php.net/chmod'))),
    '#default_value' => BOOST_PERMISSIONS_FILE,
    '#description'   => t('Sometimes because of funky servers you need it use a different file mode then the default.'),
  );
  $form['advanced']['boost_permissions_dir'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Directories: Enter in a 4 digit number (octal) that will be used by !link. Example 0775', array('!link' => l(t('chmod()'), 'http://php.net/chmod'))),
    '#default_value' => BOOST_PERMISSIONS_DIR,
    '#description'   => t('Sometimes because of funky servers you need it use a different file mode then the default.'),
  );
  $form['advanced']['boost_ignore_flush'] = array(
    '#type'          => 'radios',
    '#title'         => t('Ignore cache flushing'),
    '#default_value' => BOOST_IGNORE_FLUSH,
    '#options'       => array(
      0 => t('Disabled'),
      1 => t('Only Ignore Clear Entire Cache Commands (Recommended if caching css/js files)'),
      2 => t('Ignore Clear Entire Cache Commands & Cron Expiration'),
      3 => t('Ignore All Delete Commands (Not Recommended)'),
    ),
    '#description'   => t('Make a selection to put your site into a static cached state. Recommend turning on CSS & JS caching if enabled.'),
  );
  $form['advanced']['boost_verbose'] = array(
    '#type'          => 'select',
    '#title'         => t('Watchdog Verbose Setting'),
    '#default_value' => BOOST_VERBOSE,
    '#options'       => array(
      1 => t('1 Record almost no errors'),
      3 => t('3'),
      5 => t('5 Record all errors to the db log (watchdog)'),
    ),
  );

  // Crawler
  $form['crawler'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Boost crawler'),
  );
  $form['crawler']['boost_crawl_on_cron'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Enable the cron crawler'),
    '#default_value' => BOOST_CRAWL_ON_CRON,
    '#description'   => t('Pre-cache boosted URL\'s so they get cached before anyone accesses them. Enable the crawler first and save settings to use Preemptive Cache settings.'),
  );
  $form['crawler']['boost_loopback_bypass'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Do not flush expired content on cron run, instead recrawl and overwrite it.'),
    '#default_value' => BOOST_LOOPBACK_BYPASS,
    '#description'   => !BOOST_CRAWL_ON_CRON ? t('Enable the crawler first to enable this checkbox.') :!BOOST_OVERWRITE_FILE ? t('The "Overwrite the cached file if it already exits" must be turnned on inorder to enable this') : t('Bypass cron cache expiration and load to be expired content in the crawler. Allow this servers IP to bypass the boost cache. Check for fast but potentially stale; uncheck for slow but always fresh.'),
    '#disabled' => !BOOST_CRAWL_ON_CRON || !BOOST_OVERWRITE_FILE,
  );
  $form['crawler']['boost_push_html'] = array(
    '#type' => 'checkbox',
    '#title' => t('Preemptive Cache HTML'),
    '#default_value' => BOOST_PUSH_HTML,
    '#description' => !BOOST_CRAWL_ON_CRON ? t('Enable the crawler first to enable this checkbox.') : !BOOST_CACHE_HTML ? t('Enable the caching of this content type to enable this checkbox') : t('Crawl Site after cron runs, so the cache is primed.'),
    '#disabled' => !BOOST_CRAWL_ON_CRON || !BOOST_CACHE_HTML,
  );
  $form['crawler']['boost_push_xml'] = array(
    '#type' => 'checkbox',
    '#title' => t('Preemptive Cache XML'),
    '#default_value' => BOOST_PUSH_XML,
    '#description' => !BOOST_CRAWL_ON_CRON ? t('Enable the crawler first to enable this checkbox.') : !BOOST_CACHE_XML ? t('Enable the caching of this content type to enable this checkbox') : t('Crawl Site after cron runs, so the cache is primed.'),
    '#disabled' => !BOOST_CRAWL_ON_CRON || !BOOST_CACHE_XML,
  );
  $form['crawler']['boost_push_json'] = array(
    '#type' => 'checkbox',
    '#title' => t('Preemptive Cache AJAX/JSON'),
    '#default_value' => BOOST_PUSH_JSON,
    '#description' => !BOOST_CRAWL_ON_CRON ? t('Enable the crawler first to enable this checkbox.') : !BOOST_CACHE_JSON ? t('Enable the caching of this content type to enable this checkbox') : t('Crawl Site after cron runs, so the cache is primed.'),
    '#disabled' => !BOOST_CRAWL_ON_CRON || !BOOST_CACHE_JSON,
  );
  $form['crawler']['boost_crawl_url_alias'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Crawl All URL\'s in the url_alias table'),
    '#default_value' => BOOST_CRAWL_URL_ALIAS,
    '#description'   => !BOOST_CRAWL_ON_CRON ? t('Enable the crawler first to enable this checkbox.') : t('Preemptively cache all urls found in the Drupal url_alias table. This will crawl that page even if it is not expired. Enable & run cron to get the boost cache loaded.'),
    '#disabled' => !BOOST_CRAWL_ON_CRON
  );
  $form['crawler']['boost_crawler_throttle'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Crawler Throttle'),
    '#default_value' => BOOST_CRAWLER_THROTTLE,
    '#description'   => t('Wait X micro seconds in between hitting each url. 1000000 is 1 second.'),
  );
  $form['crawler']['boost_crawler_batch_size'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Crawler Batch Size'),
    '#default_value' => BOOST_CRAWLER_BATCH_SIZE,
    '#description'   => t('Number of URL\'s each thread grabs per database operation.'),
  );
  // TODO use BOOST_MAX_THREADS, or textfield...
  $form['crawler']['boost_crawler_threads'] = array(
    '#type'          => 'select',
    '#title'         => t('Number Of Threads'),
    '#default_value' => BOOST_CRAWLER_THREADS,
    '#options' => array(
      1 => 1,
      2 => 2,
      3 => 3,
      4 => 4,
      5 => 5,
      6 => 6,
      7 => 7,
      8 => 8,
    ),
    '#description'   => t('Be careful when choosing more then 2 threads.'),
  );
  $total = boost_crawler_total_count();
  //variable_get
  $number_done = min($total, unserialize(db_result(db_query("SELECT value FROM {variable} WHERE name = 'boost_crawler_position'")))) + 1;
  $crawl_rate = round((BOOST_TIME - variable_get('boost_crawler_start_time', BOOST_TIME))/$number_done, 2);
  if (boost_crawler_threads_alive() && $number_done > 0 && !variable_get('boost_crawler_stopped', FALSE) && round(($total - $number_done)/$crawl_rate) > 0) {
    $form['crawler']['live'] = array(
      '#type'          => 'fieldset',
      '#title'         => t('Boost crawler - Live info'),
    );
    $form['crawler']['live']['boost_crawler_rate'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Crawl Rate - Seconds Per Node (highly inaccurate)'),
      '#default_value' => $crawl_rate,
      '#disabled'      => TRUE,
    );
    $form['crawler']['live']['boost_crawler_eta'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Crawl ETA - highly inaccurate'),
      '#default_value' => format_interval(($total - $number_done)/$crawl_rate),
      '#disabled'      => TRUE,
    );
    $form['crawler']['live']['boost_stop_crawler'] = array(
      '#type' => 'submit',
      '#value' => t('Stop Crawler. !count URL\'s left.', array('!count' => $total - $number_done)),
      '#submit' => array('boost_stop_crawler_submit'),
    );
  }
  else {
    $form['crawler']['boost_reset_crawler'] = array(
      '#type' => 'submit',
      '#value' => t('Reset Crawler & Cron Semaphore'),
      '#submit' => array('boost_reset_crawler_submit'),
    );
  }


  // Apache .htaccess settings generation
  $htaccess = boost_admin_generate_htaccess(variable_get('boost_server_name_http_host', '%{SERVER_NAME}'), variable_get('boost_document_root', '%{DOCUMENT_ROOT}'));
  $form['htaccess'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Boost Apache .htaccess settings generation'),
    '#description'   => t('<a href="!link">Explanation of .htaccess variables</a> <br><br> <strong>Be sure to save the configuration and then go to the <a href="!rules">htaccess rules generation page</a> and copy the rules.</strong>', array('!link' => url('http://www.askapache.com/htaccess/mod_rewrite-variables-cheatsheet.html'), '!rules' => url('admin/settings/performance/boost-rules'))),
  );
  $form['htaccess']['boost_server_name_http_host'] = array(
    '#type'          => 'radios',
    '#title'         => t('Servers URL or Name'),
    '#default_value' => variable_get('boost_server_name_http_host', '%{SERVER_NAME}'),
    '#options'       => array(
      '%{SERVER_NAME}' => '%{SERVER_NAME}',
      '%{HTTP_HOST}' => '%{HTTP_HOST}',
      $_SERVER['SERVER_NAME'] => $_SERVER['SERVER_NAME'],
      $_SERVER['HTTP_HOST'] => $_SERVER['HTTP_HOST'],
    ),
    '#description'   => t('Best to leave these as %{}, only try the last option(s) if boost is still not working.'),
  );

  // Set DOCUMENT_ROOT
  $drupal_subdir = rtrim($base_path, '/');
  $document_root = str_replace("\\", '/', getcwd()); // fix windows dir slashes
  $document_root = str_replace($drupal_subdir, '', $document_root); // remove subdir
  $options = array('%{DOCUMENT_ROOT}' => '%{DOCUMENT_ROOT}', $document_root => $document_root); // initial options
  $rejects = array('SCRIPT_FILENAME', 'DOCUMENT_ROOT'); // values to ignore
  $output = boost_array_find($document_root, $_SERVER, $rejects); //search for values that match getcwd
  if (!empty($output)) {
    foreach ($output as $key=>$value) {
      $temp = '%{ENV:' . $key . '}';
      $options[$temp] = $temp; // adding values to options
      if ($value == $document_root) {
        $best = $temp; // set best since it's a match
      }
    }
  }
  if ($_SERVER['DOCUMENT_ROOT'] == $document_root) {
    $best = '%{DOCUMENT_ROOT}';
  }
  $description = t("Value of $best is recommended for this server.");
  if (!isset($best)) {
    $best = $document_root;
    $description .= t(' Please open an boost issue on drupal.org, since apache and php are not configured correctly.');
  }
  $form['htaccess']['boost_document_root'] = array(
    '#type'          => 'radios',
    '#title'         => t('Document Root'),
    '#default_value' => variable_get('boost_document_root', '%{DOCUMENT_ROOT}'),
    '#options'       => $options,
    '#description'   => $description,
  );

  // Clear database button
  $form['clear'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clear Boost\'s Database'),
    '#description' => t('Warning: This gives you a clean slate for the boost database, use with caution. If you change the directories or file extensions (in "Boost directories and file extensions"), then clearing the database would be a good idea, but not required.'),
  );
  $form['clear']['boost_reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset boost database: !records records', array('!records' => boost_count_db(2))),
    '#submit' => array('boost_reset_database_submit'),
  );

  $form['#validate'][] = 'boost_admin_boost_performance_page_validate';
  return system_settings_form($form);
}

/**
 * validate system_themes_form form submissions.
 */
function boost_admin_boost_performance_page_validate($form, &$form_state) {
  $form_state['values']['boost_previously'] = variable_get('boost_enabled', '');
  extract($form_state['values'], EXTR_SKIP | EXTR_REFS);
  if (BOOST_MULTISITE_SINGLE_DB) {
    $boost_file_path = boost_cache_directory(NULL, FALSE);
  }

  if (!empty($boost_enabled)) {
    // The cache is enabled
    // Ensure the cache directory exists or can be created:
    _boost_mkdir_p($boost_file_path);
    file_check_directory($boost_file_path, FILE_CREATE_DIRECTORY, 'boost_file_path');
  }

//   if (strpos($boost_file_extension, '.') !== 0) {
//     form_set_error('boost_file_extension', t('Cache file extension %extension must begin with a period.', array('%extension' => $boost_file_extension)));
//   }

  if (!empty($boost_pre_process_function) && !is_callable($boost_pre_process_function)) {
    form_set_error('boost_pre_process_function', t('Pre-process function %function() does not exist.', array('%function' => $boost_pre_process_function)));
  }
}

function boost_admin_site_offline_submit($form, &$form_state) {
  if (!empty($form_state['values']['site_offline'])) {
    if (BOOST_CLEAR_CACHE_OFFLINE) {
      if (boost_cache_clear_all()) {
        drupal_set_message(t('Boost: Static page cache cleared.'), 'warning');
      }
    }
  }
}

function boost_admin_modules_submit($form, &$form_state) {
  if ((variable_get('preprocess_css', FALSE)==TRUE || variable_get('preprocess_js', FALSE)==TRUE)) {
    if (boost_cache_clear_all()) {
      drupal_set_message(t('Boost: Static page cache cleared.'), 'warning');
    }
  }
}

function boost_admin_themes_submit($form, &$form_state) {
  if ((variable_get('preprocess_css', FALSE)==TRUE || variable_get('preprocess_js', FALSE)==TRUE)) {
    if (boost_cache_clear_all()) {
      drupal_set_message(t('Boost: Static page cache cleared.'), 'warning');
    }
  }
}

/**
 * Form builder; Displays Boost's htaccess generation page.
 *
 * @see system_settings_form()
 */
function boost_admin_htaccess_page() {
  $htaccess = boost_admin_generate_htaccess(variable_get('boost_server_name_http_host', '%{SERVER_NAME}'), variable_get('boost_document_root', '%{DOCUMENT_ROOT}'));

  $form['boost_generated'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Generated Rules'),
    '#default_value' => $htaccess,
    '#rows'          => count(explode("\n", $htaccess))+1,
    '#wysiwyg'       => FALSE,
    '#description'   => t("Copy this into your .htaccess file below <pre><tt>  # RewriteBase / </tt></pre> and above <pre><tt>  # Rewrite URLs of the form 'x' to the form 'index.php?q=x'</tt></pre>"),
  );
  return $form;
}

/**
 * Generate htaccess code.
 *
 * http://www.askapache.com/htaccess/mod_rewrite-variables-cheatsheet.html
 * @param $server_name
 *   %{SERVER_NAME} [OR] %{HTTP_HOST} [OR] www.example.com
 * @param $document_root
 *   %{DOCUMENT_ROOT} [OR] getcwd() [OR] path to webroot from ~/
 * @param $cache_dir
 *   cache dir
 * @param $gzip_dir
 *   gz dir
 * @param $html
 *   html document file extension
 */
function boost_admin_generate_htaccess($server_name = '%{SERVER_NAME}', $document_root = '%{DOCUMENT_ROOT}') {
  Global $base_path;
  $drupal_subdir = rtrim($base_path, '/');

  // Various dir's
  $cache_dir = BOOST_ROOT_CACHE_DIR;
  $gzip_dir = !(BOOST_GZIP_DIR == '') ? '/' . BOOST_GZIP_DIR : '';
  $normal_dir = !(BOOST_NORMAL_DIR == '') ? '/' . BOOST_NORMAL_DIR : '';
  $permanent_dir_gzip = !(BOOST_PERM_GZ_DIR == '') ? '/' . BOOST_PERM_GZ_DIR : '';
  $permanent_dir = !(BOOST_PERM_NORMAL_DIR == '') ? '/' . BOOST_PERM_NORMAL_DIR : '';

  // with a \ slash
  $html = '\\' . BOOST_FILE_EXTENSION;
  $xml = '\\' . BOOST_XML_EXTENSION;
  $css = '\\' . BOOST_CSS_EXTENSION;
  $js = '\\' . BOOST_JS_EXTENSION;
  $json = '\\' . BOOST_JSON_EXTENSION;
  $gz = '\\' . BOOST_GZIP_EXTENSION;

  // no slash
  $_html = BOOST_FILE_EXTENSION;
  $_xml = BOOST_XML_EXTENSION;
  $_css = BOOST_CSS_EXTENSION;
  $_js = BOOST_JS_EXTENSION;
  $_json = BOOST_JSON_EXTENSION;
  $_gz = BOOST_GZIP_EXTENSION;

  $char = BOOST_CHAR;
  $permanent_char = BOOST_PERM_CHAR;
  $server_ip = str_replace('.', '\.', $_SERVER['SERVER_ADDR']);


  // Generate the rules
  $string  = "  ### BOOST START ###\n";
  if (!BOOST_DISABLE_CLEAN_URL && (BOOST_CACHE_HTML || BOOST_CACHE_XML || BOOST_CACHE_JSON)) {
    $files = '';
    if (BOOST_CACHE_HTML) {
      $files .= "$html|";
      if (BOOST_GZIP) {
        $files .= "$html$gz|";
      }
    }
    if (BOOST_CACHE_XML) {
      $files .= "$xml|";
      if (BOOST_GZIP) {
        $files .= "$xml$gz|";
      }
    }
    if (BOOST_CACHE_JSON) {
      $files .= "$json|";
      if (BOOST_GZIP) {
        $files .= "$json$gz|";
      }
    }
    $files = trim($files, '|');
    $string .= "  <FilesMatch \"($files)$\">\n";
    $string .= "    <IfModule mod_headers.c>\n";
    $string .= "      Header set Expires \"Sun, 19 Nov 1978 05:00:00 GMT\"\n";
    $string .= "      Header set Cache-Control \"no-store, no-cache, must-revalidate, post-check=0, pre-check=0\"\n";
    $string .= "    </IfModule>\n";
    $string .= "  </FilesMatch>\n";
  }
  if (BOOST_CACHE_HTML || BOOST_CACHE_XML || BOOST_CACHE_CSS || BOOST_CACHE_JS || BOOST_CACHE_JSON) {
    $string .= "  <IfModule mod_mime.c>\n";
    $string .= BOOST_CACHE_HTML ? "    AddCharset utf-8 $_html\n" : '';
    $string .= BOOST_CACHE_XML ?  "    AddCharset utf-8 $_xml\n" : '';
    $string .= BOOST_CACHE_CSS ?  "    AddCharset utf-8 $_css\n" : '';
    $string .= BOOST_CACHE_JS ?   "    AddCharset utf-8 $_js\n" : '';
    $string .= BOOST_CACHE_JSON ? "    AddCharset utf-8 $_json\n" : '';
    $string .= BOOST_GZIP ?       "    AddEncoding gzip $_gz\n" : '';
    $string .= "  </IfModule>\n";
  }
  if (BOOST_CACHE_HTML) {
    $string .= "  <FilesMatch \"$html$gz$\">\n";
    $string .= "    ForceType text/html\n";
    $string .= "  </FilesMatch>\n";
  }
  if (BOOST_CACHE_XML) {
    $string .= "  <FilesMatch \"$xml$gz$\">\n";
    $string .= "    ForceType text/xml\n";
    $string .= "  </FilesMatch>\n";
  }
  if (BOOST_CACHE_CSS) {
    $string .= "  <FilesMatch \"$css$gz$\">\n";
    $string .= "    ForceType text/css\n";
    $string .= "  </FilesMatch>\n";
  }
  if (BOOST_CACHE_JS) {
    $string .= "  <FilesMatch \"$js$gz$\">\n";
    $string .= "    ForceType text/javascript\n";
    $string .= "  </FilesMatch>\n";
  }
  if (BOOST_CACHE_JSON) {
    $string .= "  <FilesMatch \"$json$gz$\">\n";
    $string .= "    ForceType text/javascript\n";
    $string .= "  </FilesMatch>\n";
  }
  if (BOOST_AGGRESSIVE_GZIP) {
    $string .= "\n";
    $string .= "  # Gzip Cookie Test\n";
    $string .= "  RewriteRule boost-gzip-cookie-test\.html  $cache_dir$permanent_dir_gzip/boost-gzip-cookie-test\.html\.gz [L,T=text/html]\n";
  }
  if (BOOST_CACHE_CSS || BOOST_CACHE_JS) {
    if (BOOST_GZIP) {
      $skip = BOOST_CACHE_CSS && BOOST_CACHE_JS ? 2 : 1;
      $string .= "\n";
      $string .= "  # GZIP - Cached css & js files\n";
      $string .= BOOST_AGGRESSIVE_GZIP ? "  RewriteCond %{HTTP_COOKIE} !(boost-gzip) [OR]\n" : '';
      $string .= "  RewriteCond %{HTTP:Accept-encoding} !gzip\n";
      $string .= "  RewriteRule .* - [S=$skip]\n";
      if (BOOST_CACHE_CSS) {
        $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$permanent_dir_gzip/$server_name%{REQUEST_URI}$permanent_char$css$gz -s\n";
        $string .= "  RewriteRule .* $cache_dir$permanent_dir_gzip/$server_name%{REQUEST_URI}$permanent_char$css$gz [L,QSA,T=text/css]\n";
      }
      if (BOOST_CACHE_JS) {
        $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$permanent_dir_gzip/$server_name%{REQUEST_URI}$permanent_char$js$gz -s\n";
        $string .= "  RewriteRule .* $cache_dir$permanent_dir_gzip/$server_name%{REQUEST_URI}$permanent_char$js$gz [L,QSA,T=text/javascript]\n";
      }
    }
    $string .= "\n";
    $string .= "  # NORMAL - Cached css & js files\n";
    if (BOOST_CACHE_CSS) {
      $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$permanent_dir/$server_name%{REQUEST_URI}$permanent_char$css -s\n";
      $string .= "  RewriteRule .* $cache_dir$permanent_dir/$server_name%{REQUEST_URI}$permanent_char$css [L,QSA,T=text/css]\n";
    }
    if (BOOST_CACHE_JS) {
      $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$permanent_dir/$server_name%{REQUEST_URI}$permanent_char$js -s\n";
      $string .= "  RewriteRule .* $cache_dir$permanent_dir/$server_name%{REQUEST_URI}$permanent_char$js [L,QSA,T=text/javascript]\n";
    }
  }
  if (BOOST_CACHE_HTML || BOOST_CACHE_XML || BOOST_CACHE_JSON) {
    $skip = (int)BOOST_CACHE_HTML + (int)BOOST_CACHE_XML + (int)BOOST_CACHE_JSON;
    $skip = BOOST_GZIP ? $skip*2+1 : $skip;
    $string .= "\n";
    $string .= "  # Caching for anonymous users\n";
    $string .= "  # Skip boost IF not get request OR uri has wrong dir OR cookie is set OR request came from this server OR https request\n";
    $string .= "  RewriteCond %{REQUEST_METHOD} !^GET$ [OR]\n";
    $string .= "  RewriteCond %{REQUEST_URI} (^$drupal_subdir(admin|cache|misc|modules|sites|system|themes|node/add))|(/(comment/reply|edit|user|user/(login|password|register))$) [OR]\n";
    $string .= "  RewriteCond %{HTTP_COOKIE} DRUPAL_UID [OR]\n";
    $string .= BOOST_LOOPBACK_BYPASS ? "  RewriteCond %{REMOTE_ADDR} ^$server_ip$ [OR]\n" : '';
    $string .= "  RewriteCond %{HTTPS} on\n";
    $string .= "  RewriteRule .* - [S=$skip]\n";
    if (BOOST_GZIP) {
      $skip = (int)BOOST_CACHE_HTML + (int)BOOST_CACHE_XML + (int)BOOST_CACHE_JSON;
      $string .= "\n";
      $string .= "  # GZIP\n";
      $string .= BOOST_AGGRESSIVE_GZIP ? "  RewriteCond %{HTTP_COOKIE} !(boost-gzip) [OR]\n" : '';
      $string .= "  RewriteCond %{HTTP:Accept-encoding} !gzip\n";
      $string .= "  RewriteRule .* - [S=$skip]\n";
      if (BOOST_CACHE_HTML) {
        $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$gzip_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$html$gz -s\n";
        $string .= "  RewriteRule .* $cache_dir$gzip_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$html$gz [L,T=text/html]\n";
      }
      if (BOOST_CACHE_XML) {
        $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$gzip_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$xml$gz -s\n";
        $string .= "  RewriteRule .* $cache_dir$gzip_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$xml$gz [L,T=text/xml]\n";
      }
      if (BOOST_CACHE_JSON) {
        $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$gzip_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$json$gz -s\n";
        $string .= "  RewriteRule .* $cache_dir$gzip_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$json$gz [L,T=text/javascript]\n";
      }
    }
    $string .= "\n";
    $string .= "  # NORMAL\n";
    if (BOOST_CACHE_HTML) {
      $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$normal_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$html -s\n";
      $string .= "  RewriteRule .* $cache_dir$normal_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$html [L,T=text/html]\n";
    }
    if (BOOST_CACHE_XML) {
      $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$normal_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$xml -s\n";
      $string .= "  RewriteRule .* $cache_dir$normal_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$xml [L,T=text/xml]\n";
    }
    if (BOOST_CACHE_JSON) {
      $string .= "  RewriteCond $document_root$drupal_subdir/$cache_dir$normal_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$json -s\n";
      $string .= "  RewriteRule .* $cache_dir$normal_dir/$server_name%{REQUEST_URI}$char%{QUERY_STRING}$json [L,T=text/javascript]\n";
    }
  }
  $string .= "\n";
  $string .= "  ### BOOST END ###\n";

  return $string;
}

/**
 * Counts the number of pages in the static cache.
 */
function boost_count_db($all = 0) {
  if ($all == 0) {
    return db_result(db_query("SELECT COUNT(*) FROM {boost_cache} WHERE expire <> 0 and base_dir = '%s'", BOOST_FILE_PATH));
  }
  elseif ($all == 1) {
    return db_result(db_query("SELECT COUNT(*) FROM {boost_cache} WHERE expire BETWEEN 1 AND %d AND base_dir ='%s'", BOOST_TIME, BOOST_FILE_PATH));
  }
  elseif ($all == 2) {
    return db_result(db_query("SELECT COUNT(*) FROM {boost_cache}"));
  }
}

/**
 * Counts the number of pages in the core cache.
 */
function boost_count_core_db($all = FALSE) {
  return db_result(db_query("SELECT COUNT(*) FROM {cache_page}"));
}

/**
 * Flushes boost page cache
 */
function boost_clear_cache_submit () {
  if (boost_cache_clear_all()) {
    drupal_set_message(t('Boost: Static page cache cleared.'));
  }
  else {
    drupal_set_message(t('Boost: Set "Ignore cache flushing:" to \'Disabled\' in the <a href="!link">boost advanced settings</a> & try again.', array('!link' => url('admin/settings/performance/boost', array('fragment' => 'edit-boost-ignore-flush-0-wrapper')))), 'warning');
  }
}


/**
 * Flushes all expired pages from database
 */
function boost_clear_expired_cache_submit () {
  if (boost_cache_db_expire()) {
    drupal_set_message(t('Boost: Expired stale files from static page cache.'));
  }
  else {
   drupal_set_message(t('Boost: Set "Ignore cache flushing:" to \'Disabled\' OR \'Only Ignore Complete Flushes\' in the <a href="@link">boost advanced settings</a> & try again.', array('@link' => url('admin/settings/performance/boost', array('fragment' => 'edit-boost-ignore-flush-0-wrapper')))), 'warning');
  }
}

/**
 * Flushes core page cache
 */
function boost_clear_core_page_cache_submit () {
  cache_clear_all('*', 'cache_page', TRUE);
  drupal_set_message(t('Core page cache cleared.'));
}

/**
 * Resets boost database & cache
 */
function boost_reset_database_submit () {
  if (boost_cache_clear_all()) {
    db_query("TRUNCATE {boost_cache}");
    db_query("TRUNCATE {boost_cache_settings}");
    drupal_set_message(t('Boost: Static page cache & database cleared.'));
  }
  else {
    drupal_set_message(t('Boost: Set "Ignore cache flushing:" to \'Disabled\' in the <a href="!link">boost advanced settings</a> & try again.', array('!link' => url('admin/settings/performance/boost', array('fragment' => 'edit-boost-ignore-flush-0-wrapper')))), 'warning');
  }
}

/**
 * Stop Crawler
 */
function boost_stop_crawler_submit() {
  variable_set('boost_crawler_stopped', TRUE);
  variable_set('boost_crawler_number_of_threads', 0);
  drupal_set_message(t('Boost: Stop crawler signal given; all crawler threads should be stopped in less then %time', array('%time' => format_interval(BOOST_MAX_THREAD_TIME))));
}

/**
 * Reset Crawler
 */
function boost_reset_crawler_submit() {
  db_query("DELETE FROM {variable} WHERE name LIKE '%s%%'", 'boost_crawl');
  db_query("DELETE FROM {variable} WHERE name = 'cron_semaphore'");
  cache_clear_all('variables', 'cache');
  variable_set('boost_crawler_key', md5(mt_rand()));
  drupal_set_message(t('Boost: Crawler variables reset & cron semaphore cleared (cron reset)'));
}

/**
 * Returns all key/values in array that are equal.
 *
 * @param $needle
 *  What your searching for
 * @param $haystack
 *  Array of values
 * @param $a_not
 *  Optional array of key names to exclude
 */
function boost_array_find($needle, $haystack, $a_not = array()) {
  $out = array();
  foreach($haystack as $key=>$value) {
    if ($value == $needle) {
      $good = TRUE;
      foreach($a_not as $not) {
        if (strpos($key, $not) !== FALSE) {
          $good = FALSE;
        }
      }
      if ($good) {
        $out[$key] = $value;
      }
    }
  }
  return $out;
}

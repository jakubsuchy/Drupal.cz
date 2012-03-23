<?php
/**
 * Copy this file to your drupal installation root and run from a web browser
 *
 * BACK UP YOUR IMAGES FIRST!
 */


include_once 'includes/bootstrap.inc';
include_once 'includes/common.inc';

$fields = array('thumb_path' => 'thumbnail',
                'preview_path' => 'preview',
                'image_path' => '_original');

drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

if (function_exists('_image_insert')) {
  $result = db_query("SELECT * FROM {image}");
  while ($old_image = db_fetch_object($result)) {
    foreach ($fields as $old => $new) {
      $old_file = '';
      if (file_exists($old_image->$old)) {
        $old_file = $old_image->$old;
      } else {
        $old_file = file_create_path($old_image->$old);   
      }
      if ($old_file && $old_image->$old !='' &&
          db_num_rows(db_query("SELECT fid FROM {files} WHERE nid=%d and filename='%s'", $old_image->nid, $new)) == 0) {
        _image_insert($old_image, $new, $old_file);
      }
    }
  }
}

?>

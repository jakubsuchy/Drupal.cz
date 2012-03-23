<?php
// $Id: image.imagemagick.inc,v 1.2 2005/09/04 22:49:26 tma0 Exp $

/**
 * Return information about the imagemagick toolkit
 */
function image_imagemagick_info() {
  return array('name' => 'imagemagick', 'title' => 'ImageMagick Toolkit.');
  
}

/**
 * Validate and return toolkit specific settings
 */
function image_imagemagick_settings() {
  $convert_file = variable_get('image_imagemagick_convert', '/usr/bin/convert');

  if (!file_exists($convert_file)) {
    form_set_error('image_imagemagick_convert', t('%file does not exist or is not executable.', array('%file' => "<em>$convert_file</em>")));
  }
  
  return form_textfield(t('Location of the "convert" binary'), 'image_imagemagick_convert', $convert_file, 64, 64);
}

/**
 * Resize an image to the given width and height
 */
function image_imagemagick_resize($source, $dest, $width, $height) {
  $filter = ' -scale '. $width . 'x' . $height . ' -filter QUADRATIC';
  return _image_imagemagick_convert($source, $dest, $filter);
}

/**
 * Rotate an image
 */
function image_imagemagick_rotate($source, $dest, $degrees) {
  $filter = ' -rotate ' . escapeshellarg($degrees) . ' -background #000000';  
  return _image_imagemagick_convert($source, $dest, $filter);  
}

/**
 * Crop an image to the specified dimensions
 */
function image_imagemagick_crop($source, $dest, $x, $y, $width, $height) {
  $filter = ' -crop ' . $width . 'x' . $height . '+' . $x . '+' . $y;
  return _image_imagemagick_convert($source, $dest, $filter);  
}

/**
 * Helper function: Escape filename for Unix or Windows shell.
 *
 * @param $filename filename to escape -- may contain spaces or other problematic characters
 *
 * @return escaped version of $filename which should pass through shell
 */
function _image_escape_shell($filename) {
  if (strstr($_SERVER['SERVER_SOFTWARE'], 'Win32') || strstr($_SERVER['SERVER_SOFTWARE'], 'IIS')) {
    return '"' . addslashes($filename) . '"';
  } else {
    return escapeshellarg($filename);
  }
}

/**
 * Calls the convert executable with the specified filter
 */
function _image_imagemagick_convert($source, $dest, $filter) {
  $convert_path = variable_get('image_imagemagick_convert', '/usr/bin/convert');
  if (!file_exists($convert_path)) {
    return false;
  }
  
  $filter = preg_replace("/[^A-Za-z0-9\.\-\+\040]/", '', $filter);
  $source = _image_escape_shell($source);
  $dest = _image_escape_shell($dest);
  $err = _image_exec("$convert_path $filter $source $dest");
  if ($err) {
    return false;
  }
  if (!file_exists(trim($dest, "'"))) {
    return false;
  }
  
  return true;
}

function _image_exec($cmd) {
  if (substr(php_uname(), 0, 7) == "Windows"){
    if ($h = popen("start \"bla\" $cmd", "r")) {
      pclose($h);
      return true;
    } else {
      return false;
    }
  } else {
    return exec($cmd);    
  }
} 

?>

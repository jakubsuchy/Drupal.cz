<?php

/**
 * @file
 * Generates CSS sprites for editor buttons.
 */

/**
 * Create and return editor sprite depending on editor settings or buttons.
 */
function bueditor_sprite($editor, $buttons = array()) {
  if (!empty($buttons)) {
    uasort($buttons, create_function('$b1,$b2', 'return $b1->weight - $b2->weight;'));
    return bueditor_sprite_buttons($buttons, $editor->iconpath);
  }
  elseif (isset($editor->eid) && $editor->eid > 0) {
    return bueditor_sprite_editor($editor);
  }
  return '';
}

/**
 * Create an icon sprite for an editor.
 */
function bueditor_sprite_editor($editor) {
  return bueditor_sprite_buttons(bueditor_buttons($editor->eid), $editor->iconpath);
}

/**
 * Create an icon sprite for a set of buttons.
 */
function bueditor_sprite_buttons($buttons, $iconpath) {
  $icons = array();
  $iconpath = bueditor_path_tr($iconpath);
  foreach ($buttons as $button) {
    if (substr($button->title, 0, 4) != 'tpl:' && preg_match('/\.(png|gif|jpg)$/', $button->icon)) {
      $icons[] = $iconpath . '/' . $button->icon;
    }
  }
  return bueditor_sprite_icons($icons);
}

/**
 * Create an icon sprite for a set of icons.
 */
function bueditor_sprite_icons($icons) {
  if (image_get_toolkit() != 'gd' || empty($icons) || ($total = count($icons)) < 2) {
    return '';
  }
  //Image properties of the first one in the list will be the reference for the sprite
  $first_icon = $icons[0];
  $sprite_dir = bueditor_sprites_dir();
  $sprite_name = 'sprite_' . md5(implode(' ', $icons)) . substr($first_icon, strrpos($first_icon, '.'));
  $sprite_path = $sprite_dir . '/' . $sprite_name;
  if (file_exists($sprite_path)) {
    return $sprite_name;
  }
  if (!file_check_directory($sprite_dir, 1) || !($first_info = image_get_info($first_icon)) || !($first_im = image_gd_open($first_icon, $first_info['extension']))) {
    return '';
  }
  //set fixed icon dimensions
  $icon_width = $first_info['width'];
  $icon_height = $first_info['height'];
  //set sprite dimensions based on the number of icons
  $sprite_width = $icon_width * $total;
  $sprite_height = $icon_height;
  $sprite_ext = $first_info['extension'];
  $sprite = imagecreatetruecolor($sprite_width, $sprite_height);
  //handle png transperancy
  if ($sprite_ext == 'png') {
    imagealphablending($sprite, FALSE);
    $transparency = imagecolorallocatealpha($sprite, 0, 0, 0, 127);
    imagefill($sprite, 0, 0, $transparency);
    imagesavealpha($sprite, TRUE);
  }
  //handle gif transperancy
  elseif ($sprite_ext == 'gif' && ($transparency_index = imagecolortransparent($first_im)) >= 0) {
    $transparent_color = imagecolorsforindex($first_im, $transparency_index);
    $transparency_index = imagecolorallocate($sprite, $transparent_color['red'], $transparent_color['green'], $transparent_color['blue']);
    imagefill($sprite, 0, 0, $transparency_index);
    imagecolortransparent($sprite, $transparency_index);
    imagetruecolortopalette($sprite, TRUE, 256);
  }
  //copy first icon
  imagecopy($sprite, $first_im, 0, 0, 0, 0, $icon_width, $icon_height);
  imagedestroy($first_im);
  //copy the rest
  for ($i = 1; $i < $total; $i++) {
    if (($info = image_get_info($icons[$i])) && ($im = image_gd_open($icons[$i], $info['extension']))) {
      imagecopy($sprite, $im, $i * $icon_width, 0, 0, 0, $icon_width, $icon_height);
      imagedestroy($im);
    }
  }
  //save sprite to file
  $result = image_gd_close($sprite, $sprite_path, $sprite_ext);
  imagedestroy($sprite);
  return $result ? $sprite_name : '';
}
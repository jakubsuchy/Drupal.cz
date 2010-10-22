<div class="location vcard"><div class="adr">
<?php echo $name; ?>
<?php if ($street) {?>
<div class="street-address"><?php
  echo $street;
  if ($additional) {
    echo ' '. $additional;
  }
?></div>
<?php }?>
<?php
  if ($city || $province || $postal_code) {
    $city_province_postal = array();

    if ($city) {
      $city_province_postal[] = '<span class="locality">'. $city .'</span>';
    }
    if ($province) {
      $city_province_postal[] = '<span class="region">'. $province .'</span>';
    }
    if ($postal_code) {
      $city_province_postal[] = '<span class="postal-code">'. $postal_code .'</span>';
    }

    echo implode(', ', $city_province_postal);
  }
?>
<?php if ($country_name) { ?>
<div class="country-name"><?php echo $country_name; ?></div>
<?php } ?>
<?php
  // "Geo" microformat, see http://microformats.org/wiki/geo
  if ($latitude && $longitude) {
    // Assume that 0, 0 is invalid.
    if ($latitude != 0 || $longitude != 0) {
?>
<span class="geo"><abbr class="latitude" title="<?php echo $latitude; ?>"><?php echo $latitude_dms; ?></abbr>, <abbr class="longitude" title="<?php echo $longitude; ?>"><?php echo $longitude_dms; ?></abbr></span>
<?php
    }
  }
?>
</div></div>
<?php echo $map_link; ?>

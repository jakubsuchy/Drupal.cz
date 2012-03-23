<?php if (count($locations)) {?>
<h3><?php echo count($locations) > 1 ? t('Locations') : t('Location');?></h3>
<?php
  foreach ($locations as $location) {
    echo $location;
  }
}

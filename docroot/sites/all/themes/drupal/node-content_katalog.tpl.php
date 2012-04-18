<div class="article <?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">
  <?php if ($submitted) { ?><span class="submitted"><?php print $submitted?></span><?php } ?>
  <div class="pcontent">
    <?php print $content?>
    <h3>Tagy</h3>
    <?php
      _pathauto_include();
      foreach ($taxonomy as $term) {
        echo l($term['title'], "katalog/tagy/".pathauto_cleanstring($term['title']))."\n";
      }
    ?>
  </div>
  <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
</div> <!-- article end -->
<div class="article-spacer">&nbsp;</div>

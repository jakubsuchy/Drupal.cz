<div class="article <?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">
  <?php if ($submitted) { ?><span class="submitted"><?php print $submitted?></span><?php } ?>
  <div class="pcontent">
    <div class="catalog clear-block">
      <div class="katalogtext">
        <h3 class="field">O webu</h3>
        <div class="field field-type-text field-field-popis-webu">
          <div class="field-items">
            <?php foreach ((array)$field_popis_webu as $item) : ?>
              <div class="field-item"><?php print $item['view'] ?></div>
            <?php endforeach; ?>
          </div>
        </div>
        <div class="field field-type-link field-field-url-0">
          <div class="field-items">
            <?php foreach ((array)$field_url_0 as $item) { ?>
              <div class="field-item"><?php print $item['view'] ?></div>
            <?php } ?>
          </div>
        </div>
      </div> <!--katalog text end -->
  
      <div class="field field-type-image field-field-screenshoty">
        <h3 class="field-label">Screenshot</h3>
        <div class="field-items">
          <?php foreach ((array)$field_screenshoty as $item) { ?>
            <div class="field-item"><a class="thickbox" href="<?php print url($item['filepath']);?>"><?php print theme('imagecache', 'katalogwebu', $item['filepath']) ?></a></div>
          <?php } ?>
        </div>
      </div>
    </div>

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

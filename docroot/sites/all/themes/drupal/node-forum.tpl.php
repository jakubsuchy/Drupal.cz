<div class="article <?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?> <?php echo "article-" . $node->nid;?>">
  <? if ($page == 0) { ?>
    <h2 class="title"><a href="<?php print $node_url?>"><?php print $title?></a></h2>
  <? } ?>
  <?php if ($submitted) { ?><span class="submitted"><?php print $submitted?></span><?php } ?>

  <div class="pcontent">
    <div class="forum-kat">
      <b>Kategorie:</b>
      <?php
        foreach ($node->taxonomy as $tax) {
         if ($tax->vid == 1) {
          print $tax->name;
         }
        }
      ?>
    </div>
    <div class="forum-ver">
      <b>Týká se verze:</b>
      <?php 
        foreach ($node->taxonomy as $tax) {
         if ($tax->vid == 11) {
          print $tax->name;
         }
        }
      ?>
    </div>
    <?php print $content?>
  </div>
  
  <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
</div> <!-- article end -->
<div class="article-spacer">&nbsp;</div>

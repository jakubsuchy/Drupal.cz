  <div class="article <?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?> <?php echo "article-" . $node->nid;?>">
    <? if ($page == 0) { ?>
      <h2 class="title"><a href="<?php print $node_url?>"><?php print $title?></a></h2>
        <? } ?>
<?php if ($submitted) { ?><span class="submitted"><?php print $submitted?></span><?php } ?>

    <div class="pcontent"><?php print $content?></div>
    <div style="clear: both">&nbsp;</div>
    <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
  </div> <!-- article end -->
  <div class="article-spacer">&nbsp;</div>
<?php
if ($type != 'forum' && $page != 0 && $node->nid != 523 && $node->nid != 620 && $type != 'content_zznam_v_katalogu_web' && $node->nid != 263 && $node->nid != 1611 && $type != 'poll' && $node->nid != '2381' && $node->nid != '2403' && $node->nid != 2951) {
/*  echo '<script type="text/javascript">
    //<![CDATA[
    document.write(\'<\'+\'iframe width="468" height="60" frameborder="0" style="width:468;height:60;border:none" scrolling="no" src="http://ad.adfox.cz/utf/ppcbe?js=0&amp;charset=utf&amp;druhy=yes&amp;format=0499d9ffffffff6600394c54016ca858&amp;partner=1652&amp;stranka=\'+location.href+\'"><\'+\'/iframe>\');
  //]]>
  </script>
            ';*/
}
?>

<?php
if ($page != 0 && $node->nid != 523 && $node->nid != 620 && $type != 'content_zznam_v_katalogu_web') {
/*$varianty = array('<script type="text/javascript"><!--
    google_ad_client = "pub-3025303680154136";
  google_ad_width = 468;
  google_ad_height = 60;
  google_ad_format = "468x60_as";
  google_ad_type = "text_image";
  //2007-08-15: drupal - ve foru klasika
  google_ad_channel = "5959844926";
  //-->
  </script>
    <script type="text/javascript"
      src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
      </script>', '<script type="text/javascript"><!--
      google_ad_client = "pub-3025303680154136";
      google_ad_width = 468;
      google_ad_height = 60;
      google_ad_format = "468x60_as";
      google_ad_type = "text_image";
      //2007-08-15: drupal - ve foru klasika, drupal - ve foru kontrast
      google_ad_channel = "5959844926+8672929801";
      google_color_border = "FFFFFF";
      google_color_bg = "ff6600";
      google_color_link = "ffffff";
      google_color_text = "ffffff";
      google_color_url = "008000";
      google_ui_features = "rc:6";
      //-->
      </script>
        <script type="text/javascript"
          src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
          </script>');
*/
$ran = rand(0, 1);
echo $varianty[$ran];

}
?>

<?php
/*if ($page != 0) {
?>
  <div class="adfox">
  <p>Reklama (<a href="/kontakt">chcete ji?</a>)</p>
  <div class="adfox-frame">
<!--   <div class="adfox-content"><?php print banner_display(48, 1) ?></div>
   <div class="adfox-content"><?php print banner_display(49, 1) ?></div>-->
    <script type="text/javascript"><!--
    google_ad_client = "pub-3025303680154136";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as";
google_ad_type = "text_image";
//2006-12-13: drupal.cz
google_ad_channel = "5727758341";
google_color_border = "FFFFFF";
google_color_bg = "FFFFFF";
google_color_link = "000033";
google_color_text = "000000";
google_color_url = "ff6600";
//--></script>
<script type="text/javascript"
    src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
    </script>
  </div>
  </div><!-- adfox end -->
  <div class="article-spacer adfox-after">&nbsp;</div>
  <?php
}*/
?>

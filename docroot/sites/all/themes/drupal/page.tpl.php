<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="cs" lang="cs">
<head>
 <?php print $head ?>
 <title><?php print $head_title; ?></title>
 <?php print $styles ?>
 <?php print $scripts; ?>
 <script type="text/javascript"><?php /* Needed to avoid Flash of Unstyle Content in IE */ ?> </script>
</head>
<body>
<div id="container">
<p id="goto"><a href="#main-text">Přeskočit přímo na text</a></p>
<div id="header">
 <h1><a href="/">Drupal.cz<span></span></a></h1>
  <div id="mainnav">
   <h4 class="hide">Hlavní navigace</h4>
  <?php
  if (is_array($primary_links)) {
	  echo "<ul>\n";
	  foreach ($primary_links as $lnk) {
		  print "<li><a href=\"".url($lnk['href'])."\"><span>".$lnk['title']."</span></a></li>\n";

	  }
	  echo "</ul>\n";
  }
?>
  </div> <!-- mainnav end -->
</div> <!-- header end -->

<div id="main">
  <hr class="block-separator" />
 <div class="text" id="main-text">
  <div class="maintext">
   <?php if ($mission) {?>
   <div class="mission-article">
    <span>&nbsp;</span>
    <div class="mission-body">
     <p>
      <?php print $mission; ?>
     </p>
     <p class="mission-links">
      <br />
      <a class="more-info-mission" href="/o-systemu-drupal">Více o systému Drupal</a>
      <a class="more-info-mission" href="/forum">Poradna pro začátečniky</a>
     </p>
<!--      <a class="more-info-mission" href="/o-serveru-drupal.cz">Více o tomto serveru</a>-->

     <?php $sea = google_cse_block('view', 0); print $sea['content']; ?>

    </div> <!-- mission-body end -->
    <div class="mission-footer">
     <p>&nbsp;</p>
    </div> <!-- mission-footer end -->
   </div> <!-- mission-article end -->
   <?php } // mission end ?>

  <div class="articles">
<!--   <hr />
   <h2>Články</h2>-->
  </div> <!-- articles header end -->
        <?php print $breadcrumb ?>
        <h2 class="title"><?php print $title ?></h2>
        <div class="tabs"><?php print $tabs ?></div>
  <?php print theme_help(); ?>
        <?php print $messages ?>
        <?php print $content; ?>
        <?php print $obsahdole; ?>
  </div> <!-- maintext end -->
 </div> <!-- text end -->

<div class="drupal-block">
  <?php if ($vpravo) { print $vpravo; } ?>
</div> <!-- drupal-block end -->

<div class="cleaner">&nbsp;</div>
</div> <!-- main end -->
</div> <!-- container end -->
<div id="footer">
 <?php print $footer_message ?>

 <div class="preloader">
  <img src="/<?php print $directory?>/images/nav-right.png" width="1" height="1" alt="." />
  <img src="/<?php print $directory?>/images/nav-left.png" width="1" height="1" alt="." />
  <img src="/<?php print $directory?>/images/nav-left-active.png" width="1" height="1" alt="." />
  <img src="/<?php print $directory?>/images/nav-right-active.png" width="1" height="1" alt="." />
 </div>
</div> <!-- footer end -->
<?php print $closure ?>
</body>
</html>

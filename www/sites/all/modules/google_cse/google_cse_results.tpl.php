<?php
// $Id: google_cse_results.tpl.php,v 1.1.2.2 2008/07/01 19:09:09 mfb Exp $
?>

<?php if ($prefix): ?>
  <div class="google-cse-results-prefix"><?php print $prefix; ?></div>
<?php endif; ?>

<?php print $results_searchbox_form; ?>

<div id="google-cse-results">
  <noscript>
    <?php print $noscript; ?>
  </noscript>
</div>

<script type="text/javascript">
  //<![CDATA[
    var googleSearchIframeName = 'google-cse-results';
    var googleSearchFormName = 'google-cse-results-searchbox-form';
    var googleSearchFrameWidth = Drupal.settings.googleCSE.resultsWidth;
    var googleSearchFrameborder = 0;
    var googleSearchDomain = Drupal.settings.googleCSE.domain;
    var googleSearchPath = '/cse';
  //]]>
</script>

<script type="text/javascript" src="http://www.google.com/afsonline/show_afs_search.js"></script>

<?php if ($suffix): ?>
  <div class="google-cse-results-suffix"><?php print $suffix; ?></div>
<?php endif; ?>


<!-- start node -->
<div id="node-<?php print $node->nid; ?>" class="node storylink clear-block<?php print ($sticky) ? ' sticky' : ''; print (!$status) ? ' node-unpublished' : ''; ?>">
<?php print $picture ?>
<h2 class="title"><a href="<?php print $node_url ?>"><?php print $title ?></a></h2>
<?php print $vote_storylink_via ?>
<?php print $vote_up_down_widget ?>
<div class="content"><?php print $content ?></div>
<div class="submitted"><?php print $submitted ?>
<?php if ($terms): ?>
<span class="terms"> | <?php print t('Tags') ?>: <?php print $terms ?></span>
<?php endif; ?>
</div>
<?php if ($links): ?>
<div class="links">&raquo; <?php print $links ?></div>
<?php endif; ?>
<br class="clear" />
</div>

<?php
$found = 0;
$social = 0;
$i = 0;
$output = '';
foreach($links as $key => $lnk) {
  $next = next($links);
  prev($links);
	if (preg_match("/Číst/", $lnk['title'])) {
    if ($social) {
			$output .= "</div> <!-- social-links end -->\n</div> <!-- social-add end -->\n";
      $social = 0;
    }
		$output .= "\n    <span><a class=\"".$key."\" href=\"".url($lnk['href'])."\">".$lnk['title']."</a></span>\n";
	} else {
		if (!$found) {
			$output .= "<div class=\"morelinks\">\n";
			$found = 1;
		}
		if (preg_match("/cs_social/", $key) && !$socialprinted) {
			$social = 1;
			$socialprinted = 1;
			$output .= "<div id=\"social-add-$seqid\" class=\"social-add digglink\" onmouseover=\"showSocials($seqid)\" onmouseout=\"hideSocials($seqid)\"><a class=\"social-href-link\" href=\"#\">Linkni to!</a>\n";
			$output .= "<div class=\"social-links\" id=\"social-links-$seqid\">\n";
		}
		if (!preg_match("/cs_social/i", $key) && $social) {
			$output .= "</div> <!-- social-links end -->\n</div> <!-- social-add end -->\n";
			$social = 0;
		}
		$lnk['attributes']['class'] = $key;
		$output .= "\n". l($lnk['title'], $lnk['href'], $lnk) ."\n";
	}
  $i++;
}
if ($social) {
	print "</div> <!-- social-links end -->\n</div> <!-- social-add end -->\n";
}
$output .= "</div> <!-- morelinks end -->\n";
echo "$output";
?> 

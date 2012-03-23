<?php
// $Id: gmap_polyutil.inc,v 1.3 2008/07/15 18:00:25 bdragon Exp $

/**
 * @file
 * Encoded polyline utilities.
 */

/**
 * References:
 * [1] http://code.google.com/apis/maps/documentation/polylinealgorithm.html
 * [2] http://facstaff.unca.edu/mcmcclur/GoogleMaps/EncodePolyline/
 * [3] http://mathworld.wolfram.com/
 */

/**
 * Encode a numeric value (steps 3-11 of "Encoding Latitudes and Longitudes" @ [1])
 */
function gmap_polyutil_e5_to_encoded($e5) {
  // As described in http://code.google.com/apis/maps/documentation/polylinealgorithm.html
  $work = $e5; // Make a copy.
  $work <<= 1; // 4) Shift left (Note: I have no clue what happens if PHP_INT_SIZE != 4.)
  if ($e5 < 0) {
    $work = ~$work; // 5) Invert if negative.
  }
  $out = '';
  // While we are NOT on the last chunk...
  while ($work >= 32) {
    // This combines the rest of the steps together.
    $out .= chr((32 | ($work & 31)) + 63);
    $work >>= 5;
  }
  // Last chunk doesn't get ORed with 32.
  $out .= chr(($work & 31) + 63);
  return $out;
}

function gmap_polyutil_dist($p1, $p2) {
  // Distance in two dimensions.
  // √((x1-x0)^2 + (y1-y0)^2)
  return sqrt(pow($p2[0] - $p1[0], 2) + pow($p2[1] - $p1[1], 2));
}

/**
 * Distance calculation between a point and a line segment.
 * @param $p
 *   Point to measure.
 * @param $lp1
 *   Point 1 on line.
 * @param $lp2
 *   Point 2 on line.
 */
function gmap_polyutil_distance($p, $lp1, $lp2) {
  if ($lp1[0] == $lp2[0] && $lp1[1] == $lp2[1]) {
    // The "line" is really being a point. Just use simple distance.
    return gmap_polyutil_dist($p, $lp1);
  }
  // mathematica code: (q-p1).(p2-p1)/(p2-p1).(p2-p1);
  // I asked maxima, and it said ((p2y-p1y)*(qy-p1y)+(p2x-p1x)*(qx-p1x))/((p2y-p1y)^2+(p2x-p1x)^2).
  $tmp = (($lp2[1]-$lp1[1])*($p[1]-$lp1[1])+($lp2[0]-$lp1[0])*($p[0]-$lp1[0]))/(pow($lp2[1]-$lp1[1], 2) + pow($lp2[0]-$lp1[0], 2));
  // Point is not alongside segment, is further off in $lp1's direction.
  if ($tmp <= 0) {
    return gmap_polyutil_dist($lp1, $p);
  }
  // Point is not alongside segment, is further off in $lp2's direction.
  else if ($tmp >= 1) {
    return gmap_polyutil_dist($lp2, $p);
  }
  else {
    // mathematica code: Norm[q-(p1+u (p2-p1))]
    // which means q and (p1+u (p2-p1)) are the things we are calculating distance on.
    // maxima sez: (p1+u*(p2-p1)) => [(p2x-p1x)*u+p1x,(p2y-p1y)*u+p1y]
    return gmap_polyutil_dist($p, array(($lp2[0]-$lp1[0])*$tmp + $lp1[0], ($lp2[1]-$lp1[1])*$tmp + $lp1[1]));
  }
}

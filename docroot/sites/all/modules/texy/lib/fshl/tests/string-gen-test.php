<html>
<body>
<pre>
<?php

	// strings test	
	include('../fshl-helper.php');

	function string_test($str, $hr) 
	{
		$out = fshlHelper::getStringSource($str, $hr);
		@eval('$cmp='.$out.';');
		if(isset($cmp) && $cmp === $str) {
			return $out."\n";
		}
		return $out." -  <b>FAILED</b> correct form is ".var_export($str,true)."\n";
	}
	
	$test_data = array(
	
		"hello world !",
		"",
		"must be double quoted\n",
		"\n\t\r",
		chr(3).chr(4).chr(0)."ri",
		"\x03\x04\x00ri",
		chr(0),
		"\x00",
		"\1",
		
		// back slashes
		
		"\\",
		"\\\\",
		"\"",
		'\"',
		'\\',
		'\\\'',
		'\\\'\\\'\'',
		'\\\'\\\'\0',
		'\'',
		'\0',
		"\61",
		'\x3',
	);
		
	for($hr=0; $hr <= 1; $hr++) {
		if(!$hr) {
			echo "Output in simple quotes\n\n";
		} else {
			echo "\n\nOutput in human readable format\n\n";
		}
		
		foreach($test_data as $foo=>$string) {
			echo string_test($string, $hr);
		}
	}
	
?>
</pre>
</body>
</html>
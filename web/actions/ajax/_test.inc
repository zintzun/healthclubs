<?php

	/*
	* Main input error checking
	*/

	$error = 'OK';

	if ( $error == 'OK' )
	{
		$error = "Hello World!";
	}
	
	print json_encode(array('error' => $error));

?>
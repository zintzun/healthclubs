<?php

	if ( USE_GZHANDLER ) { ob_start("ob_gzhandler"); }

	// See if the ajax sub action exists
	if ( ! isset($TARGET->arg2) || ! file_exists("actions/ajax/_{$TARGET->arg2}.inc") )
	{
		print json_encode(array('error' => "We don't understand that request"));
	}

	// Run the jax sub action
	else
	{
		include "actions/ajax/_{$TARGET->arg2}.inc";
	}

	exit;

?>
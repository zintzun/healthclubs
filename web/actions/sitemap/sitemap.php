<?php

	include "actions/sitemap/_www_sitemap.proc";	
	exit;

	$LT = '<';
	if ( 0 && IS_LOCALHOST )
	{
		print "<pre>";
		$LT = '&lt;';
	}

	if ( ! $TARGET->sub_domain ||  $TARGET->sub_domain == 'www' )
	{
		include "actions/sitemap/_www_sitemap.proc";	
	}
	else
	{
		include "actions/sitemap/_domain_sitemap.proc";	
	}

	if ( IS_LOCALHOST )
	{
		print "</pre>";	
	}

exit;

?>

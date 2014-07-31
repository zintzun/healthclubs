<?php

	// "always", "hourly", "daily", "weekly", "monthly", "yearly", or "never"

print <<<END
{$LT}?xml version="1.0" encoding="UTF-8"?>
{$LT}urlset xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.sitemaps.org/schemas/sitemap/0.9 http://www.sitemaps.org/schemas/sitemap/0.9/sitemap.xsd" xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">\n
END;

	$date = date('Y-m-d',time());

	/*
	* This gets the main nav bar
	*/

	foreach ( $GLOBAL_NAV as $nav_option )
	{

		if ( $nav_option['target'] == '/tutorials' )
		{
			$nav_option['target'] = "/{$TARGET->site->menu_text}/{$TARGET->sub_domain}";
		}
		else if ( $nav_option['target'] != '/' )
		{
			$nav_option['target'] .= "/{$TARGET->sub_domain}";
		}

		$link = "http://{$TARGET->sub_domain}.$APP_DOMAIN{$nav_option['target']}";

print <<<END
   {$LT}url>
      {$LT}loc>$link{$LT}/loc>
      {$LT}lastmod>$date{$LT}/lastmod>
      {$LT}changefreq>daily{$LT}/changefreq>
   {$LT}/url>\n
END;

	}


	/*
	* This gets the experts homepage
	*/

	//if ( $pages = $db->get_results("SELECT * FROM experts WHERE subdomain = '".$db->escape($TARGET->sub_domain)."' ORDER BY num_videos DESC") )
	if ( $pages )
	{

		foreach($pages as $page)
		{

			$link = "http://{$TARGET->sub_domain}.$APP_DOMAIN/videos/{$page->youtube_uid}/{$TARGET->sub_domain}";

print <<<END
   {$LT}url>
      {$LT}loc>$link{$LT}/loc>
      {$LT}lastmod>$date{$LT}/lastmod>
      {$LT}changefreq>daily{$LT}/changefreq>
   {$LT}/url>\n
END;

		}	
	}


	/*
	* This gets the series pages homepage
	*/

	if ( $pages = $db->get_results("SELECT * FROM videos WHERE is_featured = 1 AND subdomain = '".$db->escape($TARGET->sub_domain)."' ORDER BY row_id DESC") )
	{

		foreach($pages as $page)
		{

			$link = "http://{$TARGET->sub_domain}.$APP_DOMAIN/video-series/{$page->row_id}/".format_link_subject($page->title);

print <<<END
   {$LT}url>
      {$LT}loc>$link{$LT}/loc>
      {$LT}lastmod>$date{$LT}/lastmod>
      {$LT}changefreq>daily{$LT}/changefreq>
   {$LT}/url>\n
END;

		}	
	}



	/*
	* This gets the individual video pages
	*/

	if ( $pages = $db->get_results("SELECT * FROM videos WHERE subdomain = '".$db->escape($TARGET->sub_domain)."' ORDER BY row_id DESC") )
	{

		foreach($pages as $page)
		{

			$link = "http://{$TARGET->sub_domain}.$APP_DOMAIN/video/{$page->row_id}/".format_link_subject($page->title);

print <<<END
   {$LT}url>
      {$LT}loc>$link{$LT}/loc>
      {$LT}lastmod>$date{$LT}/lastmod>
      {$LT}changefreq>daily{$LT}/changefreq>
   {$LT}/url>\n
END;

		}	
	}

	print "{$LT}/urlset>";

	exit;

?>

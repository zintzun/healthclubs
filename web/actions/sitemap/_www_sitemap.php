<?php
include "lib/state_utils.inc";

/*
print <<<END
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.google.com/schemas/sitemap/0.84" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.google.com/schemas/sitemap/0.84 http://www.google.com/schemas/sitemap/0.84/sitemap.xsd">
   <url>
      <loc>http://www.healthclubnet.com/</loc>
      <lastmod>2009-01-01</lastmod>
      <changefreq>monthly</changefreq>
      <priority>0.8</priority>
   </url>
</urlset>
END;

exit;
 */

// "always", "hourly", "daily", "weekly", "monthly", "yearly", or "never"	
// This prints out the sitemap index 
print <<<END
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.google.com/schemas/sitemap/0.84" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.google.com/schemas/sitemap/0.84 http://www.google.com/schemas/sitemap/0.84/sitemap.xsd">

END;

$x='';

$date = date('Y-m-d');

if ( $SITES = $db->get_results("SELECT type,state,city,biz_id,name FROM businesses WHERE type!='' && state != '' && city != '' "))
{
	foreach ( $SITES as $site )
	{

	$x .=
		"<url>".
		"<loc>http://" .$_SERVER['HTTP_HOST']. (str_replace('&','&amp;',str_replace(' ','-',"/$site->type/{$GLOBALS['STATES_CAPITALISED'][$site->state]}/$site->city/$site->biz_id/$site->name")))."</loc>".
		"<lastmod>".date("Y-m-d")."</lastmod>".
		"<changefreq>monthly</changefreq>".
		"<priority>0.8</priority>".
		"</url>";
	}
}

print "$x</urlset>";

?>

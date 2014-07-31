<?php

	include "lib/state_utils.inc";

	// Must have a numeric zip or go back to home
	if ( ! isset($TARGET->arg3) || ! is_numeric($TARGET->arg3) )
	{
		notify_redirect('/');
	}  

	$STATE_SHORT = '';
	$STATE       = '';
	$ZIP       = $TARGET->arg3;
	$MATCH_SQL = ", MATCH (b.search_bucket) AGAINST ('{$ZIP}') AS rating";
	$WHERE_SQL = "MATCH (b.search_bucket) AGAINST ('{$ZIP}' IN BOOLEAN MODE)";
	$ORDER_BY  = 'ORDER BY name ASC';

	// Load in all the businesses
	if ( ! $BUSINESSES = $db->get_results("SELECT b.*, p.*, b.biz_id $MATCH_SQL FROM businesses b LEFT JOIN biz_pics p on p.biz_id=b.biz_id AND p.pic_type = 'logo' WHERE b.type = '".$db->escape($TARGET->arg1)."' AND $WHERE_SQL $ORDER_BY" ) )
	{
		$back_to = '/';
		if ( preg_match('/by-zip/',$_SERVER['HTTP_REFERER']) )
		{
			;
		}
		else if ( isset($_SERVER['HTTP_REFERER']) && $_SERVER['HTTP_REFERER'] )
		{
			//$back_to = $_SERVER['HTTP_REFERER']; // This is no good whenusing the widgets
			$back_to = '/';
		}
		
		error_redirect($back_to,str_replace(array('<ZIP>'),array($ZIP),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['no-results']));
	}
	else
	{

		include_once "interfaces/google_geocode.inc";

		// JS String that is injected in the .htm fiel for this action
		$GMAP_MARKER_JS = '';


		// Add links and geocode into to the result set
		foreach ( $BUSINESSES as $idx => $biz )
		{

			// Get geocode information (if we don't already have it)
			$biz = get_geocode($biz);

			$BUSINESSES[$idx]->logo = false;
			if ( isset($biz->pic_id) )
			{
				$BUSINESSES[$idx]->logo = "/data/pics/{$biz->biz_id}-{$biz->pic_id}.jpg";
			}

			// Format the link & zip, ready for use in action .htm
			$BUSINESSES[$idx]->link = "/{$TARGET->arg1}/".format_link_subject(get_long_state($biz->state))."/".format_link_subject($biz->city)."/{$biz->biz_id}/".format_link_subject($biz->name);
			$BUSINESSES[$idx]->zip  = preg_replace('/^(\d+)\-\d+/','\1',$biz->zip);

			if ( $biz->geocode_status == 'retrieve_success' )
			{
				// This sets up the javascript that will be used to add the gnmap markers (used in the .htm file)
				$GMAP_MARKER_JS .= "ezAjax.gmaps.addMarker({$biz->latitude}, {$biz->longitude}, \"".
					( isset($biz->pic_id) ? '<div class=\"logo_50x50\"><img src=\"'.$BUSINESSES[$idx]->logo.'\" /></div>': '').
					'<div class=\"map-address-block\">'."<b>{$biz->name}</b><br />{$biz->street}<br />{$biz->city}, {$biz->state} {$biz->zip}<br />{$biz->phone}</div>\",$idx);\n";
			}
		}

		// Center the map based on the first result
		$MAP_CENTER_JS_ARGS = "{$BUSINESSES[0]->latitude}, {$BUSINESSES[0]->longitude}";

		$STATE_SHORT = $BUSINESSES[0]->state;
		$STATE       = get_long_state($STATE_SHORT);
		$CITY = $BUSINESSES[0]->city;

	}


	$TARGET->page_title    = str_replace(array('<CITY>','<ZIP>','<STATE>','<STATE_SHORT>'),array($CITY,$ZIP,$STATE,$STATE_SHORT),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['page-title']);
	$TARGET->content_title = str_replace(array('<CITY>','<ZIP>','<STATE>','<STATE_SHORT>'),array($CITY,$ZIP,$STATE,$STATE_SHORT),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['content-title']);
	$TARGET->meta_desc = str_replace(array('<CITY>','<ZIP>','<STATE>','<STATE_SHORT>'),array($CITY,$ZIP,$STATE,$STATE_SHORT),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['meta-desc']);
	$TARGET->meta_keys = str_replace(array('<CITY>','<ZIP>','<STATE>','<STATE_SHORT>'),array($CITY,$ZIP,$STATE,$STATE_SHORT),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['meta-keys']);


	// This is used to build the breadcrumb path
	$BREADCRUMBS = array
	(
		array("/",'Home'),
		array("/{$TARGET->arg1}",$GLOBAL_NAV[$TARGET->arg1]['crumb']),
		array(false,"By Zip: $ZIP"),
	);

?>

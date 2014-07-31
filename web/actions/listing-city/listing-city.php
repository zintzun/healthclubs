<?php

	include "lib/state_utils.inc";

	// Process incoming arguments to make em look nice
	$STATE       = ucfirst(str_replace('-',' ',$TARGET->arg2));
	$CITY        = ucwords(str_replace('-',' ',$TARGET->arg3));
	$STATE_SHORT = get_short_state($STATE);

	// Load in all the businesses
	if ( $BUSINESSES  = $db->get_results("SELECT b.*, p.*, b.biz_id FROM businesses b LEFT JOIN biz_pics p on p.biz_id=b.biz_id AND p.pic_type = 'logo' WHERE b.city = '".$db->escape($CITY)."'" . " AND b.state = '" .$db->escape($STATE_SHORT)."' AND b.type = '".$db->escape($TARGET->arg1)."' ORDER BY b.name" ) )
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
			$BUSINESSES[$idx]->link = "/{$TARGET->action_orig}/{$biz->biz_id}/".format_link_subject($biz->name);
			$BUSINESSES[$idx]->zip  = preg_replace('/^(\d+)\-\d+/','\1',$biz->zip);

			if ( $biz->geocode_status == 'retrieve_success' )
			{
				// This sets up the javascript that will be used to add the gnmap markers (used in the .htm file)
				$GMAP_MARKER_JS .= "ezAjax.gmaps.addMarker({$biz->latitude}, {$biz->longitude}, \"".
					( isset($biz->pic_id) ? '<div class=\"logo_50x50\"><img src=\"'.$BUSINESSES[$idx]->logo.'\" /></div>': '').
					'<div class=\"map-address-block\">'."<b>{$biz->name}</b><br />{$biz->street}<br />$CITY, $STATE {$biz->zip}<br />{$biz->phone}</div>\",$idx);\n";
			}
		}

		// If more than one business then use $CITY, $STATE as map center
		// (or if no sensible geocode on 1st result)
		if ( count($BUSINESSES) > 1 || $BUSINESSES[0]->geocode_status == 'un_retrieved' )
		{
			$GG = new google_geocode;
			$MAP_CENTER_JS_ARGS = $GG->get_coords_as_js_args("$CITY, $STATE");
		}
		else
		{
			$MAP_CENTER_JS_ARGS = "{$BUSINESSES[0]->latitude}, {$BUSINESSES[0]->longitude}";
		}
	}
	else
	{
		$FORCE_ERROR_PAGE = true;
		return;
	}

	if ( ! isset($FORCE_ERROR_PAGE) )
	{

		// This is used to build the breadcrumb path
		$TARGET->arg1 = strtolower($TARGET->arg1);
		$TARGET->arg2 = strtolower($TARGET->arg2);
		$BREADCRUMBS = array
		(
			array("/",'Home'),
			array("/{$TARGET->arg1}",$GLOBAL_NAV[$TARGET->arg1]['crumb']),
			array("/{$TARGET->arg1}/{$TARGET->arg2}",$STATE),
			array(false,"$CITY {$GLOBAL_NAV[$TARGET->arg1]['crumb']}"),
		);
	
		$TARGET->page_title = str_replace(array('<CITY>','<STATE>','<STATE_SHORT>'),array($CITY,$STATE,$STATE_SHORT),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['page-title']);
		$TARGET->content_title = str_replace(array('<CITY>','<STATE>','<STATE_SHORT>'),array($CITY,$STATE,$STATE_SHORT),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['content-title']);
		$TARGET->meta_desc = str_replace(array('<CITY>','<STATE>','<STATE_SHORT>'),array($CITY,$STATE,$STATE_SHORT),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['meta-desc']);
		$TARGET->meta_keys = str_replace(array('<CITY>','<STATE>','<STATE_SHORT>'),array($CITY,$STATE,$STATE_SHORT),$GLOBAL_NAV[$TARGET->arg1]['titles'][$TARGET->action]['meta-keys']);

	}

?>

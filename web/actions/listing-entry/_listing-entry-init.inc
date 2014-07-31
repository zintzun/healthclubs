<?php

	include "lib/state_utils.inc";
	include "interfaces/biz/biz_reviews.inc";
	include "interfaces/biz/biz_details.inc";
	include "interfaces/picklist.inc";
	include "interfaces/biz/biz_pics.inc";
	include "interfaces/biz/biz_link_to_user.inc";

	// Process incoming arguments to make em look nice
	$STATE       = str_replace('-',' ',$TARGET->arg2);
	$CITY        = str_replace('-',' ',$TARGET->arg3);
	$STATE_SHORT = get_short_state($STATE);

	// Get details for this club specifically
	if (!$ENTRY = $db->get_row("SELECT * FROM businesses WHERE biz_id = '".$db->escape($TARGET->arg4)."'"))
	{
		$FORCE_ERROR_PAGE = true;
		return;
	}

	if ( ! isset($TARGET->arg5) )
	{
		header("HTTP/1.1 301 Moved Permanently");
		header("Location: /".$TARGET->action_orig.'/'.format_link_subject($ENTRY->name));
	}

	// Find out if biz is linked to user
	$biz_is_linked = $db->get_var("SELECT count(*) FROM biz_link_to_user WHERE biz_id=".$ENTRY->biz_id." AND is_linked=1");

	// Get the geocode fro this business (if it doesn't already have one)
	include_once "interfaces/google_geocode.inc";
	$ENTRY = get_geocode($ENTRY);

	$ENTRY->zip = preg_replace('/^(\d+)\-\d+/','\1',$ENTRY->zip);

	// Get Biz pics & logo
	$biz_pic = new biz_pic;
	$biz_pics = null;
	$biz_pics = $biz_pic->get_image_paths($ENTRY->biz_id);
	$biz_logo =	$biz_pic->get_logo_path($ENTRY->biz_id);

	$GMAP_MARKER_JS = '';
	if ( $ENTRY->geocode_status == 'retrieve_success' )
	{
		// This sets up the javascript that will be used to add the gnmap markers (used in the .htm file)
		$GMAP_MARKER_JS .= "ezAjax.gmaps.addMarker({$ENTRY->latitude}, {$ENTRY->longitude}, \"".
			($ENTRY->is_premium && $biz_is_linked && $biz_logo ? '<div class=\"logo_50x50\"><img src=\"'.$biz_logo.'\" /></div>': '').
			'<div class=\"map-address-block\">'."<b>{$ENTRY->name}</b><br />{$ENTRY->street}<br />$CITY, $STATE {$ENTRY->zip}<br />{$ENTRY->phone}</div>\",0,true);\n";
		$MAP_CENTER_JS_ARGS = "{$ENTRY->latitude}, {$ENTRY->longitude}";
	}

	// This is used to build the breadcrumb path
	$BREADCRUMBS = array
	(
		array("/",'Home'),
		array("/{$TARGET->arg1}",$GLOBAL_NAV[strtolower($TARGET->arg1)]['crumb']),
		array("/{$TARGET->arg1}/{$TARGET->arg2}",$STATE),
		array("/{$TARGET->arg1}/{$TARGET->arg2}/{$TARGET->arg3}",$CITY),
		array(false,$ENTRY->name),
	);

	$TARGET->meta_keys .= "$ENTRY->name ";

	$meta_keys = explode(',', ($TARGET->arg1 == 'health-clubs' ? 'Health Clubs ,' : 'Personal Trainers ,') . META_KEYS);
	 	
	foreach ($meta_keys as $key)
	{
		$TARGET->meta_keys .= ", $key $CITY, $key $STATE ";
	}

	$TARGET->page_title = str_replace(array('<NAME>','<CITY>','<STATE>','<STATE_SHORT>'),array($ENTRY->name,$CITY,$STATE,$STATE_SHORT),$GLOBAL_NAV[strtolower($TARGET->arg1)]['titles'][$TARGET->action]['page-title']);
	$TARGET->content_title = str_replace(array('<NAME>','<CITY>','<STATE>','<STATE_SHORT>'),array($ENTRY->name,$CITY,$STATE,$STATE_SHORT),$GLOBAL_NAV[strtolower($TARGET->arg1)]['titles'][$TARGET->action]['content-title']);
	$TARGET->meta_desc = str_replace(array('<NAME>','<CITY>','<STATE>','<STATE_SHORT>'),array($ENTRY->name,$CITY,$STATE,$STATE_SHORT),$GLOBAL_NAV[strtolower($TARGET->arg1)]['titles'][$TARGET->action]['meta-desc']);
	$TARGET->meta_keys = str_replace(array('<NAME>','<CITY>','<STATE>','<STATE_SHORT>'),array($ENTRY->name,$CITY,$STATE,$STATE_SHORT),$GLOBAL_NAV[strtolower($TARGET->arg1)]['titles'][$TARGET->action]['meta-keys']);

	//Select reviews fom the db about this business
	$REVIEWS = $db->get_results("SELECT * FROM reviews WHERE biz_id = {$TARGET->arg4} AND is_approved = 1 ORDER BY review_id DESC");

	// Obtain biz average rating.
	$biz_rating=0;
	if ( $REVIEWS )
	{
		foreach ( $REVIEWS as $review )
		{
			if (! $review->is_approved && $review->user_id != myuid() )
			{
				continue;
			}
			$biz_rating += $review->rating;
		}
	}

	if ( $review_count =  count($REVIEWS) )
	{
		$biz_rating = $biz_rating / $review_count;
	}

	// Coupon and claim link link
	$business = new business;
	$biz = $business->get($TARGET->arg4);
	
	$picklist = new picklist($biz->type.'/coupons');

	// Coupon
	$coupon_link = false;
 	if ( $biz->coupon_id && ! is_error($coupon = $picklist->get_text($biz->coupon_id)) )
	{
		$coupon_link = "/{$TARGET->action_orig}/{$biz->coupon_id}/".ucwords(str_replace(' ','-',$coupon->value));
	}

	// Claim link
	$claim_link = "/{$biz->type}/claim-biz/{$TARGET->arg4}";

	// Amenities
	$picklist = new picklist($ENTRY->type."/amenities");
	$amenities = $picklist->get($ENTRY->biz_id);

?>

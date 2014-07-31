<?php

	include "lib/state_utils.inc";

	$STATE       = ucfirst(str_replace('-',' ',$TARGET->arg2));
	$STATE_SHORT = get_short_state($STATE);
	$CITIES      = $db->get_col("SELECT city FROM businesses WHERE state = '$STATE_SHORT' GROUP BY city");

	// This is used to build the breadcrumb path
	$BREADCRUMBS = array
	(
		array("/",'Home'),
		array("/{$TARGET->arg1}",$GLOBAL_NAV[strtolower($TARGET->arg1)]['crumb']),
		array(false,"$STATE {$GLOBAL_NAV[strtolower($TARGET->arg1)]['crumb']}"),
	);

	$TARGET->page_title    = str_replace(array('<STATE>','<STATE_SHORT>'),array($STATE,$STATE_SHORT),$GLOBAL_NAV[strtolower($TARGET->arg1)]['titles'][$TARGET->action]['page-title']);
	$TARGET->content_title = str_replace(array('<STATE>','<STATE_SHORT>'),array($STATE,$STATE_SHORT),$GLOBAL_NAV[strtolower($TARGET->arg1)]['titles'][$TARGET->action]['content-title']);
	$TARGET->meta_desc = str_replace(array('<STATE>','<STATE_SHORT>'),array($STATE,$STATE_SHORT),$GLOBAL_NAV[strtolower($TARGET->arg1)]['titles'][$TARGET->action]['meta-desc']);
	$TARGET->meta_keys = str_replace(array('<STATE>','<STATE_SHORT>'),array($STATE,$STATE_SHORT),$GLOBAL_NAV[strtolower($TARGET->arg1)]['titles'][$TARGET->action]['meta-keys']);

	$__letter_bar = '';
	$clouds       = array();

	if ( $cities = $db->get_results("SELECT city,count(city) AS count FROM businesses WHERE state='".$STATE_SHORT."' AND type = '".$db->escape($TARGET->arg1)."' GROUP BY city ORDER BY city") )
	{

		$letter = $cities[0]->city[0];
		$link_path = "/$TARGET->arg1/$TARGET->arg2/%s";
	
		foreach ($cities as $result)
		{
			if ($letter != $result->city[0])
			{
				$clouds[$letter] = create_tag_cloud_html($tags[$letter], $link_path);
				$letter = $result->city[0];
			 } 
			$tags[$letter][$result->city] = $result->count;
		}
		$clouds[$letter] = create_tag_cloud_html($tags[$letter], $link_path);
		$letter = $result->city[0];
	
		
		foreach (array_keys($tags) as $letter)
		{
			$__letter_bar .= '<a href="#'.$letter.'">'.$letter.'</a> ';
		}
	}
	else
	{
		$FORCE_ERROR_PAGE = true;
		$BREADCRUMBS = false;
	}

?>

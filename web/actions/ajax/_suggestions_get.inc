<?php

	include "lib/state_utils.inc";

	/*
	* Main input error checking
	*/

	$error = 'OK';
	$suggestions = false;

	if ( isset($_REQUEST['query']) )
	{
		$query  = padd_keywords($_REQUEST['query']);	

		$sql = "SELECT *, MATCH (search_bucket) AGAINST ('$query') AS rating FROM businesses WHERE MATCH (search_bucket) AGAINST ('".str_replace(' ','* ',$query)."*' IN BOOLEAN MODE) ORDER BY rating DESC LIMIT 10";

		if ( $results = $db->get_results($sql) )
		{
			foreach ( $results as $item )
			{
				$state = get_long_state($item->state);
				$suggestions[] = array
				(
					'link' => "/{$item->type}/".format_link_subject($state).'/'.format_link_subject($item->city).'/'.$item->biz_id.'/'.format_link_subject($item->name),
					'text' => "{$item->name}, $item->city, $item->state, ".substr($item->zip,0,5)
				);
			}
		}
				
	}

	print json_encode(array('error' => $error, 'suggestions' => $suggestions));

?>

<?php

	/*
	* Search for businesses matching query
	*/

	$error = 'OK';
	$businesses = false;

	if ( isset($_POST['query']) )
	{
		$query  = padd_keywords($_POST['query']);	

		$sql = "
		SELECT 
			biz_link_to_user.biz_id as bizid, 
			biz_link_to_user.user_id AS userid,
			biz_link_to_user.is_linked as is_linked,
			businesses.*,
			users.login,
			users.email,
			MATCH (search_bucket) AGAINST ('$query') AS rating 
		FROM 
				businesses 
				LEFT JOIN biz_link_to_user ON businesses.biz_id=biz_link_to_user.biz_id
				LEFT JOIN users ON users.user_id=biz_link_to_user.user_id
		WHERE 
			MATCH (search_bucket) AGAINST ('+".str_replace(' ',' +',$query)."*' IN BOOLEAN MODE) 
		ORDER BY 
			rating 
		DESC LIMIT 20";

		if ( $results = $db->get_results($sql) )
		{
			foreach ( $results as $item )
			{
				if ( strlen($item->name) > 30 )
				{
					$item->name = substr($item->name,0,30).'..';
				}
				
				$businesses[] = array
				(
					'bizId'     => $item->biz_id,
					'userId'    => $item->userid,
					'isLinked'  => $item->is_linked,
					'text'      => "{$item->name}, {$item->city}, {$item->state}",
					'linkedTo'  => $item->login?"{$item->login} ({$item->email})":'',
				);
			}
		}
	}

	print json_encode(array('error' => $error, 'businesses' => $businesses));

?>

<?php

	/*
	* Search for users(biz owners only) matching query 
	*/

	$error = 'OK';

	$users = false;

	if ( isset($_POST['query']) )
	{
		$user = new user();

		if ($results = $user->get_users('', $_POST['query'].'*', true) )
		{
			foreach ($results as $item)
			{
				$text = "{$item->login} ({$item->email}) {$item->first_name} {$item->last_name}";
				
				if ( strlen($text) > 40 )
				{
					$text = substr($text,0,40).'..';
				}
				
				$users[] = array
				(
					'user_id' => $item->user_id,
					'text'   => $text
				);
			}
		}
	}

	print json_encode(array('error' => $error, 'users' => $users));

?>


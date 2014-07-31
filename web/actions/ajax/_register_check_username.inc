<?php

	$userAvailable = false;

	if ( isset($_POST['username']) )
	{
		$user = new user;
		$userAvailable = ! $user->get($_POST['username']);
	}

	print json_encode(array('error' => 'OK', 'userAvailable' => $userAvailable));

?>
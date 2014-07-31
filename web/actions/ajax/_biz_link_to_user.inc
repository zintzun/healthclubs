<?php

	/*
	* Will create or delete link between user_id and biz_id
	*
	* Parameters expected:
	*
	* 	user_id
	* 	biz_id
	* 	approve_or_remove
	*/

	$error = 'OK';

	if ( ! isset($_POST['user_id']) )
	{
		$error = 'Unexpected error: Missing user id.';
	}
	else if ( ! isset($_POST['biz_id']) )
	{
		$error = 'Unexpected error: Missing biz id.';
	}
	else if ( ! isset($_POST['approve_or_remove']) || ! in_array($_POST['approve_or_remove'], array('approve','remove')) )
	{
		$error = 'Unexpected error: Unknown action.';
	}

	if ($error == 'OK')
	{
		include "interfaces/biz/biz_link_to_user.inc";
		
		$biz_link_to_user = new biz_link_to_user;
		
		if ($lib_error = is_error( $biz_link_to_user->$_POST['approve_or_remove']($_POST['user_id'],$_POST['biz_id']) ))
		{
			if ($_POST['approve_or_remove'] == 'approve')
			{
				$error = "Unexpected error: Could not link user to business.";
			}
			else
			{
				$error = "Unexpected error: Could not remove link between user and business.";
			}
		}
	}
	
	print json_encode(array('error' => $error));
	
?>

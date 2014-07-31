<?php
	/*
	* Toggles is_done status for lead
	*
	* Parameters expected:
	*
	* 	biz_id
	* 	is_premium
	*/

	$error = 'OK';
   $is_done = '';

	if ( ! isset($_POST['biz_id']) || ! is_numeric($_POST['biz_id']) )
	{
		$error = 'Unexpected error: Missing business id.';
	}
	else if ( ! is_numeric($_POST['biz_id']) )
	{
		$error = 'Unexpected error: Invalid business id.';
	}
	else if ( ! isset($_POST['is_premium']) )
	{
		$error = 'Unexpected error: Missing listing premium status.';
	}
	else if ( ! in_array($_POST['is_premium'],array('0','1')) )
	{
		$error = 'Unexpected error: Invalid listing premium status.';
	}
	
	if ( $error == 'OK' )
	{
		global $db;

		if ( $error == 'OK' )
		{
			$db->query("UPDATE businesses SET is_premium = ". $_POST['is_premium']." WHERE biz_id = ".$_POST['biz_id']);
		}
	}
	
	print json_encode(array('error' => $error));

?>

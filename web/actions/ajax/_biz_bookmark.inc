<?php

	/*
	* Will create a biz bookmark. Add a record in biz_bookmarks
	*
	* Parameters expected:
	*
	* 	user_id
	* 	biz_id
	*
	*/

	$error = 'OK';
	$is_old_bookmark = false;

	if ( ! isset($_POST['user_id']) )
	{
		$error = 'Unexpected error: Missing user id.';
	}
	else if ( ! isset($_POST['biz_id']) )
	{
		$error = 'Unexpected error: Missing biz id.';
	}

	if ($error == 'OK')
	{
		include "interfaces/biz/biz_bookmarks.inc";
		
		$bookmark = new biz_bookmark;

		if ($bookmark->get($_POST['user_id'],$_POST['biz_id']) )
		{
			$is_old_bookmark = true;
		}
		else {
			
			if ($lib_error = is_error( $bookmark->add($_POST['user_id'],$_POST['biz_id']) ))
			{
				if ($lib_error == "invalid_user_id")
				{
					$error = "Unexpected error: Invalid User Id.";
				}
				else if ($lib_error == "invalid_biz_id")
				{
					$error = "Unexpected error: Invalid Bussiness Id.";
				}
			}
		}
	}
	
	print json_encode(array('error' => $error, 'isOldBookmark' => $is_old_bookmark));

?>

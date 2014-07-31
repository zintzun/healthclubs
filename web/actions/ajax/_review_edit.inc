<?php

	$error = 'OK';
		
	if ( ! session_is_active() )
	{
	  $error = "Please log in";
	}
	else if (!isset($_POST['reviewId']) || ! is_numeric($_POST['reviewId']) )
	{
		$error = "Unexpected error: Invalid review id";
	}
	else if (!isset($_POST['rating']) ||  ! is_numeric($_POST['rating']) )
	{
		$error = "Please click a star to rate this business.";
	}
	else if (!isset($_POST['review']) ||  ! $_POST['review'] = trim($_POST['review']) )
	{
		$error = "Please write a review for this busines.";
	}
	else if ($db->get_var("SELECT user_id FROM reviews WHERE review_id = '".$db->escape($_POST['reviewId'])."'") != session_user_id())
	{
		$error = "Unexpected error: not owner of this review.";
	}
	
	if ($error == 'OK')
	{
		include "interfaces/biz/biz_reviews.inc";
		
		$parms = array 
		(
			'user_id'		=> session_user_id(),
			'review_id' => $_POST['reviewId'],
			'review' 		=> $_POST['review'],
			'rating' 		=> $_POST['rating'],
		);
		$review = new review;

		if ($test_error=is_error($review->add_or_update($parms)))
		{
			$error = "Unexpected error: $test_error";
		}

	}


	print json_encode(array('error' => $error, 'test' => 'test1'));


?>
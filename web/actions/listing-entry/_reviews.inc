<?php

	/*
	* Error strings for reviews
	*/

	$STRINGS = array
	(
		'invalid_user_id' => 'We couldnt find that user!',
		'no_session'		=> 'User not logged in!',
	);

	if ( isset($_POST['do_add_review']) && isset($_POST['new-review']) )
	{

		/*
		* Error checking for add review
		*/

		$error = false;

		// Always error check
		if ( ! isset($_POST['new-review']) || ! trim($_POST['new-review']) )
		{
			$error = "Please enter a review";
		}
		else if ( ! isset($_POST['star-new-rating']) || ! is_numeric($_POST['star-new-rating']) || ! $_POST['star-new-rating'] > 0 )
		{
			$error = "Please enter a rating";
		}

		// Logged out state / anonymous error checks
		if ( ! isset($_POST['new-name']) || ! trim($_POST['new-name']) )
		{
			$error = "Please enter your name";
		}
		else if ( ! isset($_POST['new-email']) || !eregi("^[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$", $_POST['new-email']) )
		{
			$error = "Please enter a valid email address";
		}
		else if ( ! captcha_is_correct() )
		{
			$error = "Please re-enter the reCAPTCHA information";
		}

		if ( $error )
		{
			error_me($error); // Note: This exits and redirects
		}

		/*
		* Insert new review (if no error)
		*/

		$review = new review;

		$parms =	array
		(
			       'name' => $_POST['new-name'],
			      'email' => $_POST['new-email'],
			    'review'  => $_POST['new-review'],
			    'rating'  => $_POST['star-new-rating'],
			     'biz_id' => $TARGET->arg4,
			    'type'    => $TARGET->arg1,
			    'user_id' => 0,
			'is_approved' => 0
		);

		if ( session_is_active() )
		{
			$parms['user_id'] = $_SESSION['USER']['user_id'];
			$parms['is_approved'] = true;	
		}
		else
		{
		 	// Send email to reviewer
			include "interfaces/mail.inc";
			send_mail_helper($parms['email'],"template/mail/message-to-reviewer.txt",array('name' => $parms['name']));
		}

		// Add Review for this business
		if ( $error = is_error($review_id = $review->add_or_update($parms)) )
		{
   		error_me(get_error($error));
		}

		session_unregister('LAST_POST');

		// yayy, it is all good!
		notify_me("Your review has been added successfully");
	}

?>

<?php

  if (isset($TARGET->arg2) && !in_array($TARGET->arg2,array('basic-info','thank-you')))
	{
		$FORCE_ERROR_PAGE = true;
		return;
	}

	if ( session_is_active() && session_is_biz_owner() )
	{
			notify_redirect("/biz/add-listing/biz-info?new");
	}

	include "lib/state_utils.inc";

	$TARGET->page_title = $GLOBAL_NAV['biz/add-listing']['page-title'];
	
	$fields = array
	(
		'first_name',
		'last_name',
		'position',
		'phone',
		'email',
		'business_name',
		'type',
		'street1',
		'street2',
		'city',
		'state',
		'zip',
		'website',
	);

	if ( isset($_POST['do_submit_basic_info']) )
	{
		if (! isset($_POST['email']) || ! ($_POST['email'] = trim($_POST['email']) ) )
		{
			error_redirect('/add-listing/basic-info','Please provide your email address.');
		}
		else if ( !eregi("^[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$", $_POST['email']) )
		{
			error_redirect('/add-listing/basic-info','Please provide a valid email address.');
		}
		else if ( ! captcha_is_correct() )
		{
			error_redirect('/add-listing/basic-info',"Please re-enter the reCAPTCHA information");
		} 
		
		// Send  email with question 
		include "interfaces/mail.inc";

		foreach ($fields as $field)
		{
			$msg_parms[$field] = isset($_POST[$field]) ? $_POST[$field] : '';
		}

		$msg_parms['from'] = APP_EMAIL;

		if (isset($_POST['type']))
		{
			$msg_parms['type'] = $_POST['type'] ? 'Personal Trainer' : 'Healthclub';
		}

		send_mail_helper($msg_parms['email'],"template/mail/biz_add_listing_confirmation.txt",$msg_parms);
		send_mail_helper(SUPPORT_EMAIL,"template/mail/biz_add_listing_info.txt",$msg_parms);
							
		session_unregister('LAST_POST');
		notify_redirect('/add-listing/thank-you','');
	}

?>


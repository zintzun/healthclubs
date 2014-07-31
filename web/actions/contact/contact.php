<?php

  $question = 'Your Question';

	if (isset($TARGET->arg2) && is_numeric($TARGET->arg2))
	{
		include "interfaces/biz/biz_details.inc";
		$business = new business;
		$biz_info = $business->get($TARGET->arg2);
		$_POST['reason'] = 3;
		$suggestion[0] = "Corrections to ".$biz_info->name.' ( id:'.$biz_info->biz_id.' )';
		$question = 'Suggestion(s)';
	}


	$TARGET->page_title = $GLOBAL_NAV['contact']['page-title'];
	$TARGET->meta_desc  = $GLOBAL_NAV['contact']['meta-desc'];
	$TARGET->meta_keys  = $GLOBAL_NAV['contact']['meta-keys'];
	$__title = $GLOBAL_NAV['contact']['content-title'];
	
	$fields = array
	(
		'first_name',
		'last_name',
		'email',
		'phone',
		'reason',
		'question',
	);

	if ( isset($_POST['do_contact_us']) )
	{
		if (isset($_POST['question']) && ($_POST['question'] = trim($_POST['question'])) )
		{
			if (! isset($_POST['email']) || ! ($_POST['email'] = trim($_POST['email']) ) )
			{
				error_redirect('/contact','Please provide your email address.');
			}
			else if ( !eregi("^[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$", $_POST['email']) )
			{
				error_redirect('/contact','Please provide a valid email address.');
			}
			else if ( ! captcha_is_correct() )
			{
				error_redirect('contact',"Please re-enter the reCAPTCHA information");
			} 

			// Send  email with question 
			include "interfaces/mail.inc";

			$msg_parms = array
			(
				'ip_address'		=>	$_SERVER['REMOTE_ADDR'],
				'from'               => APP_EMAIL,
				'senders_email'      => $_POST['email'],
				'senders_first_name' => trim($_POST['first_name']),
				'senders_last_name'  => trim($_POST['last_name']),
				'senders_phone'      => trim($_POST['phone']),
				'contact_reason'     => (isset($_POST['suggestion'])? $_POST['suggestion'] : ($_POST['reason']? $CONTACT_REASONS[$_POST['reason']]:'No Reason Selected')),
				'question'           => trim($_POST['question']),
				'club_link'          => isset($_POST['suggestion'])? 'http://'.$_SERVER['HTTP_HOST'].'/admin/business/'.$biz_info->biz_id : '',
			);
			send_mail_helper(SUPPORT_EMAIL,"template/mail/contact.txt",$msg_parms);
								
			session_unregister('LAST_POST');
			notify_redirect('/contact','Thanks for contacting us, we will get back to you soon.');
		}
	}

?>

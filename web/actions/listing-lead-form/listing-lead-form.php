<?

	include "interfaces/biz/biz_details.inc";
	include "interfaces/picklist.inc";
	
	$business = new business;
	$__biz = $business->get($TARGET->arg4);

	$picklist = new picklist($__biz->type.'/coupons');

	if ( ! ($__coupon = $picklist->get_text($__biz->coupon_id) ) )
	{
    $FORCE_ERROR_PAGE = true;
    return;
	}

	$__form_was_submitted = false;
	
	if (isset($_POST['do_coupon']))
	{
		foreach (array ('first_name','last_name','email','zip') as $field)
		{
			if ( ! isset($_POST[$field]) || ! ($_POST[$field]=trim($_POST[$field])) )
			{
				error_redirect('/'.$TARGET->action_orig,"Please enter your ".str_replace('_',' ',$field) );
			
			}
		}
		if (! eregi("^[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[    a-z0-9-]+)*(\.[a-z]{2,4})$", $_POST['email']) )
		{
			error_redirect('/'.$TARGET->action_orig,"Please enter a valid email address.");
		}
		foreach (array ('area_code','phone_first_3','phone_last_4') as $field)
		{
			if ( ! isset($_POST[$field]) || ! ($_POST[$field]=trim($_POST[$field])) )
			{
				error_redirect('/'.$TARGET->action_orig,"Please enter your phone number.");
			}
		}
		if ( ! isset($_POST['i_am_18']) )
		{
			error_redirect('/'.$TARGET->action_orig,"Please confirm your age.");
		}
		else if ( ! $_POST['i_am_18'])
		{
			error_redirect('/'.$TARGET->action_orig,"Our coupons are available for users older than 18 only.");
		}
		
		// Form is ok. Insert new lead record in table.
		include "interfaces/biz/biz_leads.inc";
		$__biz_lead = new biz_lead;
		
		$__parms = array
		(
			'first_name' => $_POST['first_name'],
			'last_name'  => $_POST['last_name'],
			'email'      => $_POST['email'],
			'phone'      => $_POST['area_code'].'-'.$_POST['phone_first_3'].'-'.$_POST['phone_last_4'],
			'zip'        => $_POST['zip'],
			'i_am_18'    => $_POST['i_am_18'],
			'biz_id'     => $_POST['biz_id'],
			'type'       => $_POST['type'],
			'coupon'     => $_POST['coupon'],
			'lead_rate'  => get_lead_rate($db->get_var("SELECT user_id FROM biz_link_to_user WHERE biz_id = {$__biz->biz_id}")), //see utils.inc
		);

		// Send email to reviewer
		include "interfaces/mail.inc";
		send_mail_helper($_POST['email'],"template/mail/message-to-lead.txt",array('name' => $_POST['first_name'],'club' => $__biz->name));
		send_mail_helper(SEND_LEAD_CC_TO,"template/mail/message-to-lead.txt",array('name' => $_POST['first_name'],'club' => $__biz->name));

		// Add record to table.
		$__biz_lead->add_or_update($__parms);

		session_unregister('LAST_POST');
		$__form_was_submitted = true;
	}

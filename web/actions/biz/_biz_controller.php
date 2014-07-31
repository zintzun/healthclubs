<?php

	/*
	* Dealing with all biz related functions for this website
	*/

	class biz_controller
	{
		
		public function __construct()
		{
			$this->v->fields = array
			(
				'username'  => 'Username',
				'email'     => 'Email address',
				'password'  => 'Password',
				'password2' => 'Confirm password',
			);
		}

		/*
		* Main control panel
		*/
		public function biz_index()
		{
			if ( session_is_active() )
			{
				$this->try_redirect_to_control_panel();
			}
			else
			{
				notify_redirect('/biz/login');	
			}
		}

		/*
		* Sucess message shown after user clicks join
		*/

		public function biz_join_success()
		{
			// No controller code needed, just display the htm file
		}

		/*
		* Failure message shown after user clicks join
		*/

		public function biz_join_error()
		{
			// No controller code needed, just display the htm file
		}

		/*
		* Biz join page
		*/

		public function biz_join()
		{
			
			global $TARGET;

			session_forget_me();
			
			if ( isset($_POST['do_register']) )
			{

				$error = false;

				$user = new user;

				// username must exist
				if ( ! isset($_POST['username']) || ! $_POST['username'] = trim($_POST['username']) )
				{
					$error = "Please enter your user name";
				}
				// Bad chars in username
				else if ( preg_match('/[^a-zA-Z0-9\_]/',$_POST['username']) )
				{
					$error = "Please only use standard characters<br />in your username (a-z A-Z 0-9 _ )";
				}
				// User must not already exist
				else if ( $user->get($_POST['email']) )
				{
					$error = "An account with this email already exists. Would you like to <a href='/member/login'>sign in?</a> (<a href='/member/forgot-password'>Forgot password?</a>)";
				}
				// User must not already exist
				else if ( $user->get($_POST['username']) )
				{
					$error = "An account with this user name already exists. Try something like <b>{$_POST['username']}".rand(1,999)."</b>";
				}
				// Email must exist
				else if ( ! isset($_POST['email']) || ! $_POST['email'] = trim($_POST['email']) )
				{
					$error = "Please enter your email address";
				}
				// Must be a valid email address
				else if ( !eregi("^[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$", $_POST['email']) )
				{
					$error = "Please enter a valid email address";
				}
				// Password must exist
				else if ( ! isset($_POST['password']) || ! $_POST['password'] )
				{
					$error = "Please enter your password";
				}
				// Password must exist
				else if ( ! isset($_POST['password2']) || ! $_POST['password2'] )
				{
					$error = "Please enter a confirmation password";
				}
				// Password must be correct length
				else if ( $_POST['password'] != $_POST['password2'] )
				{
					$error = "The confirmation password does not match";
				}
				// Password must be correct length
				else if ( strlen($_POST['password']) < 6 || strlen($_POST['password']) > 30 )
				{
					$error = "Your new password must be greater than 5 and less than 31 characters";
				}
				// username must be correct length
				else if ( strlen($_POST['username']) < 2 || strlen($_POST['username']) > 32 )
				{
					$error = "Your username must be more than 1 and less than 33 characters";
				}
				// Make sure they entered a correct captcha
				else if ( ! captcha_is_correct() )
				{
					$error = "Please re-enter the reCAPTCHA information";
				} 

				if ( $error )
				{
					error_me($error);
				}

				/*
				* If at this point then all is good and it's time to add the user
				*/

				$parms = array
				(
					'login'    => $_POST['username'],
					'email'    => $_POST['email'],
					'password' => $_POST['password'],
					'is_biz_owner' => 1,
				);

				$user->add($parms);

				// CC Tyler of any new signups
				if ( ! IS_LOCALHOST )
				{
					$message = "New TDS Signup - {$_POST['username']}/{$_POST['email']}";
					mail(SEND_LEAD_CC_TO,$message, $message);
				}

				// If this is a biz claim, then insert a link to this user
				if ( isset($TARGET->arg3) && is_numeric($TARGET->arg3) )
				{
					include_once "interfaces/biz/biz_link_to_user.inc";
					$biz_link_to_user = new biz_link_to_user;
					$biz_link_to_user->request($user->data->user_id,$TARGET->arg3);
				}

				// Send confirmation email
				if ( $this->_send_validation_email($_POST['username'],$mail_template='biz_register_verify') )
				{
					notify_redirect('/biz/join-success');
				}
				else
				{
					error_redirect('/biz/join-error');
				}

			}
		}

		/*
		* Re-Send validation email
		*/
		
		public function biz_resend_validation()
		{
			global $TARGET;
						
			if ( isset($_REQUEST['do_resend_validation']) )
			{
				if ( $this->_send_validation_email($_REQUEST['username'],$mail_template='biz_register_verify') )
				{
					notify_redirect('/biz/join-success');
				}
				else
				{
					notify_redirect('/biz/join-error');
				}
			}
			
		}

		/*
		* The action they end up at when clicking the confirm link
		*/
		
		public function biz_confirm()
		{
			global $TARGET;
			
			$this->v->confirm_status = 'success';
			
			$user = new user;
			
			// Test the link values show different errors based on that
			// Is this a valid confirm link?
			// Does the user exist in the DB?
			// Is the secret correct?
			// Is the user already confirmed?
			
			if ( !isset($TARGET->arg3) || !is_numeric($TARGET->arg3) || !isset($TARGET->arg4) || !is_numeric($TARGET->arg4))
			{
				$this->v->confirm_status = 'invalid_link_format';
			}
			else if ( ! $user->get($TARGET->arg3) )
			{
				$this->v->confirm_status = 'no_user';
			}
			else if ( $user->data->short_secret != trim($TARGET->arg4) )
			{
				$this->v->confirm_status = 'bad_secret';
			}
			else if ( $user->data->is_validated )
			{
				$this->v->confirm_status = 'already_validated';
			}
			
			if ( $this->v->confirm_status == 'success' )
			{
				$user->update_value($TARGET->arg3, 'is_validated', 1);
				$user->new_short_secret($TARGET->arg3);
			}
			
		}
		public function biz_payments()
		{
		}

		/*
		* Show biz_leads for  biz-owner
		*/

		public function biz_leads()
		{
			include "actions/admin/admin_billing.func";
			
			global $TARGET, $db, $BIZ_NAV;

			$this->_validate();

			set_page_title($BIZ_NAV['biz/leads']['page-title']);
			set_meta_desc($BIZ_NAV['biz/leads']['meta-desc']);
			$this->v->title = $BIZ_NAV['biz/leads']['content-title'];

			// Set date range to select leads by
			$this->v->range = isset($_REQUEST['range']) && is_numeric($_REQUEST['range']) ? $_REQUEST['range'] : 0;
			$RANGE_SQL      = get_range_sql($this->v->range);

			include "interfaces/biz/biz_leads.inc";
			
			$biz_lead = new biz_lead;

			$sql = 
				 "SELECT bl.*, b.name, b.street, b.city, b.state
				  FROM   biz_leads as bl 
				  JOIN biz_link_to_user as k ON k.biz_id = bl.biz_id
				  JOIN businesses as b ON k.biz_id = b.biz_id
				  WHERE  bl.is_approved = 1 
				  AND    k.user_id = " . myuid() ."
				  AND    $RANGE_SQL
 				  ORDER BY bl.creation_date";

			// Get all leads by type specified by argument3
			$this->v->leads= $db->get_results($sql);
			
			// Download leads to file my-leads.csv
			if ( isset($_GET['download']) && $_GET['download'] == 'true' )
			{
				header('Content-type: application/octet-stream');
				header('Content-Disposition: attachment; filename="my-leads.csv"');
				print "Date,First Name,Last Name,Email,Phone,Zip,Coupon,Listing,Street,City,State\n";
				foreach ( $this->v->leads as $lead)
				{
					print "\"{$lead->creation_date}\",\"{$lead->first_name}\",\"{$lead->last_name}\",\"{$lead->email}\",\"{$lead->phone}\",\"{$lead->zip}\",\"{$lead->coupon}\",\"{$lead->name}\",\"{$lead->street}\",\"{$lead->city}\",\"{$lead->state}\"\n";
				}

				exit;
			}
		}
		
		/*
		* Listings
		*/
		
		public function biz_listings()
		{
			global $TARGET, $db, $BIZ_NAV;
			
			$this->_validate();


			if ( isset($_GET['delete']) && is_numeric($_GET['delete']) 
				   &&
				   $db->get_var("SELECT count(*) from biz_link_to_user WHERE biz_link_to_user.biz_id = ".$_GET['delete']." AND biz_link_to_user.user_id =". myuid() )
		 	   )
			{
				include "interfaces/biz/biz_details.inc";

				$business = new business;
				$business->delete_business($_GET['delete']);
				//notify_redirect($success_action,$success_message);
			}

			// Get the main listign title from config.inc
			set_page_title($BIZ_NAV['biz/listings']['page-title']);
			set_meta_desc($BIZ_NAV['biz/listings']['meta-desc']);
			$this->v->title = $BIZ_NAV['biz/listings']['content-title'];
			
			$this->v->businesses = $db->get_results("SELECT businesses.*, biz_link_to_user.* FROM biz_link_to_user INNER JOIN businesses ON biz_link_to_user.biz_id = 	businesses.biz_id WHERE  biz_link_to_user.user_id =". myuid() );
			
		}

		/*
		* FAQ
		*/

		public function biz_faq()
		{
			global $BIZ_NAV;
			
			$this->_validate();

			set_page_title($BIZ_NAV['biz/faq']['page-title']);
			set_meta_desc($BIZ_NAV['biz/faq']['meta-desc']);
			$this->v->title = $BIZ_NAV['biz/faq']['content-title'];
		}

		/*
		* Customer Support
		*/

		public function biz_customer_support()
		{
			global $BIZ_NAV, $BIZ_SUPPORT_TOPICS;

			$this->_validate();

			set_page_title($BIZ_NAV['biz/customer-support']['page-title']);
			set_meta_desc($BIZ_NAV['biz/customer-support']['meta-desc']);
			$this->v->title = $BIZ_NAV['biz/customer-support']['content-title'];

			$this->v->question_topics = $BIZ_SUPPORT_TOPICS;
			
			if ( isset($_POST['do_biz_customer_support']) )
			{
				if ($_POST['question'] = trim($_POST['question']) )
				{

					// Send  email with question 
					include "interfaces/mail.inc";

					$msg_parms = array
					(
						'senders_email'      => $_SESSION['USER']['email'],
						'phone_number'       => $_SESSION['USER']['phone'],
						'senders_first_name' => $_SESSION['USER']['first_name'],
						'senders_last_name'  => $_SESSION['USER']['last_name'],
						'login_name'         => $_SESSION['USER']['login'],
						'support_topic'      => $_POST['topic']?$this->v->question_topics[$_POST['topic']]:'No Topic Selected',
						'question'           => $_POST['question']
					);
					send_mail_helper(SUPPORT_EMAIL,"template/mail/biz_customer_support.txt",$msg_parms);
										
					session_unregister('LAST_POST');
					notify_redirect('/biz/customer-support','Your support request has been sent.');
				}
			}
		}


		/*
		* Business Owner login
		*/

		public function biz_login()
		{
			
			$this->try_redirect_to_control_panel();

			/*
			* If this is a login
			*/
			
			if ( isset($_POST['do_login']) )
			{
				$user = new user;
				
				if ( ! isset($_POST['username']) || ! $username = trim($_POST['username']) )
				{
					error_me("Please enter a username or email");
				}
				else if ( ! isset($_POST['password']) || ! $password = trim($_POST['password']) )
				{
					error_me("Please enter a password");
				}
				else if ( ! $user->get($_POST['username']) || $error = is_error($user->validate_pass($_POST['username'], $_POST['password'])) )
				{
					if ( $error == 'not_validated' )
					{
						error_me("Please click the link in the validation email we sent you. <br/> <a href='/biz/resend-validation?rid={$user->data->user_id}'>Click here to resend validation email</a>");
					}
					else
					{
						error_me("We couldn't recognize those login details. Please try again.");
					}
				}
				// Login success
				else
				{
					session_establish((array) $user->data,isset($_POST['remember_me']));	

					if ( isset($_SESSION['RESTRICTED_ACTION']) )
					{
						$location = $_SESSION['RESTRICTED_ACTION'];
						session_unregister('RESTRICTED_ACTION');
						notify_redirect($location);
					}
					else
					{
						$this->try_redirect_to_control_panel();
					}			
				}		
			}
			
		}
		
		/*
		* If already logged in then pipe em to the correct control panel
		* depending on their account type
		*/
		
		public function try_redirect_to_control_panel()
		{
			
			if ( session_is_active() )
			{
				if ( session_is_admin() )
				{
					notify_redirect("/admin/home");
				}
				else if ( session_is_member() )
				{
					notify_redirect("/member/home");
				}
				else if ( session_is_biz_owner() )
				{
					notify_redirect("/biz/home");
				}
			}
		}
		
		/*
		* If a user forget thier password, here they can reclaim it
		*/
		
		public function biz_reset_password()
		{
			
			if ( isset($_POST['do_reset_password']) )
			{
				$user = new user;
				
				if ( ! isset($_POST['username']) || ! $username = trim($_POST['username']) )
				{
					error_me("Please enter a username or email");
				}
				else if ( ! $user->get($_POST['username']) )
				{
					error_me("We couldn't find a user with that username or email.");
				}
				// Login success
				else
				{					
					if ( $this->_send_validation_email($_POST['username'],$mail_template='password_reset',$link_action='reset-confirm') )
					{
					notify_redirect('/biz/reset-success');
				}
				else
				{
					notify_redirect('/biz/reset-error');
				}
				}
			}
		}

		/*
		* Sucess message shown after user clicks reset
		*/

		public function biz_reset_success()
		{
			// No controller code needed, just display the htm file
		}

		/*
		* Failure message shown after user clicks reset
		*/

		public function biz_reset_error()
		{
			// No controller code needed, just display the htm file
		}
		
		/*
		* The page that does the real password reset
		*/

		public function biz_reset_confirm()
		{
			global $TARGET;
			
			$this->v->confirm_status = 'show_form';
			
			$user = new user;

			if ( !isset($TARGET->arg3) || !is_numeric($TARGET->arg3) || !isset($TARGET->arg4) || !is_numeric($TARGET->arg4))
			{
				$this->v->confirm_status = 'invalid_link_format';
			}
			else if ( ! $user->get($TARGET->arg3) )
			{
				$this->v->confirm_status = 'no_user';
			}
			else if ( $user->data->short_secret != trim($TARGET->arg4) )
			{
				$this->v->confirm_status = 'bad_secret';
			}
			
			if ( isset($_POST['do_reset_password']) && $this->v->confirm_status == 'show_form' )
			{
				$user = new user;
				
				if ( ! isset($_POST['password']) || ! $password = trim($_POST['password']) )
				{
					error_me("Please enter a new password");
				}
				else if ( strlen($_POST['password']) < 6 || strlen($_POST['password']) > 30 )
				{
					error_me("Your new password must be greater than 5 and less than 31 characters");
				}				
				else
				{
					$user->update_pass($TARGET->arg3, $password);
					$this->v->confirm_status = 'success';
				}
			}

		}

		
		/*
		* biz_edit_details
		*/
		
		public function biz_password()
		{
			global $TARGET, $BIZ_NAV;
			
			$this->_validate();
						
			set_page_title($BIZ_NAV['biz/password']['page-title']);
			set_meta_desc($BIZ_NAV['biz/password']['meta-desc']);
			$this->v->title = $BIZ_NAV['biz/password']['content-title'];

			$user = new user(myuid());

			if ( isset($_POST['do_change_password']) )
			{
				if ( ! $old_password = trim($_POST['old_password']) )
				{
					error_me("Please enter your old password");
				}
				else if ( ! $new_password = trim($_POST['new_password']) )
				{
					error_me("Please enter your new password");
				}
				else if ( strlen($new_password) < 6 || strlen($new_password) > 30 )
				{
					error_me("Your new password must be greater than 5 and less than 31 characters");
				}				
				else if ( is_error($user->validate_pass(myuid(), $old_password)) )
				{
					error_me("We could not verify your old password. Please try again.");
				}
				else
				{
					$user->update_pass(myuid(), $new_password);
					notify_me("Your password was updated successfully.");
				}

			}
		
		}

		
		/*
		* biz_home
		*/

		public function biz_home()
		{
			global $TARGET, $BIZ_NAV, $db;
			
			$this->_validate();
			
			set_page_title($BIZ_NAV['biz/home']['page-title']);
			set_meta_desc($BIZ_NAV['biz/home']['meta-desc']);
			$this->v->title = $BIZ_NAV['biz/home']['content-title'];

			// If new_user && biz link show next status info screen
			if ( $this->v->i_have_an_approved_biz === '0' )
			{
				set_view('biz_home_awaiting_approval');
			}
			// If approved show normal home page
			else
			{
				set_view('biz_home_approved');
				
				$stats = array 
				(
					// Todo 
					'num_leads'    => "biz_leads bl, biz_link_to_user blt WHERE bl.is_approved = 1 AND bl.is_done = 0 AND blt.biz_id=bl.biz_id AND blt.is_linked=1 AND blt.user_id=".myuid(),
					'num_listings' => "biz_link_to_user WHERE is_linked = 1 AND user_id = ".myuid(),
				);
				
				foreach ($stats as $field => $sql)
				{
					foreach ($stats as $field => $sql)
					{
						$this->v->stats[$field] = $db->get_var("SELECT count(*) FROM ".$sql);
					}
				}
			}

		}

		
		/*
		* Make sure it's an active admin session
		*/
		
		private function _validate()
		{
			global $TARGET, $db;
			
			if ( ! session_is_biz_owner() )
			{
				$_SESSION['RESTRICTED_ACTION'] = $_SERVER['REQUEST_URI'];				
				notify_redirect('/biz/login');
			}

			$this->v->i_have_an_approved_biz = $db->get_var("SELECT is_linked AS i_have_an_approved_biz FROM biz_link_to_user WHERE user_id  = ". myuid());

			// If they have claimed a business but are un-approved
			if ( $this->v->i_have_an_approved_biz === '0' && $TARGET->controller->method != 'biz_home' && $_SESSION['USER']['first_name'] )
			{
				notify_redirect('/biz/home');
			}
			// If they have not entered account info then
			// ensure they do under all circomstances
			else if ( ( ! $_SESSION['USER']['first_name'] || ! $_SESSION['USER']['pp_last_tid'] ) && $TARGET->controller->method != 'biz_account_info' )
			{
				notify_redirect('/biz/account-info');
			}
			else if ( ! $_SESSION['USER']['first_name'] && $TARGET->controller->method == 'biz_account_info'  )
			{
				;	
			}
			// If they have not added, or claimed a business
			else if ( $this->v->i_have_an_approved_biz !== '0' && $_SESSION['USER']['pp_last_tid'] && ! $this->v->i_have_an_approved_biz && $TARGET->controller->method != 'biz_add_listing' )
			{
				notify_redirect('/biz/add-listing');
			}
			else if ( $this->v->i_have_an_approved_biz !== '0' && $TARGET->controller->method != 'biz_add_listing' && $biz = $db->get_row("SELECT * FROM biz_link_to_user AS bl JOIN businesses AS b ON b.biz_id=bl.biz_id WHERE bl.user_id  = ". myuid()." AND (description IS NULL OR description = '')") )
			{
				notify_redirect("/biz/add-listing/{$biz->biz_id}");
			}
			
		}

		private function _update_credit_card()
		{

			$_POST['card_number'] = str_replace(' ','',$_POST['card_number']);
			
			$CC_PATTERNS = array
			(
				'amex'       => "/^([34|37]{2})([0-9]{13})$/",
				'diners'     => "/^([30|36|38]{2})([0-9]{12})$/",
				'discover'   => "/^([6011]{4})([0-9]{12})$/",
				'mastercard' => "/^([51|52|53|54|55]{2})([0-9]{14})$/",
				'visa'       => "/^([4]{1})([0-9]{12,15})$/"
			);
			
			$CSV_PATTERNS = array
			(
				'amex'       => "/^\d\d\d\d$/",
				'diners'     => "/^\d\d\d$/",
				'discover'   => "/^\d\d\d$/",
				'mastercard' => "/^\d\d\d$/",
				'visa'       => "/^\d\d\d$/"
			);
			
			// Make sure the credit card is correct format
			if ( ! preg_match($CC_PATTERNS[$_POST['card_type']],$_POST['card_number']) )
			{
				error_me('Please enter a valid credit card number');
			}

			if ( ! preg_match('/^\d\d$/',$_POST['exp_month']) )
			{
				error_me('Please enter a valid expiry month (format MM)');
			}
			
			if ( ! preg_match('/^\d\d\d\d$/',$_POST['exp_year']) )
			{
				error_me('Please enter a valid expiry year (format YYYY)');
			}
			
			if ( ! preg_match($CSV_PATTERNS[$_POST['card_type']],$_POST['csv']) )
			{
				error_me('Please enter a valid CSV number');
			}
			
			if ( ! trim($_POST['card_name']) )
			{
				error_me('Please enter the name shown on your card');
			}

			$card_name  = explode(' ',$_POST['card_name']);
			$first_name = $card_name[0];
			if ( ! isset($card_name[1]) )
			{
				error_me('Please enter the last name name shown on your card');
			}
			$last_name = $card_name[1];
			if ( isset($card_name[2]) )
			{
				$last_name .= ' '. $card_name[2];
			}

			// Now check the credit card with the real paypal system just in case there's an error
			include "interfaces/paypal.inc";
			$paypal = new paypal();
			$cc_parms = array
			(
				'user_id'     => myuid(),
				'card_type'   => ucfirst($_POST['card_type']),
				'first_name'  => $first_name,
				'last_name'   => $last_name,
				'card_number' => $_POST['card_number'],
				'exp_month'   => $_POST['exp_month'],
				'exp_year'    => $_POST['exp_year'],
				'csv'         => $_POST['csv'],
			);
			
			if ( $error = is_error($paypal->add_card($cc_parms)) )
			{
				error_me($error);	
			}
			
			// End cc check

		}

		/*
		* Biz Account Info
		*/
		
		public function biz_account_info()
		{
			$this->_validate();

			$this->v->form_fields = array
			(
				'first_name'      => 'Please enter your First Name',
				'last_name'       => 'Please enter your Last Name',
				'biz_name'        => 'Please enter your Business Name',
				'biz_position'    => 'Please enter your Position / Title',
				'phone'           => 'Please enter your Phone',
				'email'           => 'Please enter your Email',
			);

			// If first name is empty then assume its first time user
			$this->v->is_first_login = $_SESSION['USER']['first_name'] ? false : true;
			
			// Always get my data from the DB
			$user = new user(myuid());

			// Empty parms array used for update if no errors
			$parms = array();

			// This takes form input and updates the DB
			if ( isset($_POST['do_update']) )
			{
	
				$fields = $this->v->form_fields;
				unset($fields['email']);

				foreach ( $fields as $field => $field_label )
				{
					if ($field != 'on_mailing_list' && ( !isset($_POST[$field]) || !	$_POST[$field] = trim($_POST[$field]) ) )
					{
						error_me($field_label);	
					}
					$parms[$field] = $_POST[$field];
				}

				// This is the update
				$user->update(myuid(),$parms);
				
				// We also need to update the session values now
				// (Note: These are checked to see if its a first time user)
				foreach ( $fields as $field => $field_label )
				{
					$_SESSION['USER'][$field] = $_POST[$field];
				}

				// If credit card update
				if ( isset($_POST['do_update_cc']) )
				{
					$this->_update_credit_card();
				}
				
				notify_me('Your details have been updated successfully');
			}

			if ( ! $_POST )
			{
				$_POST = (array) $user->data;
				$this->v->is_post = false;
				
				if ( isset($_GET['change_cc']) )
				{
					$_POST['card_number'] = '';
				}
				
			}
			else
			{
				$_POST['card_type']   = $user->data->card_type;
				
				if ( ! defined('ERROR_STR') )
				{
					$_POST['card_number'] = $user->data->card_number;
				}
				$_POST['email'] = $user->data->email;
				$this->v->is_post = true;
			}
		}

		/*
		* Send validation email
		*/
		
		private function _send_validation_email($login,$mail_template='biz_register_verify',$link_action='confirm')
		{

			global $APP_DOMAIN;

			$user = new user;

			if ( $user->get($login) )
			{

				include "interfaces/mail.inc";
	
				$msg_parms = array
				(
					'link' => "http://$APP_DOMAIN/biz/{$link_action}/{$user->data->user_id}/{$user->data->short_secret}",
				);

				// For use in future pages
				$_SESSION['MAILED_USER'] = $user->data;

				send_mail_helper($user->data->email,"template/mail/{$mail_template}.txt",$msg_parms);

				return true;

			}
			
			return false;
		
		}
		
		/*
		* Main Add Listing Function
		*/

		public function biz_add_listing()
		{
			global $TARGET;

			$this->_validate();
			
			// Creates a session object based on db row object that is used accross tabs. 
			$this->_add_listing_init_session_obj();
			
			// Converts the session object values to be used in the views
			// i.e $__name, $__street etc.
			$this->_add_listing_load_session_to_view();
	
			// This works out what function & view should be used for the
			// current tab
			if ( ! isset($TARGET->arg3) )
			{
				notify_redirect('/biz/add-listing/biz-info?new');
			}
			else if ( isset($TARGET->arg3) && $TARGET->arg3 == 'biz-info' )
			{
				$this->_add_listing_biz_info();
				set_view('biz_add_listing_biz_info');
			}
			else if ( isset($TARGET->arg3) && $TARGET->arg3 == 'biz-coupons' )
			{
				$this->_add_listing_biz_coupons();
				set_view('biz_add_listing_biz_coupons');
			}
			else if ( isset($TARGET->arg3) && $TARGET->arg3 == 'biz-photos' )
			{
				$this->_add_listing_biz_photos();
				set_view('biz_add_listing_biz_photos');
			}

		}
	
		/*
		* Add Listing -> Business Info
		*/
		
		private function _add_listing_biz_info()
		{
			if (isset($_GET['new']))
			{
				$_SESSION['new_biz'] = true;
			}
			else
			{
				unset($_SESSION['new_biz']);
			}
			if ( isset($_POST['do_save']) || isset($_POST['do_next']) )
			{

				$this->_convert_post_to_session_obj_and_write_to_db();
				
				if ( isset($_POST['do_save']) )
				{
					notify_me('Details updated successfully');
				}
				
				if ( isset($_POST['do_next']) )
				{
					notify_redirect('/biz/add-listing/biz-photos');
				}
			}
		}
		
		/*
		* Add Listing -> Photos
		*/
		
		private function _add_listing_biz_photos()
		{
			global $db;
	
			include "interfaces/biz/biz_pics.inc";
			$biz_pic = new biz_pic;

			if ( isset($_GET['delete_pic']) && is_numeric($_GET['delete_pic']) )
			{
				$biz_pic->remove($_SESSION['BIZ_LISTING']['biz_id'],$_GET['delete_pic']);
			}

			/*
			* Dealing with picklist saving and getting
			*/

			$picklist_name = $_SESSION['BIZ_LISTING']['type']."/amenities";
			
			// Get all the amenities fro this typ eof business
			include "interfaces/picklist.inc";
			$picklist_admin = new picklist_admin($picklist_name);
			$this->v->amenities = $picklist_admin->get_all_incl_pick_val_id();

			$n = count($this->v->amenities); 
			$column_size = floor($n/4);

			for ($i=0;$i<4;$i++)
			{
				$this->v->rows[$i] = $column_size;
			}
			for ($i=0;$i<($n % 4);$i++)
			{
				$this->v->rows[$i] = $column_size + 1;
			}

			// This now writes the selected ameneties from the post
			$picklist = new picklist($picklist_name);
			if ( isset($_POST['do_save']) || isset($_POST['do_next']) || isset($_POST['do_previous']) )
			{
				$picklist->delete_all($_SESSION['BIZ_LISTING']['biz_id']);

				foreach ( $this->v->amenities as $pick )
				{
					if ( isset($_POST['amenity'][$pick->pick_val_id]) )
					{
						$picklist->assign_val($pick->pick_val_id,$_SESSION['BIZ_LISTING']['biz_id']);
					}
				}
			}
			
			if (isset($_SESSION['BIZ_LISTING']['biz_id']))
			{
				$this->v->my_picks = $picklist->get($_SESSION['BIZ_LISTING']['biz_id']);
			}

			/*
			* All the other save stuff
			*/

			if ( isset($_POST['do_save']) || isset($_POST['do_next']) || isset($_POST['do_previous']) )
			{

				$this->_convert_post_to_session_obj_and_write_to_db();
				
				if ( isset($_POST['do_save']) )
				{
					notify_me('Details updated successfully');
				}
				
				// At end of this section function move to next screen
				if ( isset($_POST['do_next']) )
				{
					notify_redirect('/biz/add-listing/biz-coupons');
				}

				// Move to previous screen
				if ( isset($_POST['do_previous']) )
				{
					notify_redirect('/biz/add-listing/biz-info');
				}

			}

			// Get existing logo (if there is one!)
			if (isset($_SESSION['BIZ_LISTING']['biz_id']))
			{
				$this->v->logo_path = $biz_pic->get_logo_path($_SESSION['BIZ_LISTING']['biz_id']);
				$this->v->biz_pics = $biz_pic->get_image_paths($_SESSION['BIZ_LISTING']['biz_id']);
			}
			
			// Get the default logos
			foreach ( dir_to_array('data/logos') as $file )
			{
				$this->v->logos[] = '/data/logos/'.$file;
			}

		}
		
		/*
		* Add Listing -> Coupons & Discounts
		*/
		
		private function _add_listing_biz_coupons()
		{
			global $db;
			
			if ( isset($_POST['do_save']) || isset($_POST['do_next']) || isset($_POST['do_previous']) || isset($_POST['do_finish']) )
			{
				$this->_convert_post_to_session_obj_and_write_to_db();

				// If this is a new business and it's do_save, or 
				// do_finish then it's time to insert a link into this business
				if ( ! $db->get_var("SELECT count(*) FROM biz_link_to_user WHERE user_id = ".myuid()." AND biz_id = ".$_SESSION['BIZ_LISTING']['biz_id']) && ( isset($_POST['do_save']) || isset($_POST['do_finish']) ) )
				{
					// Insert link to this business now!!!
					include "interfaces/biz/biz_link_to_user.inc";
					$biz_link_to_user = new biz_link_to_user;
					$biz_link_to_user->request(myuid(),$_SESSION['BIZ_LISTING']['biz_id']);
					$db->query("UPDATE businesses SET creator_id = 0 WHERE biz_id = " . $_SESSION['BIZ_LISTING']['biz_id']);
					notify_redirect('/biz/home',"Thanks for adding your listing. It will be enabled shortly.");
				}

				if ( isset($_POST['do_save']) )
				{
					notify_me('Details updated successfully');
				}
				
				// At end of this section function move to next screen
				if ( isset($_POST['do_finish']) )
				{
					notify_redirect('/biz/home');
				}

				// Move to previous screen
				if ( isset($_POST['do_previous']) )
				{
					notify_redirect('/biz/add-listing/biz-photos');
				}
			}

			$this->v->coupons = $this->_get_coupons();

		}

		/*
		* This gets possible coupon types for this listing
		*/

		private function _get_coupons()
		{
			if (isset($_SESSION['BIZ_LISTING']['type']) && $_SESSION['BIZ_LISTING']['type'])
			{
				include "interfaces/picklist.inc";
				$picklist_admin = new picklist_admin($_SESSION['BIZ_LISTING']['type']."/coupons");			
				
				$coupons = array();
				foreach ( $picklist_admin->get_all_incl_pick_val_id() as $item )
				{
					$coupons[$item->pick_val_id] = $item->value;
				}
			}
			
			return $coupons;
		}
		
		/*
		* Add Listing -> Business Info
		*/
		
		private function _add_listing_init_session_obj()
		{
			global $TARGET, $db;
			
			$ignore_fields = array
			(
				'search_bucket',
				'date_updated',
				'geocode_status',
				'latitude',
				'longitude',
			);

			// If EDITING an existing record
			if ( isset($TARGET->arg3) && is_numeric($TARGET->arg3) )
			{
				// If there is an edit ID then set the biz listing session obj
				// to become values taken from the database
				// If cant find the biz, then just redirect to add
				if ( ! $biz = $db->get_row("SELECT * FROM businesses WHERE biz_id = {$TARGET->arg3}",ARRAY_A) )
				{
					notify_redirect('/biz/add-listing/biz-info');
				}
				
				foreach ( $ignore_fields as $field )
				{
					unset($biz[$field]);
				}

				$_SESSION['BIZ_LISTING'] = $biz;
				
				notify_redirect('/biz/add-listing/biz-info');
				
			}

			// If ADDING a new record (or in the middle of adding a new one)
			if ( ! isset($_SESSION['BIZ_LISTING']) || isset($_GET['new']) )
			{
				
				// But first, lets see if we were previously editing a business?!
				if ( ! $biz = $db->get_row("SELECT * FROM businesses WHERE creator_id = ". myuid(),ARRAY_A) )
				{
					$biz = $db->get_row("SELECT * FROM businesses WHERE biz_id = 1",ARRAY_A);
				}
				
				foreach ( $ignore_fields as $field )
				{
					unset($biz[$field]);
				}

				$_SESSION['BIZ_LISTING'] = $biz;
			}

		}
		
		/*
		* Loads values from the initialised session nobject into the view
		*/
		
		private function _add_listing_load_session_to_view()
		{
			// Load the values from the session into view variables
			foreach ( $_SESSION['BIZ_LISTING'] as $var => $val )
			{
				$this->v->{$var} = $val;
			}
		}
		
		/*
		* Try to save the logo (if were on the right screen)
		*/
		
		private function _try_to_save_logo()
		{

			$STRINGS = array
			(
				'missing_biz_id'   => 'Unexpected error: missing_biz_id',
				'missing_pic_type' => 'Unexpected error: missing_pic_type',
				'missing_width'    => 'Unexpected error: missing_width',
				'missing_height'   => 'Unexpected error: missing_height',
				'missing_src_path' => 'Unexpected error: missing_src_path',
				'missing_mime'     => 'Unexpected error: missing_mime',
				'invalid_biz_id'   => 'Unexpected error: invalid_biz_id',
				'invalid_width'    => 'Unexpected error: invalid_width',
				'invalid_height'   => 'Unexpected error: invalid_height',
				'invalid_pic_type' => 'Unexpected error: invalid_pic_type',
				'file_not_found'   => 'Unexpected error: file_not_found',
				'file_is_too_big'  => 'The logo image file was too big. Please try a smaller image.',
			);
			
			$parms = false;
			
			// If they selected a stock pic then add it!
			if ( isset($_POST['stock-pic']) && $_POST['stock-pic'] )
			{
				$parms = array
				(
					'biz_id' => $_SESSION['BIZ_LISTING']['biz_id'],
					'pic_type' => 'logo',
					'img_src_path' => get_regex('/(data.*)/',$_POST['stock-pic']),
					'mime_type' => get_regex('/(jpeg|jpg|gif|png)/',$_POST['stock-pic'])
				);
			}
			else
			{
				return;
			}
			/*
			else if ( isset($_FILES['logo-up-file']) && $_FILES['logo-up-file']['size'] > 0 )
			{
				
				$parms = array
				(
					'biz_id' => $_SESSION['BIZ_LISTING']['biz_id'],
					'pic_type' => 'logo',
					'img_src_path' => $_FILES['logo-up-file']['tmp_name'],
					'mime_type' => $_FILES['logo-up-file']['type']
				);
			}
			else if ( $_FILES['logo-up-file']['size']  <= 0 || $_FILES['logo-up-file']['size']  > MAX_PIC_UPLOAD_SIZE)
			{
					error_me($STRINGS['file_is_too_big']);
			}
			 */

			if ( $parms )
			{
				$biz_pic = new biz_pic;
				$biz_pic->remove_logo($_SESSION['BIZ_LISTING']['biz_id']);
				if ( $error = is_error($biz_pic->store($parms)) )
				{
					error_me($STRINGS[$error]);
				}
			}

		}
		
		/*
		* Try to save pics for this listing
		*/
		
		private function _try_to_save_pics()
		{
			$STRINGS = array
			(
				'missing_biz_id'   => 'Unexpected error: missing_biz_id',
				'missing_pic_type' => 'Unexpected error: missing_pic_type',
				'missing_width'    => 'Unexpected error: missing_width',
				'missing_height'   => 'Unexpected error: missing_height',
				'missing_src_path' => 'Unexpected error: missing_src_path',
				'missing_mime'     => 'Unexpected error: missing_mime',
				'invalid_biz_id'   => 'Unexpected error: invalid_biz_id',
				'invalid_width'    => 'Unexpected error: invalid_width',
				'invalid_height'   => 'Unexpected error: invalid_height',
				'invalid_pic_type' => 'Unexpected error: invalid_pic_type',
				'file_not_found'   => 'Unexpected error: file_not_found',
				'file_is_too_big'  => 'An image file was too big. Please try a smaller image.',
			);
			
			// If they selected a stock pic then add it!
			if ( isset($_FILES['file1']) )
			{
				
				$biz_pic = new biz_pic;
				
				if ( $biz_pic->get_num($_SESSION['BIZ_LISTING']['biz_id']) >= BIZ_PICS_MAX_NUM_PER_LISTING )
				{
					error_me("The maximum number of images allowed is " . BIZ_PICS_MAX_NUM_PER_LISTING );
				}
				
				for ( $i=1; true; $i++ )
				{
					
					if ( ! isset($_FILES['file1']['tmp_name'][$i]) )
					{
						break;	
					}

					if ( $_FILES['file1']['size'][$i] > 0 )
					{	
						$parms = array
						(
							'biz_id' => $_SESSION['BIZ_LISTING']['biz_id'],
							'pic_type' => 'listing',
							'img_src_path' => $_FILES['file1']['tmp_name'][$i],
							'mime_type' => $_FILES['file1']['type'][$i]
						);
						
						if ( $error = is_error($biz_pic->store($parms)) )
						{
							error_me($STRINGS[$error]);
						}
					}
					
				} 
			}
			
		}
		
		/*
		* Convert anything that comes in on the post and writes
		* it to the DB
		*/

		private function _convert_post_to_session_obj_and_write_to_db()
		{
			global $db;

			$STRINGS = array
			(
				'missing_type'             => 'Please select a business type',
				'missing_name'             => 'Please enter your business name',
				'missing_street'           => 'Please enter a street address',
				'missing_city'             => 'Please enter a city',
				'missing_state'            => 'Please enter a state',
				'missing_zip'              => 'Please enter a zip',
				'missing_description'      => 'Please enter a description',
				'invalid_zip'              => 'Please enter a valid zip',
				'invalid_cost_membership'  => 'Please enter a valid membership cost',
				'invalid_cost_guest'       => 'Please enter a valid guest cost',
				'invalid_cost_to_join'     => 'Please enter a valid cost to join',
				'invalid_cost_membership_timeframe' => 'Please select a membership timeframe',
				'invalid_cost_guest_timeframe' => 'Please select a guest timeframe',
			);

			include "interfaces/biz/biz_details.inc";

			// Cant name an HTML text input "name"
			if ( isset($_POST['biz_name']) )
			{
				$_POST['name'] = str_replace('/','-',$_POST['biz_name']);
			}

			foreach ( $_POST as $k => $v )
			{
				if ( array_key_exists($k,$_SESSION['BIZ_LISTING']) )
				{
					@$_SESSION['BIZ_LISTING'][$k] = $_POST[$k];				
				}
			}

			// This unsets the biz ID for a new entry
			if ( array_key_exists('biz_id',$_SESSION['BIZ_LISTING']) &&  $_SESSION['BIZ_LISTING']['biz_id'] == 1 || $_SESSION['BIZ_LISTING']['biz_id'] == '' )
			{
				unset($_SESSION['BIZ_LISTING']['biz_id']);

				// This sets me as the creator of this entry. It's so that
				// I can pick it back up again in case of browser failure
				$_SESSION['BIZ_LISTING']['creator_id'] = myuid();
			}
			
			// If the coupon type is not set then use a default value
			if ( ! $_SESSION['BIZ_LISTING']['coupon_id'] )
			{
				if ( $coupons = $this->_get_coupons() )
				{
					foreach ( $coupons as $id => $text )
					{
						$_SESSION['BIZ_LISTING']['coupon_id'] = $id;
						break;
					}
				}
			}

			// Write the session object back to the DB
			$business = new business;
			if ( $error = is_error($biz_id = $business->add_or_update($_SESSION['BIZ_LISTING'])) )
			{
				error_redirect('/biz/add-listing/biz-info',$STRINGS[$error]);
			}

			// Always get the new biz ID from the update function
			$_SESSION['BIZ_LISTING']['biz_id'] = $biz_id;
			
			$this->_try_to_save_logo();
			//$this->_try_to_save_pics();

		}
		
		public function biz_billing()
		{
			
			global $db;
			
			$this->_validate();
			
			$account = $db->get_row("SELECT * FROM users WHERE user_id = ".myuid() );
			
			/*
			* Dealing with results query
			*/

			// Set date range to select leads by
			$this->v->range   = isset($_REQUEST['range']) && is_numeric($_REQUEST['range']) ? $_REQUEST['range'] : 0;
			$RANGE_SQL        = get_range_sql($this->v->range);
			
			$this->v->lead_total = 0;
			$this->v->leads = '';
			
			
			// Now, for display, we only want with full filter (including letter)
			if ( $results = $db->get_results("SELECT bl.credit_lead, bl.lead_id, b.name, bl.lead_rate, bl.first_name, bl.last_name, bl.phone, bl.email, bl.zip, bl.coupon, bl.creation_date date, bl.biz_id, b.street biz_street, b.city biz_city, b.state biz_state, b.zip biz_zip FROM biz_leads bl LEFT JOIN businesses b ON bl.biz_id=b.biz_id LEFT JOIN biz_link_to_user k ON bl.biz_id=k.biz_id LEFT JOIN users u ON k.user_id=u.user_id WHERE k.user_id=".myuid()." AND bl.is_approved = 1 AND u.user_id is NOT NULL AND $RANGE_SQL ORDER BY b.name ASC, bl.lead_id DESC") )
			{
				$leads = array();

				foreach ($results as $result)
				{
					if ( ! $result->credit_lead )
					{
						$this->v->lead_total += $result->lead_rate;
					}
					$leads[$result->biz_id][] = $result;
				}

				$this->v->leads = $leads;
				
				
				if ( isset($_REQUEST['invoice']) && $this->v->leads )
				{
				
					//<b> <i> <u> (as well as <strong> and <em>)
					//<a href="...">
					//<p> <br> (<tr> and <blockquote> treated like <br>)
					//<img src="..." width="..." [height="..."]>
					//<font face="..." color="..."> 
	
					$HTML  = "<img src=\"template/gfx/healthclub-logo.jpg\" width=\"180\">";
					$HTML .= "<br><br><br><br>";
					$HTML .= "<font color=\"#999999\">Invoice For Leads: ".date('F Y',strtotime("{$this->v->range} month"))."</font><br>";

					$total  = 0;
					$credit = 0;

					foreach ( $this->v->leads as $listing )
					{
						$HTML .= "<br>";
						$HTML .= "{$listing[0]->name}<br>";
						
						$alt    = 1;

						foreach ( $listing as $item )
						{
							$HTML .= "<font color=\"#999999\">ID:{$item->lead_id} - ".date('m/d/Y',strtotime($item->date)).", {$item->first_name} {$item->last_name}, {$item->email}, {$item->phone}, {$item->zip}, {$item->coupon}, ". ($item->credit_lead ? 'Lead cost: (no charge)' : 'Lead cost: $'.$item->lead_rate) . "</font><br>";
						
							if ( $item->credit_lead )
							{
								$credit += $item->lead_rate;
							}
							else
							{
								$total+=$item->lead_rate;
							}
						
						}
							
					}

					$HTML .= "<br><b>Total:</b> $". number_format($total,2);


					include 'third_party/fpdf/html2pdf.php';		

					$pdf=new PDF_HTML();
					$pdf->SetFont('Arial','',12);
					$pdf->AddPage();
					$pdf->WriteHTML($HTML);
					$pdf->Output();
					exit;

				}

				
				/*
				* This deals with the CSV export
				*/

				if ( isset($_REQUEST['export']) && $this->v->leads )
				{

					header('Content-type: application/octet-stream');
					header('Content-Disposition: attachment; filename="leads.csv"');

					$keys = array();
					
					foreach ( $this->v->leads as $listing )
					{
						foreach ( $listing as $idx => $lead )
						{
							$cols = '';
							$vals = '';
							
							foreach ( get_object_vars($lead) as $k => $v )
							{
								
								if ( $k == 'credit_lead' ) continue;
								if ( $k == 'user_id' ) continue;
								if ( $k == 'name' ) $k = 'listing';
								
								$k = ucwords(str_replace('_',' ',$k));
								$cols .=	"$k, ";
								$vals .=	"$v, ";
								$keys[]=$k;
							}
							
							if ( ! isset($done_header) )
							{
								print substr($cols,0,-2)."\n";
								$done_header = true;
							}
							print substr($vals,0,-2)."\n";
							
						}
					}

					exit;
				}

			}
						
		}
		
	}

?>

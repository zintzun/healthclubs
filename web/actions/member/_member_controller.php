<?php

	/*
	* Dealing with all member related functions for this website
	*/

	class member_controller
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
		
		public function member_index()
		{
			if ( session_is_active() )
			{
				$this->try_redirect_to_control_panel();
			}
			else
			{
				notify_redirect('/member/login');	
			}
		}
		
		/*
		* Member join page
		*/

		public function member_join()
		{
			
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
				);

				$user->add($parms);

				// Send confirmation email
				if ( $this->_send_validation_email($_POST['username']) )
				{
					notify_redirect('/member/join-success');
				}
				else
				{
					error_redirect('/member/join-error');
				}

			}
		}
		
		/*
		* Send validation email
		*/
		
		private function _send_validation_email($login,$mail_template='register_verify',$link_action='confirm')
		{

			global $APP_DOMAIN;

			$user = new user;

			if ( $user->get($login) )
			{

				include "interfaces/mail.inc";
	
				$msg_parms = array
				(
					'link' => "http://$APP_DOMAIN/member/{$link_action}/{$user->data->user_id}/{$user->data->short_secret}",
				);

				// For use in future pages
				$_SESSION['MAILED_USER'] = $user->data;

				send_mail_helper($user->data->email,"template/mail/{$mail_template}.txt",$msg_parms);

				return true;

			}
			
			return false;
		
		}
		
		/*
		* Re-Send validation email
		*/
		
		public function member_resend_validation()
		{
			global $TARGET;
						
			if ( isset($_REQUEST['rid']) )
			{
				if ( $this->_send_validation_email($_REQUEST['rid']) )
				{
					notify_redirect('/member/join-success');
				}
			}
			
			notify_redirect('/member/join-error');
		}

		/*
		* Sucess message shown after user clicks join
		*/

		public function member_join_success()
		{
			// No controller code needed, just display the htm file
		}

		/*
		* Failure message shown after user clicks join
		*/

		public function member_join_error()
		{
			// No controller code needed, just display the htm file
		}
		
		/*
		* The action they end up at when clicking the confirm link
		*/
		
		public function member_confirm()
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
		
		/*
		* This does the actual login
		*/
		
		public function member_login()
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
						error_me("Please click the link in the validation email we sent you. <br/> <a href='/member/resend-validation?rid={$user->data->user_id}'>Click here to resend validation email</a>");
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
					$this->try_redirect_to_control_panel();
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
		
		public function member_reset_password()
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
					notify_redirect('/member/reset-success');
				}
				else
				{
					notify_redirect('/member/reset-error');
				}
				}
			}
		}

		/*
		* Sucess message shown after user clicks reset
		*/

		public function member_reset_success()
		{
			// No controller code needed, just display the htm file
		}

		/*
		* Failure message shown after user clicks reset
		*/

		public function member_reset_error()
		{
			// No controller code needed, just display the htm file
		}
		
		/*
		* The page that does the real password reset
		*/

		public function member_reset_confirm()
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
		* member_bookmarks
		*/
		
		public function member_bookmarks()
		{
			global $TARGET, $MEMBER_NAV;
			
			$this->_validate();
			
			set_page_title($MEMBER_NAV['member/bookmarks']['page-title']);
			set_meta_desc($MEMBER_NAV['member/bookmarks']['meta-desc']);
			$this->v->title = $MEMBER_NAV['member/bookmarks']['content-title'];
			
			include "interfaces/biz/biz_bookmarks.inc";
			include "lib/state_utils.inc";
			
			$bookmarks = new biz_bookmark;
			
			$this->v->bookmarks = false;

			if ( ! is_error($results = $bookmarks->get_by_user(myuid()) ) )
			{
				$this->v->bookmarks = $results;
			}

			/*
			* Delete a bookmark. Get[Delete] field is the user_id-biz_id
			*/
			if ( isset($_GET['delete']))
			{
				list($user_id, $biz_id) = explode("-", $_GET['delete']);
				
				if ( is_numeric($user_id) && is_numeric($biz_id) )
				{
					$bookmarks->remove($user_id,$biz_id);
					notify_redirect('/'.$TARGET->action_orig);
				}
			}

		}
		
		/*
		* member_reviews
		*/
		
		public function member_reviews()
		{
			global $TARGET, $MEMBER_NAV;
			
			$this->_validate();
						
			set_page_title($MEMBER_NAV['member/reviews']['page-title']);
			set_meta_desc($MEMBER_NAV['member/reviews']['meta-desc']);
			$this->v->title = $MEMBER_NAV['member/reviews']['content-title'];
			
			include "interfaces/biz/biz_reviews.inc";
			include "lib/state_utils.inc";
			
			$reviews = new review;

			$this->v->reviews = false;

			if ( ! is_error($results = $reviews->get_by_user_id(myuid()) ) )
			{
				$this->v->reviews = $results;
			}

		}
		
		/*
		* member_edit_details
		*/
		
		public function member_password()
		{
			global $TARGET, $MEMBER_NAV;
			
			$this->_validate();
						
			set_page_title($MEMBER_NAV['member/password']['page-title']);
			set_meta_desc($MEMBER_NAV['member/password']['meta-desc']);
			$this->v->title = $MEMBER_NAV['member/password']['content-title'];

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
		* delete_account
		*/
		
		public function member_delete_account()
		{
			$this->_validate();
		}

		/*
		* member_home
		*/

		public function member_home()
		{
			global $TARGET, $MEMBER_NAV;
			
			$this->_validate();
			
			set_page_title($MEMBER_NAV['member/home']['page-title']);
			set_meta_desc($MEMBER_NAV['member/home']['meta-desc']);
			$this->v->title = $MEMBER_NAV['member/home']['content-title'];

		}

		/*
		* member_newsletter
		*/

		public function member_newsletter()
		{
			global $TARGET, $MEMBER_NAV;
			
			$this->_validate();
						
			set_page_title($MEMBER_NAV['member/newsletter']['page-title']);
			set_meta_desc($MEMBER_NAV['member/newsletter']['meta-desc']);
			$this->v->title = $MEMBER_NAV['member/newsletter']['content-title'];
			
			$user = new user(myuid());

			if ( isset($TARGET->arg3) )
			{
				$user->update_value(myuid(), 'on_mailing_list', ($TARGET->arg3 == 'subscribe' ? 1 : 0) );
				notify_redirect('/member/newsletter',"Settings updated successfully");
			}

			$this->v->on_mailing_list = $user->data->on_mailing_list;
			
		}
		
		/*
		* member_special_offers
		*/

		public function member_special_offers()
		{
			global $TARGET, $MEMBER_NAV;
			
			$this->_validate();
			
			set_page_title($MEMBER_NAV['member/special-offers']['page-title']);
			set_meta_desc($MEMBER_NAV['member/special-offers']['meta-desc']);
			$this->v->title = $MEMBER_NAV['member/special-offers']['content-title'];

		}
		
		/*
		* Make sure it's an active admin session
		*/
		
		private function _validate()
		{
			if ( ! session_is_member() )
			{
				notify_redirect('/member/login');
			}
		}

	}

?>

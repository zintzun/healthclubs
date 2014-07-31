<?php

	/*

	User Class
	. . . . . . . . . . . . . . . . . . . . . . . . . .

	Methods:

		Public

			add($parms)
			get($target_user) // email, login, or id
			validate_pass($target_user, $pass)
	
			update($target_user, $parms)
			update_pass($target_user, $new_pass)
			update_email($target_user, $new_email)
			update_login($target_user, $new_login)
			update_url($target_user, $new_url)
			
			new_long_secret($target_user)
			new_short_secret($target_user)

		Private

			can_use_email($email)
			target_field($target_user)
			update_value($target_user, $field, $value)
			user($target_user=false)

	. . . . . . . . . . . . . . . . . . . . . . . . . .

	Usage:
	
		$parms = array
		(
			'login' => 'jv2',
			'email' => 'jv3@vip.ie',
			'password' => 'my_password',
			
		);

		$user = new user;
		$user->add($parms);

	. . . . . . . . . . . . . . . . . . . . . . . . . .

	Data Structure:
	
		User Object
		(
		    [user_id] => 2
		    [login] => newlogin
		    [short_secret] => 14528
		    [long_secret] => u37VvPbK8hHzKXHKeqJWknXpx3v8nqz2
		    [password] => 630faa05ea22c48ed5c4b16ad64f6dfa
		    [email] => jv4@vip.ie
		    [url] => 
		    [is_admin] => 0
		    [last_login] => 
		    [last_updated] => 2007-10-27 12:39:47
		    [creation_date] => 2007-10-27 12:14:17
		)

	*/

	class user
	{
		var $data = false;
		

		function __construct($target_user=false)
		{
			if ( $target_user )
			{
				$this->get($target_user);	
			}
		}

		/**
		 * Valid password.
		 */

		public function validate_pass($target_user, $pass)
		{

			global $db;

			if ( ! $this->get($target_user) )
			{
				return "error: user_does_not_exist";
			}

			$target_field = $this->target_field($target_user);

			if ( ! $user = $db->get_row("SELECT * FROM users WHERE password = '".md5(trim($pass))."' AND $target_field = '$target_user'") )
			{
				return "error: bad_password";
			}
			else if (! $user->is_validated )
			{
				return "error: not_validated";
			}

			// Else, good login!
			return true;
			
		}

		/**
		 * Update Email
		 */

		public function update_email($target_user, $new_email)
		{

			if ( $error = is_error($this->can_use_email($new_email)) )
			{
				return "error: $error";
			}
			
			$this->set_as_edited($target_user);
			
			return $this->update_value($target_user, 'email', $new_email);
		}

		/**
		 * Update Login
		 */

		public function update_login($target_user, $new_login)
		{
			if ( ! ($new_login = trim($new_login)) )
			{
				return "error: missing_login";
			}
			else 	if ( $this->get($new_login) )
			{
				return "error: login_already_exists";
			}
			
			return $this->update_value($target_user, 'login', $new_login);
		}

		/**
		 * Update Pass
		 */

		public function update_pass($target_user, $new_pass)
		{
			$this->new_short_secret($target_user);
			return $this->update_value($target_user, 'password', md5($new_pass));
		}

		/**
		 * Update URL
		 */

		public function update_url($target_user, $new_url)
		{
			$this->set_as_edited($target_user);
			return $this->update_value($target_user, 'url', trim($new_url));
		}

		/**
		 * Generate new short secret and update db
		 */

		public function new_short_secret($target_user)
		{

			$secret = generate_secret('numeric',5);

			if ( $error = is_error($this->update_value($target_user, 'short_secret', $secret) ) )
			{
				return "error: $error";
			}
			
			return $secret;

		}

		/**
		 * Generate new long secret and update db
		 */

		public function new_long_secret($target_user)
		{
			$secret = generate_secret('mixed',32);

			if ( $error = is_error($this->update_value($target_user, 'long_secret', $secret) ) )
			{
				return "error: $error";
			}
			
			return $secret;
		}

		/**
		 * Change any value for a user record.
		 */

		public function update_value($target_user, $field, $value)
		{

			global $db;

			if ( ! $this->get($target_user) )
			{
				return "error: user_does_not_exist";
			}

			$target_field = $this->target_field($target_user);

			return $db->query("UPDATE users SET $field = '".$db->escape($value)."', last_updated = NOW() WHERE $target_field = '$target_user'");

		}

		public function set_as_edited($target_user)
		{
			$this->update_value($target_user, 'approval_status', 'edited_details');
		}
		
		public function approve($target_user)
		{
			$this->update_value($target_user, 'approval_status', 'approved');
		}

		/**
		 * Add user to the system based on incoming parms
		 */

		public function add($parms)
		{

			global $db;

			/**
			 * Error checking
			 */

			if ( ! is_array($parms) )
			{
				return "error: require_parms_array";
			}

			foreach ( array('login','email','password') as $parm )
			{
				if ( !isset($parms[$parm]) )
				{
					return "error: missing_$parm";
				}
			}

			if ( $error = is_error($this->can_use_email($parms['email'])) )
			{
				return "error: $error";
			}
			else if ( isset($parms['login']) && $this->get($parms['login']) )
			{
				return "error: login_already_exists";
			}

			/**
			 * Setup insert
			 */

			$parms['password']     = md5($parms['password']);
			$parms['short_secret'] = generate_secret('numeric',5);
			$parms['long_secret']  = md5(trim($parms['email']).'1foreach'.strrev(trim($parms['email'])));

			/**
			 * Add new user
			 */

			$fields  ='';
			$vals    ='';

			foreach ($parms as $field => $val )
			{
				$fields .= "$field, ";
				$vals   .= "'".$db->escape( $val)."', ";
			}

			$sql = "INSERT INTO users (".substr($fields,0,-2).",creation_date,last_updated) VALUES (".substr($vals,0,-2).",NOW(),NOW())";

			$result = $db->query($sql);
			
			$this->get($parms['email']);

			return $db->insert_id;

		}

		/**
		 * Deletes user record  for $target_user
		 * Can take a user_id, email or login.
		 */

		public function remove($target_user)
		{
			global $db;
			
			
			if ( ! ($user = $this->get($target_user)) )
			{
				return false;
			}

			// Sanitize whatever comes into this function
			$target_user = trim(preg_replace('/[^a-zA-Z0-9\-\_\@\.]/','',$target_user));

			// Decide which field to look up

			$target_field = $this->target_field($target_user);
			
			$db->query("DELETE FROM reviews WHERE user_id = ".$user->user_id);
			$db->query("DELETE FROM users WHERE user_id = ".$user->user_id);
			$db->query("DELETE FROM biz_link_to_user WHERE user_id = ".$user->user_id);

			return true;
			
		}


		/**
		 * Updates user information for $target_user
		 * Can take a user_id, email or login.
		 */

		public function update($target_user, $parms)
		{
			global $db;

			if ( ! ($user = $this->get($target_user)) )
			{
				return "error: user_does_not_exist";
			}
			
			$set_sql = get_set_sql($parms);
			
			if ( ! isset($parms['approval_status']) )
			{
				$this->set_as_edited($target_user);
			}
			
			return $db->query("UPDATE users SET $set_sql,last_updated = NOW() WHERE user_id = {$user->user_id}");
		}
	
		/********************************************************************
		* user->get_users_by_approval_status($approval_status, $search)
		*/	
				
		public function get_users($approval_status = false, $search = false, $biz_owner = false)
		{
			global $db;
			
			$APPROVAL_SQL = '';
			$MATCH_SQL    = '';
			$WHERE_SQL    = '';
			$ORDER_BY     = '';
			
			if ( $search = $db->escape(trim($search)) )
			{
				//first_name,last_name,email,url,biz_name,biz_position,phone,login
				$MATCH_SQL = ", MATCH (first_name,last_name,email,url,biz_name,biz_position,phone,login) AGAINST ('$search') AS rating";
				$WHERE_SQL = "MATCH (first_name,last_name,email,url,biz_name,biz_position,phone,login) AGAINST ('$search' IN BOOLEAN MODE)";
				$ORDER_BY  = 'ORDER BY rating DESC';
			}
			else
			{
				$APPROVAL_SQL = "approval_status = '".$db->escape($approval_status)."'";
			}
			
			$BIZ_SQL = 'AND is_biz_owner = 0';
			if ( $biz_owner )
			{
				$BIZ_SQL = 'AND is_biz_owner = 1';
			}
			
			return $db->get_results("SELECT * $MATCH_SQL FROM users WHERE $WHERE_SQL $APPROVAL_SQL $BIZ_SQL $ORDER_BY");
		}
	
		/**
		 * Loads a user's details into this object and returns a copy of it too.
		 * Can take a user_id, email or login.
		 */

		public function get($target_user)
		{

			global $db;

			// Sanitize whatever comes into this function
			$target_user = trim(preg_replace('/[^a-zA-Z0-9\-\_\@\.]/','',$target_user));

			$target_field = $this->target_field($target_user);

			$this->data = $db->get_row("SELECT * FROM users u WHERE $target_field = '".$db->escape($target_user)."'");

			return $this->data;

		}

		/**
		 * Work out how this request wants to target this user...
		 */

		private function target_field($target_user)
		{
			if ( is_numeric($target_user) )
			{
				$field = 'user_id';
			}
			else if ( preg_match ('/@/',$target_user) )
			{
				$field = 'email';
			}
			else
			{
				$field = 'login';
			}
			
			return $field;
		}

		/**
		 * Make sure its an email that we can use
		 */

		private function can_use_email($email)
		{

			// Email must be valid
			if ( !eregi("^[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$", $email) )
			{
				return 'error: bad_email_address';
			}
			// Email to long
			else if ( strlen($email) > 100 )
			{
				return 'error: email_to_long';
			}
			// User with this email exists
			else if ( $this->get($email) )
			{
				return 'error: email_already_in_db';
			}

			return true;
		}

	}

?>

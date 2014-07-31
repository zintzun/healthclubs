<?php

	$_SESSION['ROOT_DOMAIN'] = $_SERVER['HTTP_HOST'];

	/**
	 * Establish web session, if remember me was switched on
	 */

	if ( ! session_is_active() )
	{
		session_try_to_remember_me();
	}
	else
	{
		session_keep_alive();
	}

	/**
	 * session_establish($user_id,$roles,$email,$username)
	 *
	 * Create a valid session
	 *
	 * @todo Validate input
	 */

	function session_establish($parms,$remember_me=false)
	{

		foreach ( $parms as $k => $v )
		{
			$_SESSION['USER'][$k] = $v;
		}

		if ( $remember_me )
		{
			session_remember_me(myuid());
		}

		$_SESSION['is_logged_in'] = time();

		session_create_new_secret();

	}

	/**
	 * session_create_new_secret()
	 */

	function session_set_as_admin()
	{
		$_SESSION['USER']['is_admin'] = true;
	}

	/**
	 * session_create_new_secret()
	 */

	function session_create_new_secret()
	{	
		static $have_created_secret = false;
		
		if ( ! $have_created_secret )
		{
			$have_created_secret = true;

			$_SESSION['SESSION_SECRET'] = md5(rand(0,1000000));

			setcookie('S',$_SESSION['SESSION_SECRET'],null,'/',$_SESSION['ROOT_DOMAIN']);

			$_COOKIE['S'] = $_SESSION['SESSION_SECRET'];
		}
	}

	/**
	 * session_secret_is_valid()
	 *
	 * Make sure that current session secret is valid
	 */

	function session_secret_is_valid()
	{		
		if ( isset($_SESSION['SESSION_SECRET']) && isset($_COOKIE['S']) )
		{			
			if ( $_SESSION['SESSION_SECRET'] == $_COOKIE['S'] )
			{
				return true;	
			}
		}
	}

	/**
	 * session_clear()
	 *
	 * Clear auth related session values
	 */

	function session_clear()
	{

		unset($_SESSION['is_logged_in']);
		
		if ( isset($_SESSION['USER']) && $_SESSION['USER'] )
		{
			foreach ( $_SESSION['USER'] as $session_var => $unused )
			{
				unset($_SESSION['USER'][$session_var]);
			}

			unset($_SESSION['USER']);

		}

	}

	/**
	 * session_is_active()
	 *
	 * Tests to see if we have an active session or not
	 * ( see session_is_valid() )
	 */

	function session_is_active()
	{

		if ( ! isset($_SESSION['is_logged_in']) || ! is_numeric($_SESSION['is_logged_in']) )
		{
			return false;
		}
		else if ( ( time() - $_SESSION['is_logged_in'] ) > (SESSION_TIME_OUT*60) )
		{
			return false;
		}

		return true;
	}

	/**
	 * session_is_valid()
	 *
	 * Validates session and updates the last touch time
	 * (see session_validate_for_web())
	 */

	function session_is_valid()
	{

		if ( ! session_is_active() )
		{
			session_try_to_remember_me();
		}

		if ( ! session_is_active() || ! session_secret_is_valid() )
		{
			session_clear();
			return false;
		}

		return true;
	}

	/**
	 * session_user_id()
	 *
	 * Get active session user id if one!
	 */

	function session_user_id()
	{
		if ( session_is_active() )
		{
			return $_SESSION['USER']['user_id'];
		}

		return 0;
	}
	
	/**
	 * session_is_biz_owner()
	 *
	 * Is this a biz owner
	 */

	function session_is_biz_owner()
	{
		if ( session_is_active() )
		{
			return $_SESSION['USER']['is_biz_owner'];
		}

		return false;
	}
	
	/**
	 * session_is_member()
	 *
	 * Is this a biz owner
	 */

	function session_is_member()
	{
		if ( session_is_active() )
		{
			return !$_SESSION['USER']['is_biz_owner'];
		}

		return false;
	}

	/**
	 * session_keep_alive()
	 *
	 * Keep current session active
	 */

	function session_keep_alive()
	{

		session_create_new_secret();
		
		// Update last touched time
		$_SESSION['is_logged_in'] = time();

	}

	/**
	 * session_validate_for_web()
	 *
	 * Validates a session on the woyano website))
	 */

	function validate_session()
	{
		if ( ! session_is_valid() )
		{
			$_SESSION['RESTRICTED_ACTION'] = $_SERVER['REQUEST_URI'];
			notify_redirect('/');
		}
	}

	/**
	 * session_is_admin()
	 *
	 * Quick test to see if it's a valid admin session
	 */

	function session_is_admin()
	{
		if ( session_is_active() && isset($_SESSION['USER']['is_admin']) && $_SESSION['USER']['is_admin'] )
		{
			return true;
		}
	}

	/**
	 * session_validate_for_admin()
	 *
	 * Validates a session for admin on the woyano website
	 */

	function validate_admin()
	{
		validate_session();
		if ( ! session_is_admin() )
		{
			session_clear();
			error_redirect('/admin_home');
		}
	}

	/**
	 * session_try_to_remember_me()
	 *
	 * Try to re establish web session based on remember me cookie
	 */

	function session_try_to_remember_me()
	{
		global $db;

		if ( isset($_COOKIE['remember_me']) && $_COOKIE['remember_me'] )
		{

			$user = new user;

			if ( $user = (array) $user->get($_COOKIE['remember_me']) )
			{

				unset($user['password']);
				unset($user['secret']);
				
				session_establish($user);

			}

		}

	}

	/**
	 * session_remember_me()
	 *
	 * Remember this user for a looong time
	 */

	function session_remember_me($user_id)
	{
		// Store values in cookie
		setcookie ('remember_me', $user_id, time() + ((3600*24)*365), '/', $_SESSION['ROOT_DOMAIN']);
		$_COOKIE['remember_me'] = $user_id;		
	}

	/**
	 * session_forget_me()
	 *
	 * Kill the remember me cooie
	 */

	function session_forget_me()
	{
		session_clear();

		// Unset the cookie
		setcookie ('remember_me', '', time() - ((3600*24)*365), '/', $_SESSION['ROOT_DOMAIN']);
		unset($_COOKIE['remember_me']);

	}

?>
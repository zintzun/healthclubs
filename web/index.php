<?php

	// Load config options --
	include "lib/common.inc";
	include 'lib/config.inc';
	include "lib/utils.inc";
	include "lib/text_utils.inc";
	include "lib/ez_sql_core.inc";
	include "lib/ez_mysql.inc";
	include "lib/db_connect.inc";
	include "lib/target.inc";

	include "interfaces/user.inc";
	include "interfaces/session.inc";
	include "interfaces/html_form.inc";

	// Force the system in and out of https mode
	if ( ! preg_match('/(ajax)/',$_GET['action']) && ! IS_LOCALHOST && ! preg_match('/^www.healthclubnet.com/',$_SERVER['HTTP_HOST']) )
	{
		if ( preg_match(HTTPS_ACTION_REGEX,$_GET['action']) && ! isset($_SERVER['HTTPS']) )
		{
		   notify_redirect("https://".$_SERVER['HTTP_HOST'].'/'.trim($_SERVER['REQUEST_URI'],'/'));
		}
		else if ( isset($_SERVER['HTTPS']) && ! preg_match(HTTPS_ACTION_REGEX,$_GET['action']) )
		{
		   notify_redirect("http://".$_SERVER['HTTP_HOST'].'/'.trim($_SERVER['REQUEST_URI'],'/'));
		}
	}

	if ( preg_match('/^healthclubnet/i',$_SERVER['HTTP_HOST']) )
	{
		if ( $_GET['action'] == '' )
		{
			header("HTTP/1.1 301 Moved Permanently");
			header("Location: http://www.".$_SERVER['HTTP_HOST']);
			exit;
		}
		else if ( $_GET['action'] == 'personal-trainers.html' )
		{
			header("HTTP/1.1 301 Moved Permanently");
			header("Location: http://www.".$_SERVER['HTTP_HOST'].'/personal-trainers');
			exit;
		}
		else if ( preg_match('/^(health-clubs|personal-trainers)/i',$_GET['action']) )
		{
			header("HTTP/1.1 301 Moved Permanently");
			header("Location: http://www.".$_SERVER['HTTP_HOST'].'/'.$_GET['action']);
			exit;
		}
	}
	else if ( isset($_GET['action']) && $_GET['action'] == 'personal-trainers.html' )
	{
					header("HTTP/1.1 301 Moved Permanently");
					header("Location: http://".$_SERVER['HTTP_HOST']."/personal-trainers");
					exit;
	}

	// Initialise target
	$TARGET = new target(isset($_GET['action'])?$_GET['action']:'home');

	if ( $TARGET->action_orig == 'favicon.ico' )
	{
		exit;
	}

	// Sitemap hack because ask wont except a file without .xml in the name grr
	if ( preg_match('/sitemap.xml/i',$TARGET->action_orig) )
	{
		header ("content-type: text/xml"); 
		include "actions/sitemap/_www_sitemap.proc";	
		exit;
	}

	
	//    // Another hack for robots text
	
	
	//	if ( preg_match('/robots.txt/i',$TARGET->action_orig) )
	//	{
	//		$TARGET->action = 'robots';
	//		$TARGET->action_root = 'actions/robots';
	//	}



	// Hack to pipe listing action to its sub-action
	if ( preg_match(LISTING_ROOTS,$TARGET->action) )
	{
		$i=6;
		$listing_action = 'root';
		foreach (  array('lead-form','unused','entry','city','state') as $cur_action )
		{
			if ( isset($TARGET->{'arg'.$i}) && $i != 5)
			{
				$listing_action = $cur_action;
				break;
			}
			$i--;
		}
		$TARGET->action = "listing-$listing_action";
		$TARGET->action_root = "actions/listing-{$listing_action}";
		
		// If this is a search by zip action
		if ( $listing_action == 'city' && $TARGET->arg2 == 'by-zip' )
		{
			$TARGET->action = "search-by-zip";
			$TARGET->action_root = "actions/search-by-zip";
		}

		// If this is a search by zip action
		if ( isset($TARGET->arg2) && $TARGET->arg2 == 'claim-biz' )
		{
			$TARGET->action = "listing-claim-biz";
			$TARGET->action_root = "actions/listing-claim-biz";
		}
	}

	// Make sure action exists
	if ( ! is_dir($TARGET->action_root) )
	{
		$TARGET->action      = 'page-not-found';
		$TARGET->action_root = 'actions/page-not-found';
		$TARGET->web_root    = 'template';
		$TARGET->gfx_root    = 'template/gfx';
	}

	/***************************************************************
	* Nice & simple caching system :)
	*/

	if ( CACHE_FULL_PAGES && preg_match(CACHE_THIS_ACTION,$TARGET->action) )
	{
		if ( $PAGE = cache_get($TARGET->cache_id,false) )
		{
			ob_start("ob_gzhandler");
			print $PAGE;
			exit;
		}
	}

	// Keep track of last action (must be after target init)
	$_SESSION['LAST_ACTION'] = $_SERVER['REQUEST_URI'];

	/***************************************************************
	* Re-populate post values from last post (if set)
	*/

	// Store these post values for next session
	if ($_POST)
	{
		$_SESSION['LAST_POST'] = $_POST;
		
		// Clear special post buttons such as do_save
		foreach ( $_SESSION['LAST_POST'] as $k => $v )
		{
			if ( preg_match('/^(do_)/',$k) )
			{
				unset($_SESSION['LAST_POST'][$k]);
			}
		}
		
	}
	else if ( isset($_SESSION['LAST_POST']) )
	{

		// Assign last post to this post
		$_POST = $_SESSION['LAST_POST'];

		// Unset the stored values
		session_unregister('LAST_POST');

	}

	/***************************************************************
	* Populate error/notify constants (if set)
	*/

	if ( isset($_SESSION['ERROR_STR']) )
	{
		define('ERROR_STR', $_SESSION['ERROR_STR']);
		unset($_SESSION['ERROR_STR']);
	}
	else if ( isset($_SESSION['NOTIFY_STR']) )
	{
		define('NOTIFY_STR', $_SESSION['NOTIFY_STR']);
		unset($_SESSION['NOTIFY_STR']);
	}

	/***************************************************************
	* Run this action
	*
	* (1) Try to run with a controller
	*/

	if ( $TARGET->controller )
	{

		include $TARGET->controller->path;
		
		// Iinstantiate the controller
		eval("\$CONTROLLER = new {$TARGET->controller->class};");

		// Make sure method exists, else use index
		if ( ! method_exists($CONTROLLER,$TARGET->controller->method ) )
		{
			$TARGET->controller->view = $TARGET->controller->method = "{$TARGET->controller->root}_index";
		}

		// Now run the controller method
		$CONTROLLER->{$TARGET->controller->method}();

		// Make all controller v vars available to the template
		if ( isset($CONTROLLER->v) && $CONTROLLER->v ) foreach ( $CONTROLLER->v as $k => $v ) ${'__'.$k} = $v;

		ob_start();
		include "$TARGET->action_root/{$TARGET->controller->view}.htm";
		$TARGET->content = ob_get_clean();

	}

	/***************************************************************
	* (1) Try to run the old way with 'func','proc'
	*/
	else
	{

		// Loop through each action file
		foreach ( array('func','proc','htm') as $type )
		{
			$path = "$TARGET->action_root/{$TARGET->action}.$type";
	
			if ( $type == 'htm' ) 
			{
				ob_start();
			}
			
			if ( file_exists($path) )
			{
				include $path;
			}
			
			if ( $type == 'htm' ) 
			{
				$TARGET->content = ob_get_clean();
			}
	
		}

	}

	/***************************************************************
	* Now run the template/cache stuff (if required)
	*/
	
	ob_start();
	foreach ( $TARGET->templates as $template )
	{
		include $template;	
	}

	if ( CACHE_FULL_PAGES && preg_match(CACHE_THIS_ACTION,$TARGET->action) )
	{	
		cache_store($TARGET->cache_id,ob_get_contents(),false);
	}

?>

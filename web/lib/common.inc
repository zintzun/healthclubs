<?php

	session_cache_expire(60); // cache expire 60 mins

	session_start();

	if ( ! isset($_SESSION['cache_pages_for_admin']) && preg_match('/(98.149.177.161|71.103.59.211|75.83.238.123|127.0.0.1|98.154.226.7|71.103.46.234)/',$_SERVER['REMOTE_ADDR']) )
	{
		define('IS_ADMIN_IP',true);
	}
	else
	{
		define('IS_ADMIN_IP',false);
	}

	define('SESSION_TIME_OUT', 30);

	if ( preg_match("/Users/i",$_SERVER["DOCUMENT_ROOT"]) )
	{
		define('OS','mac');
	}
	else if ( isset($_SERVER["SystemRoot"]) && preg_match("/windows/i",$_SERVER["SystemRoot"]) )
	{
		define('OS','windows');
	}
	else
	{
		define('OS','unix');
	}

	define('DEBUG', (OS == 'windows' || OS == 'mac') ? false : false);
	define('USE_GZHANDLER', (OS == 'windows' || OS == 'mac') ? false : true);
	define('USE_ANALYTICS', (OS == 'windows' || OS == 'mac') ? false : true);
	define('IS_LOCALHOST', (OS == 'windows' || OS == 'mac') ? true : false);

	if ( IS_LOCALHOST )
	{
		error_reporting(E_ALL);
	}
	
	define('PHP_INI_SEPERATOR',(OS == 'windows') ? ';' : ':');
	ini_set('include_path','.'.PHP_INI_SEPERATOR.'./third_party/PEAR'.PHP_INI_SEPERATOR.'./third_party/contact_import');

	/***************************************************************
	* Strip slashes on all incoming post
	*/

	// Is magic quotes on?
	if (get_magic_quotes_gpc())
	{
		foreach ( array ( '_GET', '_POST', '_REQUEST' ) as $type )
		{
			// Yes? Strip the added slashes
			foreach ( ${$type} as $key => $val )
			{
				if ( ! is_array($val) )
				{
					${$type}[$key] = stripslashes($val);
				}
			}
		}
	}

	/*
	* Adds a javascript file to the array TARGET->js_files.
	* In template/header.htm all the js files in the array will be included.
	*/
	function use_js($js_file)
	{
		global $TARGET;
		$TARGET->js_files[] = $js_file;
	}

	/*
	* Adds a CSS file to the array TARGET->css_files.
	* In template/header.htm all the CSS files in the array will be included.
	* Will remove the first / of the path if given full path.
	*/
	function use_css($css_file)
	{
		global $TARGET;
		$css_file = ltrim($css_file, '/');
		$TARGET->css_files[] = $css_file;
	}

	function captcha_is_correct()
	{
		if ( ! USE_CAPTCHA )
		{
			return true;	
		}
		
		include_once "third_party/recaptcha/recaptchalib.php";
		
		$resp = recaptcha_check_answer 
		(
			RECAPTCHA_PRIVATE_KEY, // In config.inc
			$_SERVER["REMOTE_ADDR"],
			$_POST["recaptcha_challenge_field"],
			$_POST["recaptcha_response_field"]
		);
		
		return $resp->is_valid;
		
	}

	function html_tabs($nav_array)
	{

		global $TARGET;

		$html = '<ul class="tabs">'."\n";

		foreach ( $nav_array as $nav )
		{
			$current  = ''; 
			if ( preg_match($nav['match'],$TARGET->action_orig) )
			{
				$current  = 'class="current"'; 
			}
			$html .= "<li $current><a href='{$nav['target']}'>{$nav['text']}</a></li>\n";
		}

		return $html.'</ul>';

	}

	function make_parms_from_post($fields)
	{
		// Only add a field to parms if it exists in the form
		$parms = array();
		foreach ( $fields as $field )
		{
			if ( isset($_POST[$field]) )
			{
				$parms[$field] = $_POST[$field];
			}
		}
		
		return $parms;
	}
	
	function set_view($view)
	{
		global $TARGET;
		$TARGET->controller->view = $view;
	}
	
	function set_page_title($page_title)
	{
		global $TARGET;
		$TARGET->page_title = $page_title;
	}
	
	function set_meta_desc($meta_desc)
	{
		global $TARGET;
		$TARGET->meta_desc = $meta_desc;
	}

?>

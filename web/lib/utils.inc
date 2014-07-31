<?php

	function set_pref($pref_name,$pref_val)
	{
		global $db;
		$db->query("INSERT INTO prefs (pref_name,pref_val) VALUES ('".$db->escape($pref_name)."','".$db->escape($pref_val)."') ON DUPLICATE KEY UPDATE pref_val = '".$db->escape($pref_val)."'");
	}
	
	function get_pref($pref_name)
	{
		global $db;
		return $db->get_var("SELECT pref_val FROM prefs WHERE pref_name = '".$db->escape($pref_name)."'");
	}

	function get_lead_rate($user_id)
	{
		$user = new user;
		if ( $user->get($user_id) )
		{
			if ( $user->data->lead_rate )
			{
				return $user->data->lead_rate;	
			}	
		}
		
		return get_pref('global-lead-rate');
	}

	/***************************************************************
	* Looks for an array named STRINGS that maps error codes to more informative error messages.
	* If the array STRING is not set then the error code is displayed verbatim.
	*/

	function get_error($error)
	{
		global $STRINGS;
		return isset($STRINGS[$error])?$STRINGS[$error]:$error;
	}

	/***************************************************************
	* Opens file and reads contents, Optional limit parameter
	* allows you to set how many lines are read out of the file. if no limit is specified whole file is read
	*/
	
	function file_get($path, $limit ='')
	{
		$file = '';

		// if path has a www or http in it then use fsock open
		if (preg_match('/(http:|www\.)/i',$path))
		{
			// from the path deduce the host and port number. assume port 80
			$exp= explode('/', str_replace('http://', '', $path) );
			$host = $exp[0];
			$port = 80;

			// If a colon exists in the path then assume a different port is used other than 80.
			// use explode to find the new host and the port
			if(strpos($host,':') > 0)
			{
				$host_arr = explode(':', $host );
				$host = $host_arr[0];
				$port = $host_arr[1];
			}

			if ($fd = fsockopen($host, $port))
			{
				fputs($fd, "GET $path HTTP/1.0\n");
				fputs($fd, "Host: $host\n");
				fputs($fd, "Connection: close\n\n");
			}


		}
		else
		{
			$fd = fopen ($path, 'r');

		}

		if ($fd)
		{
			$i = 0; //reset counter to track limit
			while (!feof ($fd) )
			{
			  	// if a limit has been set and the row count reach limit break loop
			  	if (is_numeric($limit) AND $i >= $limit)
			  	{
			  		break;
			  	}
			  	else
			  	{
			  		// add line to output var
			  		$file .= fgets($fd, 1024);
			  		$i++;
			  	}
		  	}
			fclose ($fd);
		}

		$fp = explode('Connection: close',$file);

		if (isset($fp[1]))
		{
			$file = $fp[1];
		}
		// Note: We always return a val even if its '' dont want to stop the script if the file is not there


		return $file;
	}


	/***************************************************************
	* Simple func to write a file
	* defaults to write mode
	*/

	function file_write($path, $str, $mode = 'w')
	{

		if ( ! $fp = @fopen($path, $mode) )
		{
			die("Unable to write to file '$path')");
		}
		else
		{
			fwrite ($fp,$str);
			fclose($fp);
		}

	}

	/****************************************************************
	* Runs any command on the command line and returns
	* the results (if any)
	*/

	function run_command($cmd)
	{
		ob_start();
		system($cmd);
		$cmd_output = ob_get_contents();
		ob_end_clean();
		return $cmd_output;
	}

	/****************************************************************
	* Returns contents of a DIR as an array
	*/

	function dir_to_array($path,$verbose=false,$filter='/^(\.|\.\.|\.svn)$/')
	{
		$i=0;
		if ($handle = @opendir($path))
		{
			while (false !== ($file = @readdir($handle)))
			{
				if (!preg_match($filter,$file))
				{
					if ( $verbose )
					{
						$stat         = stat("$path/$file");
						$stat['file'] = $file;
						$dir_array[]  = $stat;
					}
					else
					{
						$dir_array[] = $file;
					}
				}
			}
			@closedir($handle);
			
			if ( isset( $dir_array ) )
			{
				return $dir_array;
			}
		}
		else
		{
			return false;
		}
	}

	/****************************************************************
	* Dump a PHP object to screen (nicely formatted)
	*/

	function print_ar($array)
	{
		print "<pre style='text-align: left; margin: 20px;'>";
		print_r($array);
		print "</pre>";
	}

	function clear_post_vals()
	{
		if ( isset($_POST) )
		{
			foreach ( $_POST as $key => $value )
			{
				$_POST[$key] = '';
			}
		}
	}
	
	function get_var($var_name,$default='')
	{
		// Must be post because of framework storing post val in session
		return isset($_POST[$var_name])?$_POST[$var_name]:$default;
	}

	/**
	 * @param string $location the url
	 * @param string $error_str the error str
	 */
	function error_redirect($location,$error_str=false)
	{
		jvw_redirect($location,'ERROR_STR',$error_str);
	}
	
	/**
	 * @param string $location the url
	 * @param string $error_str the notify str
	 */
	function notify_redirect($location,$error_str=false)
	{
		jvw_redirect($location,'NOTIFY_STR',$error_str);
	}
	
	function jvw_redirect($location,$str_name=false,$str=false)
	{
		if ( $str && $str_name)
		{
			$_SESSION[$str_name] = $str;
		}

		header("Location: $location");
		exit;
	}
	
	function error_me($str=false)
	{
		global $TARGET;
		error_redirect('/'.$TARGET->action_orig,$str);
	}

	function notify_me($str=false)
	{
		global $TARGET;
		notify_redirect('/'.$TARGET->action_orig,$str);
	}
	
	// Expected format....  error: no_user
	function is_error($result)
	{
		if ( is_string($result) && preg_match('/^error: (.*)$/s', $result, $matches) )
		{
			return $matches[1];
		}

		return false;
	}

	// Note: The bots are referenced in the linnks by bref=bot_id_number
	// this is so that goolge does not see the word google in our query string

	function is_bot()
	{
		global $BOTS;
		static $cached_result;

		if ( $cached_result || $cached_result === false )
		{
			return $cached_result;
		}

		$is_bot = false;

		$BOTS = array
		(
			1  => 'google',
			2  => 'fast',
			3  => 'yahoo',
			4  => 'exalead',
			5  => 'sqworm',
			6  => 'msn',
			7  => 'jeeves',
			8  => 'inktomi',
			9  => 'teoma',
			10 => 'zealbot',
			11 => 'overture',
			12 => 'wisenut',
			13 => 'robozilla',
			14 => 'php',
		);

		foreach ( $BOTS as $bot_id => $bot )
		{
			if ( preg_match("/$bot/i",$_SERVER['HTTP_USER_AGENT']) )
			{
				$is_bot = $bot_id;
			}
		}

		return $cached_result = $is_bot;
	}

	function cache_store($identifier_str,$cache_contents,$do_serialize=true)
	{
		global $db,$TARGET;
		
		if ( ! USE_CACHE )
		{
			return false;	
		}
		
		if ( $do_serialize )
		{
			$cache_contents = serialize($cache_contents);
		}
		
		$md5id = md5($identifier_str);
		$set_sql = "data = COMPRESS('".$db->escape($cache_contents)."'), cached_time = '".time()."'";
		
		if ( ! $db->get_var("SELECT count(*) FROM cache WHERE area = 'site_wide' AND md5id = '$md5id'") )
		{
			$db->query("INSERT INTO cache SET $set_sql, area = 'site_wide', md5id = '$md5id'");
		}
		else
		{
			$db->query("UPDATE cache SET $set_sql WHERE area = 'site_wide' AND md5id = '$md5id'");
		}
	}

	function cache_get($identifier_str,$do_serialize=true)
	{

		global $db,$DO_NOT_CACHE,$TARGET;

		$md5id = md5($identifier_str);

		if ( isset($DO_NOT_CACHE) || ! USE_CACHE )
		{
			$db->query("DELETE FROM cache WHERE area = 'site_wide' AND md5id = '$md5id'");
			return false;	
		}

		// Try to get previously cached version
		if ( $cache = $db->get_row("SELECT * FROM cache WHERE area = 'site_wide' AND md5id = '$md5id'") )
		{
			// Only use this cache file if less than 24 (hours)
			if ( (time() - $cache->cached_time) > (3600*24*CACHE_TIMEOUT) )
			{
				;
			}
			else
			{
				if ( $do_serialize )
				{
					// Uncompress with mySQL
					// return unserialize($db->get_var("SELECT UNCOMPRESS(data) FROM cache WHERE subdomain = '".$db->escape($TARGET->sub_domain)."' AND md5id = '$md5id'"));
					// Uncompress with PHP
					return unserialize(gzuncompress(substr($db->get_var("SELECT data FROM cache WHERE area = 'site_wide' AND md5id = '$md5id'"), 4)));
				}
				else
				{
					return gzuncompress(substr($db->get_var("SELECT data FROM cache WHERE area = 'site_wide' AND md5id = '$md5id'"), 4));
				}
			}
		}

	}

	function map_data_path($file_name,$root_dir,$depth=3,$force_create=false,$force_different_name=false)
	{
		$file_name = (string) $file_name;
		// strip extension (if one exists) to avoid creating a directory of "."
		if ( strrpos($file_name, '.') !== false ) 
		{
			$ext = substr($file_name, strrpos($file_name, '.'));
			$file_name = substr($file_name, 0, strrpos($file_name, $ext));
		}

		// Make a five character prefix for this file
		$sub_dir = '';
		for ( $i=0 ; $i < strlen($file_name); $i++ )
		{	
			if ( $i == ($depth) || $file_name[$i] == '' ) break;

			$sub_dir  .= $file_name[$i].'/';

			// Make sure this dir exists
			if ( $force_create && ! is_dir("$root_dir/$sub_dir") )
			{
				mkdir("$root_dir/$sub_dir");
			}
		}

		if ( $force_different_name )
		{
			return "{$root_dir}/{$sub_dir}{$force_different_name}";
		}
		else if( isset($ext) )
		{
			return "{$root_dir}/{$sub_dir}{$file_name}{$ext}";
		}
		else
		{
			return "{$root_dir}/{$sub_dir}{$file_name}";
		}

	}

	function clear_session_error()
	{
		unset($_SESSION['ERROR_STR']);
		unset($_SESSION['NOTIFY_STR']);
	}
		
	function myuid()
	{
		return isset($_SESSION['USER']['user_id'])?$_SESSION['USER']['user_id']:0;
	}
		
	/****************************************************************
	* Recursively remove directory
	*/

	function recursive_rmdir($dir, $DeleteMe=true)
	{
	    if(!$dh = @opendir($dir)) return;
	    
	    while (($obj = readdir($dh))) 
	    {
	        if($obj=='.' || $obj=='..') continue;
	        if (!@unlink($dir.'/'.$obj)) recursive_rmdir($dir.'/'.$obj, true);
	    }
	    if ($DeleteMe)
	    {
	        closedir($dh);
	        @rmdir($dir);
	    }
	}

?>
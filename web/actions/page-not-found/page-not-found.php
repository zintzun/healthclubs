<?

	$www = '';
	if ( preg_match('/^healthclubnet/i',$_SERVER['HTTP_HOST']))
	{
		$www = 'www.';
	}
	if ( preg_match('/(healthclubs|personaltrainers)/i',$TARGET->arg1) )
	{
		if ( preg_match('/^healthclubs/i',$TARGET->action_orig) )
		{
			$new_target =  preg_replace('/^healthclubs/i', 'health-clubs',$TARGET->action_orig);
		}
		else if ( preg_match('/^personaltrainers/i',$TARGET->action_orig) )
		{
			$new_target =  preg_replace('/^personaltrainers/i', 'personal-trainers',$TARGET->action_orig);
		}

		if  (isset($TARGET->arg4) && !(isset($TARGET->arg5)))
		{
			list($biz_id) = explode('.',$TARGET->arg4);

			if ( preg_match('/\.html/i',$TARGET->arg4) && is_numeric($biz_id) )
			{
				include "interfaces/biz/biz_details.inc";
				$business = new business;

				if ($biz = $business->get($biz_id) )
				{

					//$new_target =  preg_replace('/\.html$/i', '',$new_target);
					list($new_target) = explode('.', $new_target);

					header("HTTP/1.1 301 Moved Permanently");
					header("Location: http://$www".$_SERVER['HTTP_HOST']."/". $new_target .'/'.format_link_subject($biz->name));
					exit;
				}
			}	
		}
		else if (isset($TARGET->arg3) && !(isset($TARGET->arg4)))
		{
			if ( preg_match('/\.html/i',$TARGET->arg3))
			{
					list($new_target) = explode('.', $new_target);


					header("HTTP/1.1 301 Moved Permanently");
					header("Location: http://$www".$_SERVER['HTTP_HOST']."/". $new_target);
					exit;
			}
		}
		else if (isset($TARGET->arg2) && !(isset($TARGET->arg3)))
		{
			if ( preg_match('/\.html/i',$TARGET->arg2))
			{
					list($new_target) = explode('.', $new_target);

					header("HTTP/1.1 301 Moved Permanently");
					header("Location: http://$www".$_SERVER['HTTP_HOST']."/". $new_target);
					exit;
			}
		}

	}


	header("HTTP/1.0 404 Not Found"); 
?>

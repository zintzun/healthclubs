<?php

	if ( isset($GLOBALS['mail.inc']) ) return;

	$GLOBALS['mail.inc'] = true;

	function send_mail_helper($to,$template,$parms)
	{

		$message = file_get($template);

		send_mail
		(
			$to, 
			merge_snippet($parms,mail_get_subject($message)), 
			merge_snippet($parms,mail_strip_headers($message)), 
			merge_snippet($parms,mail_get_from($message))
		);
	}

	/**
	 * Function to send external email
	 */

	function send_mail($to, $subject, $message, $from)
	{

		$to_addr   = split_mail_address($to);
		$from_addr = split_mail_address($from);

		// Send to session debug on localhost
		if ( IS_LOCALHOST )
		{
			if ( ! isset($_SESSION['MAIL_SEND_COUNT']) )
			{
				$_SESSION['MAIL_SEND_COUNT'] = 0;
			}
			
			$_SESSION['MAIL_SEND_COUNT']++;
			
			//print_ar(array($mail));
			$_SESSION['LAST_MAIL_DEBUG'] = array
			(
				'Send Num' => $_SESSION['MAIL_SEND_COUNT'],
				'To' => ($to_addr[1]?$to_addr[1]:$to_addr[0])." <$to_addr[0]>",
				'Subject' => $subject,
				'Message' => wordwrap($message,100),
				'From' => "From: $from_addr[1] <$from_addr[0]>\n",
			);
		}
		else
		{
			mail
			(
				"$to_addr[1] <$to_addr[0]>",
				$subject,
				wordwrap($message,100),
				"From: $from_addr[1] <$from_addr[0]>\n"
			);
		}
		
	}

	// @todo - depreciate function
	function mail_get_subject(&$mail_content)
	{
		return mail_get_header('Subject',$mail_content);
	}

	// @todo - depreciate function
	function mail_get_from(&$mail_content)
	{
		return mail_get_header('From',$mail_content);
	}

	// @todo - depreciate function
	function mail_get_header($header,&$mail_content)
	{
		preg_match("/$header: (.*)[\n]+/",$mail_content,$matches);
		if ( isset($matches[1]) )
		{
			return $matches[1];
		}
	}

	// @todo - depreciate function
	function mail_strip_headers(&$mail_content)
	{
		return preg_replace("/From: (.*)[\n\r]+Subject: (.*)[\n\r]+/",'',$mail_content);
	}

	function split_mail_address($address)
	{
		if ( preg_match("/(.*) <(.*)>/", $address, $matches) )
		{
			$name = $matches[1];
			$email = $matches[2];
		}
		else
		{
			$email = $address;
			$name = false;
		}

		return array($email, $name);
	}


?>
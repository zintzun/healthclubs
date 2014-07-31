<?php

	session_forget_me();	
	
		if($_SERVER['SERVER_PORT'] != '80') 
	
	{ 
	
		header('Location: http://'.$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI']); exit(); 
	
	}
	
	notify_redirect('/biz/login', 'You have been successfully logged out.');
		
?>
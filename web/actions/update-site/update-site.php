<?php

	if ( ! IS_ADMIN_IP )
	{
		validate_admin();
	}
	
	// Some updates may take a while to run and we don't want them to stop half way through!
	ini_set('max_execution_time', 0);
	ini_set('memory_limit', '300M');
	set_time_limit(0);

	include "interfaces/schema_helpers.inc";

	set_error_handler('update_error_handler');

	// Update_id is the update to run (and all updates after it)
	if (isset($_POST['do_update_id'])) 
	{
		$update_id = $_POST['do_update_id'];
	}

	// If auto, it will figure out which updates to run automatically
	if ( isset($_GET['auto_run']) )
	{
		$auto_run = $_GET['auto_run'];
	}

	print '<h1>System updates</h1>';
	print "<p>Current system date is ".date('Y-m-d H:i:s')."</p>";

	// Find out what version we're up to and do all updates after that only
	$running_version = update_get_running_version();
		
	// Find out what updates are available (all, including those already done)
	$updates = update_get_updates();

	$latest_version = max($updates);

	// Let the user know if they're up to date or not
	print update_status_html($running_version, $latest_version);

	// Allow the user to manually select an update to run
	print update_manual_select_html($running_version, $latest_version, $updates);	

	// Display the update/install history (except if we're about to do an update
	if ( ! (isset($update_id)) )
	{
		print update_history_html($running_version);
	}

	// Display update messages from previous run (stored in session variable due to redirect)
	if (isset($_SESSION['update_messages']))
	{
		print update_messages_html($_SESSION['update_messages']);
		unset($_SESSION['update_messages']);
	}


	// For an auto run, start with the update after the running version
	if (isset($auto_run) && $running_version < $latest_version )
	{
		$update_id = $running_version + 1;
	}

	// ACTUALLY DO UPDATES
	if ( isset($update_id) ) 
	{
		// Do all updates starting from and including $update_id to to $latest_version
		update_do_updates($update_id, $latest_version);

		update_message("<b>All updates complete!</b>");

		// update_message("Creating new schema files");

		// Now that we're done, pop the latest table schemas straight from the db into the schema files

		// We want schema files for almost all tables
		// update_refresh_table_schemas($last_update = max($updates), $exclude_tables = array('email_addresses'));
		
		update_message("<b>Finished</b>");

		notify_redirect('/'.$TARGET->action);
	}

	exit; // need to exit

?>
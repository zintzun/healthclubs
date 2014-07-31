<?php

	$SQL = "ALTER TABLE users ADD lead_rate FLOAT DEFAULT 0";
	$db->query($SQL);
	update_message($SQL);
	
	$SQL = "ALTER TABLE biz_leads ADD credit_lead BOOL DEFAULT 0";
	$db->query($SQL);
	update_message($SQL);
	
	update_message("Installing table prefs");
	create_table(include "db/prefs.inc");
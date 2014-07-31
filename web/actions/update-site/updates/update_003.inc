<?php

	$SQL = "ALTER TABLE users ADD card_number VARCHAR(4)";
	$db->query($SQL);
	update_message($SQL);
	
	$SQL = "ALTER TABLE users ADD card_type VARCHAR(10)";
	$db->query($SQL);
	update_message($SQL);
	
	$SQL = "ALTER TABLE users ADD card_name VARCHAR(255)";
	$db->query($SQL);
	update_message($SQL);

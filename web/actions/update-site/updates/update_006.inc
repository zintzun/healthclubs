<?php

	$SQL = "ALTER TABLE biz_leads ADD lead_rate FLOAT DEFAULT 0";
	$db->query($SQL);
	update_message($SQL);
	
	$SQL = "UPDATE biz_leads SET lead_rate = 4.99";
	$db->query($SQL);
	update_message($SQL);

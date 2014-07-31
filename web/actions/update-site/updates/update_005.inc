<?php

	$SQL = "ALTER TABLE users ADD pp_last_tid VARCHAR (255)";
	$db->query($SQL);
	update_message($SQL);
	
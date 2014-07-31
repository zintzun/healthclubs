<?php
	 
	return array
	(
		'table_name'      => 'prefs',

		'VARCHAR (32)' => array
		(
			'pref_name'
		),

		'TEXT' => array
		(
			'pref_val',
		),

		'PRIMARY KEY'=> array
		(
			'pref_name',
		),
	);
?>

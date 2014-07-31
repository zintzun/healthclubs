<?php

	return array
	(
		'table_name'      => 'google_geocache',

		'VARCHAR (32)' => array
		(
			'addr_key',
		),
		
		'VARCHAR (255)' => array
		(
			'address',
		),
		
		'VARCHAR (20)' => array
		(
			'latitude',
			'longitude',
		),

		'KEY' => array
		(
			'address',
		),

		'PRIMARY KEY'=> array
		(
			'addr_key',
		),
	);

?>
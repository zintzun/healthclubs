<?php

	return array
	(
		'table_name'      => 'cache',

		'VARCHAR (100)' => array
		(
			'md5id',
			'area',
		),
		
		'INT' => array
		(
			'cached_time',
		),

		'BLOB' => array
		(
			'data',
		),

		'PRIMARY KEY'=> array
		(
			'md5id,area',
		),
	);

?>
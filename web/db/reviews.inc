<?php

	return array
	(
		'table_name'      => 'reviews',

		'INT UNSIGNED NOT NULL AUTO_INCREMENT' => array
		(
			'review_id',
		),
		
		'INT UNSIGNED' => array
		(
			'biz_id',
			'user_id',
			'rating'
		),

		'VARCHAR (255)' => array
		(
			'type',
			'email',
			'name'
		),
		
		'TEXT' => array
		(
			'review',
		),

		'DATETIME'=> array
		(
			'creation_date',
			'last_updated',
		),

		'BOOL DEFAULT 0' => array
		(
			'is_approved',
		),

		'KEY'=> array
		(
			'biz_id',
			'user_id',
		),

		'PRIMARY KEY'=> array
		(
			'review_id',
		),
	);

?>

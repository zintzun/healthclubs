<?php

	return array
	(
		'table_name'      => 'biz_leads',

		'INT UNSIGNED NOT NULL AUTO_INCREMENT' => array
		(
			'lead_id',
		),
		
		'INT UNSIGNED NOT NULL' => array
		(
			'biz_id',
		),

		'VARCHAR (255)' => array
		(
			'type',
			'first_name',
			'last_name',
			'phone',
			'zip',
			'email',
			'coupon'
		),

		'DATETIME'=> array
		(
			'creation_date',
			'last_updated',
		),
		
		'BOOL DEFAULT 0' => array
		(
			'i_am_18',
			'is_approved',
			'is_done',
		),

		'FULLTEXT'=> array
		(
			'first_name,last_name,phone,zip,email,coupon',
		),

		'KEY'=> array
		(
			'biz_id',
		),

		'PRIMARY KEY'=> array
		(
			'lead_id',
		),
	);
?>

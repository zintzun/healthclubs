<?php

	return array
	(
		'table_name'      => 'biz_pics',

		'INT UNSIGNED NOT NULL AUTO_INCREMENT' => array
		(
			'pic_id',
		),
		
		'INT UNSIGNED NOT NULL' => array
		(
			'biz_id',
		),
		
		'INT UNSIGNED' => array
		(
			'width',
			'height',
			'sort_order',
		),
		
		'VARCHAR (255)' => array
		(
			'pic_type',
		),
		
		'BOOL DEFAULT 0' => array
		(
			'is_published',
		),

		'KEY'=> array
		(
			'biz_id',
			'pic_type',
		),

		'PRIMARY KEY'=> array
		(
			'pic_id',
		),
	);
?>

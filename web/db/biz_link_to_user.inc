<?php

	return array
	(
		'table_name'      => 'biz_link_to_user',
		
		'INT UNSIGNED' => array
		(
			'user_id',
			'biz_id'
		),

		'BOOL DEFAULT 0' => array
		(
			'is_linked',
		),
		
		'DATETIME'=> array
		(
			'creation_date',
		),

		'KEY'=> array
		(
			'user_id',
			'biz_id',
		),

		'PRIMARY KEY'=> array
		(
			'user_id,biz_id',
		),
	);

?>

<?php

	/**
	 * Table structure for biz_bookmarks
	 */

	return array
	(
		'table_name'      => 'biz_bookmarks',
		
		'INT UNSIGNED' => array
		(
			'user_id',
			'biz_id',
		),
		
		'KEY'=> array
		(
			'biz_id',
			'user_id',
		),
		
		'PRIMARY KEY'=> array
		(
			'biz_id,user_id',
		),
	);

?>

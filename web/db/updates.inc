<?php
	 
	return array
	(
		'table_name'      => 'updates',
		
		'INT UNSIGNED NOT NULL' => array
		(
			'update_id',
		),
		
		'DATETIME'=> array
		(
			'date_updated',
		),

		'PRIMARY KEY'=> array
		(
			'update_id',
		),
	);

?>
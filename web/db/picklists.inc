<?php

	return array
	(
		'table_name'      => 'picklists',

		'INT UNSIGNED NOT NULL AUTO_INCREMENT' => array
		(
			'pick_val_id',
		),

		'INT UNSIGNED' => array
		(
			'sort_order'
		),
		
		'VARCHAR (255)' => array
		(
			'picklist_name',
			'value'
		),
		
		'PRIMARY KEY'=> array
		(
			'pick_val_id',
		),
	);

?>

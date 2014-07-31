<?php

	return array
	(
		'table_name'      => 'picklists_lookup',
		
		'INT UNSIGNED' => array
		(
			'pick_val_id',
			'owner_id'
		),
				
		'PRIMARY KEY'=> array
		(
			'pick_val_id,owner_id',
		),
	);

?>

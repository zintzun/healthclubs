<?php

	/**
	 * Table structure for users
	 */

	return array
	(
		'table_name'      => 'billing_history',

		'INT UNSIGNED NOT NULL AUTO_INCREMENT' => array
		(
			'payment_id',
		),

		'ENUM' => array
		(
			'status' => array 
			(
				'values'  => array ('initialized','processed'),
				'default' => 'initialized',
			),
		),
		
		'INT UNSIGNED DEFAULT 0' => array
		(
			'user_id',
			'leads_total',
			'leads_credited',
			'leads_billed',
		),

		'VARCHAR (32)' => array
		(
			'billing_month',
		),
		
		'VARCHAR (255)' => array
		(
			'biz_name',
			'description',
			'pp_pnref',
			'pp_respmsg',
			'pp_result',
			'pp_authcode',
			'pp_avsaddr',
			'pp_avszip',
			'error'
		),
		
		'FLOAT' => array
		(
			'billing_amount',
			'lead_rate'
		),

		'PRIMARY KEY'=> array
		(
			'payment_id',
		),
	);

?>
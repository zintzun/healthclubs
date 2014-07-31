<?php

	return array
	(
		'table_name'      => 'businesses',

		'INT UNSIGNED NOT NULL AUTO_INCREMENT' => array
		(
			'biz_id',
		),
		
		'INT UNSIGNED DEFAULT 0' => array
		(
			// Only used during business initialisation process
			// to get a handle on a biz if somone makes one and the
			// session ends, and they loging again it picks it up
			'creator_id',
		),
		
		'TINYINT UNSIGNED DEFAULT 2' => array
		(
			'coupon_expires',
		),

		'VARCHAR (255)' => array
		(
			'name',
			'street',
			'street2',
			'city',
			'state',
			'zip',
			'phone',
			'homepage',
			'hours',
			'email',
			'type',
			'fax',
			'paypal_email',
			'coupon_terms'
		),

		'VARCHAR (20)' => array
		(
			'latitude',
			'longitude',
		),

		'TEXT' => array
		(
			'description',
			'search_bucket'
		),

		'DATETIME'=> array
		(
			'date_updated',
		),

		'ENUM' => array
		(
			'geocode_status' => array 
			(
				'values'  => array ('un_retrieved','retrieve_success','retrieve_fail'),
				'default' => 'un_retrieved',
			),
			
			'cost_membership_timeframe' => array 
			(
				'values'  => array ('day','week','month','year'),
				'default' => 'month',
			),
			
			'cost_guest_timeframe' => array 
			(
				'values'  => array ('day','week','month'),
				'default' => 'month',
			),
			
			'coupon_timeframe' => array 
			(
				'values'  => array ('days','weeks','months','years'),
				'default' => 'months',
			),
		),

		'FLOAT DEFAULT 0'=> array
		(
			'cost_membership',
			'cost_guest',
			'cost_to_join',
		),

		'FULLTEXT'=> array
		(
			'search_bucket',
		),
		
		'BOOL DEFAULT 0' => array
		(
			'is_premium',
			'is_live',
			'is_24_hour',
			'is_appointment_only',
			'is_ihrsa',
		),
		
		'TINYINT UNSIGNED DEFAULT 6' => array
		(
			// Hours for this business
			'mon_open',
			'tue_open',
			'wed_open',
			'thu_open',
			'fri_open',
			'sat_open',
			'sun_open',
		),
		
		'TINYINT UNSIGNED DEFAULT 10' => array
		(
			// Hours for this business
			'mon_close',
			'tue_close',
			'wed_close',
			'thu_close',
			'fri_close',
			'sat_close',
			'sun_close',
		),

		'MEDIUMINT UNSIGNED DEFAULT 0' => array
		(
			'coupon_id',
		),

		'KEY'=> array
		(
			'city',
			'state',
			'creator_id'
		),

		'PRIMARY KEY'=> array
		(
			'biz_id',
		),
	);

?>

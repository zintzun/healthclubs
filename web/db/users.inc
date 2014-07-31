<?php

	/**
	 * Table structure for users
	 */

	return array
	(
		'table_name'      => 'users',

		'INT UNSIGNED NOT NULL AUTO_INCREMENT' => array
		(
			'user_id',
		),

		'VARCHAR (32)' => array
		(
			'login',
			'short_secret',
			'long_secret',
			'password',
		),

		'VARCHAR (255)' => array
		(
			'email',
			'url',
			'first_name',
			'last_name',
			'biz_name',
			'biz_position',
			'phone',
			'pp_last_tid',
		),
		
		'ENUM' => array
		(
			'approval_status' => array 
			(
				'values'  => array ('new_user','edited_details','approved'),
				'default' => 'new_user',
			),
		),

		'BOOLEAN DEFAULT 0'=> array
		(
			'is_biz_owner',
			'is_admin',
			'is_validated',
			'on_mailing_list',
		),

		'DATETIME'=> array
		(
			'last_login',
			'last_updated',
			'creation_date',
		),

		'FULLTEXT'=> array
		(
			'first_name,last_name,email,url,biz_name,biz_position,phone,login',
		),

		'PRIMARY KEY'=> array
		(
			'user_id',
		),
	);

?>
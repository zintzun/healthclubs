<?

	/**
	 * Table structure for users
	 */

	return array
	(
		'table_name'      => 'orders',

		'INT UNSIGNED NOT NULL AUTO_INCREMENT' => array
		(
			'order_id',
		),

		'INT UNSIGNED NOT NULL' => array
		(
			'user_id',
		),

		'REAL NOT NULL' => array
		(
			'amount',
		),

		'VARCHAR (32)' => array
		(
			'transaction_id',
			'correlation_id',
			'ack',
			'pimp_rc',
			'version',
		),

		'CHAR' => array
		(
			'avs_code',
			'cvv2_match',
		),

		'DATETIME'=> array
		(
			'timestamp',
		),

		'KEY'=> array
		(
			'transaction_id',
			'user_id',
		),

		'PRIMARY KEY'=> array
		(
			'order_id',
		),
	);

?>

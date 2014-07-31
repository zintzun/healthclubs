<?php

	include_once "interfaces/google_geocode.inc";

	/*
	* These are the fields that are involved in a new business
	*/
 
	$BIZ_FIELDS = array
	(
		'is_premium',
		'type',
		'name',
		'street',
		'city',
		'state',
		'zip',
		'phone',
		'homepage',
		'email',
		'description',
	);

	/*
	* Class to manage businesses & addresses
	*/

	class business
	{

		/*
		* Generic add_or_update
		*/

		function add_or_update($parms)
		{
			global $db;

			$need_this = array
			(
				'type',
				'name',
				'street',
				'city',
				'state',
				'zip',
				'description'
			);

			foreach ( $need_this as $field )
			{
				if ( ! isset($parms[$field]) || ! strlen($parms[$field] = trim($parms[$field])))
				{
					return "error: missing_$field";	
				}
			}

			if ( ! preg_match('/\d\d\d\d\d/',$parms['zip']) )
			{
				return "error: invalid_zip";	
			}

			if ( isset($parms['cost_membership_timeframe']) && ! in_array($parms['cost_membership_timeframe'],array('day','week','month','year')) )
			{
				return "error: invalid_cost_membership_timeframe";	
			}
			
			if ( isset($parms['cost_guest_timeframe']) && ! in_array($parms['cost_guest_timeframe'],array('day','week','month')) )
			{
				return "error: invalid_cost_guest_timeframe";	
			}

			$must_be_numeric = array
			(
				'cost_membership',
				'cost_guest',
				'cost_to_join',
			);
			
			foreach ( $must_be_numeric as $field )
			{
				if ( isset($parms[$field]) && trim($parms[$field]) != '' && ! is_numeric($parms[$field]) )
				{
					return "error: invalid_$field";	
				}
			}

			// really need to kill this if it's sent in as empty
			if ( isset($parms['biz_id']) && $parms['biz_id'] == '' )
			{
				unset($parms['biz_id']);	
			}

			$parms['date_updated'] = 'NOW()';
			
			// Create SET sql based on input array
			$set_sql = get_set_sql($parms);

			// If no biz_id OR no entry with this biz id
			if ( ! isset($parms['biz_id']) || ( isset($parms['biz_id']) && is_numeric($parms['biz_id']) && ! $db->get_var("SELECT count(*) FROM businesses WHERE biz_id = {$parms['biz_id']}") ) )
			{
				$db->query("INSERT INTO businesses SET $set_sql");
				$my_id = $db->insert_id;
			}
			// If entry exists
			else
			{				
				$db->query("UPDATE businesses SET $set_sql WHERE biz_id = {$parms['biz_id']}");
				$my_id = $parms['biz_id'];
			}
			
			// Get all data and insert it into the search_bucket
			$biz = $db->get_row("SELECT * FROM businesses WHERE biz_id = $my_id");
			$all_rows = (array) $biz;
			unset($all_rows['search_bucket']);
			unset($all_rows['geocode_status']);
			$db->query("UPDATE businesses SET search_bucket  = '".padd_keywords(implode(' ',$all_rows))."' WHERE biz_id = {$my_id}");

			// Attach geocode data to this address
			get_geocode($biz,$force=true);

			return $my_id;
		}

		/*
		* delete_business
		*/

		function delete_business($biz_id)
		{
			global $db;

			if ( !is_numeric($biz_id))
			{
				return 'error: invalid_biz_id';
			}
		
			$db->query('DELETE FROM biz_link_to_user WHERE biz_id = ' . $biz_id);
			return $db->query('DELETE FROM businesses WHERE biz_id = ' . $biz_id);
		}


		/*
		* get
		*/

		function get($biz_id)
		{
			global $db;
		
			if ( !is_numeric($biz_id))
			{
				return 'error: invalid_biz_id';
			}
		
			return $db->get_row('SELECT * FROM businesses WHERE biz_id = ' . $biz_id);
		}
	}

?>

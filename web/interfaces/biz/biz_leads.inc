<?php

	/*

	biz_lead Class
	. . . . . . . . . . . . . . . . . . . . . . . . . . . 
	Usage:

	biz_lead->add_or_update($parms)
	biz_lead->approve($lead_id)
	biz_lead->get($lead_id)
	biz_lead->get_leads_for_biz($biz_id)
	biz_lead->get_leads_by_biz_type($type)
	biz_lead->remove($lead_id)
	*/

	class biz_lead
	{
		/********************************************************************
		* biz_lead->add_or_update($parms)
		
			If lead_id is numeric, then update, otherwise  it is a new lead
			
			type: businees type, is a required parameter to create a new lead.
			
			Example of input array:
			$parms = array
			(
				'lead_id'.......=> 1,
				'biz_id'........=> 1,
				'type'..........=> 'health-clubs',
				'first_name'....=> 'justin-first',
				'last_name'.....=> 'vincent-last',
				'phone'.........=> '555-555-5555',
				'zip'...........=> '90210',
				'email'.........=> 'jv@jv.com',
				'coupon'........=> 'One Day Pass',
				'creation_date'.=> '2009-02-26 17:49:01',
				'i_am_18'.......=> '1',
				'is_approved'...=> 0
				);
		*/	

		public function add_or_update($parms)
		{
			global $LISTING_TYPES, $db;
			
			if ( isset($parms['email']) && ! $parms['email'] = trim($parms['email']) )
			{
				return 'error: invalid_email';
			}
			else if ( isset($parms['email']) && !eregi("^[_a-z0-9\'-]+(\.[_a-z0-9\'-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,4})$", $parms['email']) )
			{
				return 'error: invalid_email';
			}

			// Create SET sql based on input array
			$set_sql = get_set_sql($parms);
			
			// If entry exists update
			if ( isset($parms['lead_id']) && is_numeric($parms['lead_id']) )
			{
				// Update only if lead with lead_id exists
				if ( ! $db->get_var("SELECT count(*) FROM biz_leads WHERE lead_id = {$parms['lead_id']}") )
				{
					return "error: lead_id_not_found";
				}
				
				// Update biz_lead
				$db->query("UPDATE biz_leads SET $set_sql,last_updated = NOW() WHERE lead_id = {$parms['lead_id']}");
				
				return $parms['lead_id'];
			}
			else
			{
				
				// Type is a required parameter
				if ( ! isset($parms['type']) || ! ($parms['type'] = trim($parms['type'])) || ! in_array($parms['type'],$LISTING_TYPES) )
				{
					return 'error: invalid_type';
				}
				
				$parms['creation_date']  = 'NOW()';
				$parms['last_updated']  = 'NOW()';
				
				$set_sql = get_set_sql($parms);

				// New lead
				$db->query("INSERT INTO biz_leads SET $set_sql");
				
				return $db->insert_id;
			}
			
		}

		/********************************************************************
		* biz_lead->approve($lead_id)
		*/	
		
		public function approve($lead_id)
		{
			global $db;

			if ( ! is_numeric($lead_id) )
			{
				return "error: invalid_lead_id";
			}
			
			$db->query("UPDATE biz_leads SET is_approved = 1,last_updated = NOW() WHERE lead_id = '$lead_id'");
			
		}

		/********************************************************************
		* biz_lead->toggle_is_done($lead_id)
		*/

		public function toggle_is_done($lead_id)
		{
			global $db;
			
			if ( ! is_numeric($lead_id) )
			{
				return "error: invalid_lead_id";
			}

			$db->query("UPDATE biz_leads SET is_done = is_done XOR 1,last_updated = NOW() WHERE lead_id = '$lead_id'");
		  
			return $db->get_var("SELECT is_done FROM biz_leads WHERE lead_id = ".$_POST['lead_id']);

		}
		
		/********************************************************************
		* biz_lead->toggle_is_done($lead_id)
		*/

		public function toggle_credit_lead($lead_id)
		{
			global $db;
			
			if ( ! is_numeric($lead_id) )
			{
				return "error: invalid_lead_id";
			}

			$db->query("UPDATE biz_leads SET credit_lead = credit_lead XOR 1,last_updated = NOW() WHERE lead_id = '$lead_id'");

			return $db->get_var("SELECT credit_lead FROM biz_leads WHERE lead_id = ".$_POST['lead_id']);

		}

		/********************************************************************
		* biz_lead->remove($lead_id)
		*/	
		
		public function remove($lead_id)
		{
			global $db;
			
			if ( ! is_numeric($lead_id) )
			{
				return "error: invalid_lead_id";
			}
			
			return $db->query("DELETE FROM biz_leads WHERE lead_id = $lead_id");
		}
		
		/********************************************************************
		* biz_lead->get($lead_id)
		*/	
		
		public function get($lead_id)
		{
			global $db;
			
			if ( ! is_numeric($lead_id) )
			{
				return "error: invalid_lead_id";
			}
			
			if (! ($biz_lead = $db->get_row("SELECT * FROM biz_leads WHERE lead_id = '$lead_id'") ))
			{
				return "error: biz_lead_not_found";
			}
			
			return $biz_lead;
			
		}

		/********************************************************************
		* biz_lead->get_leads_for_biz($lead_id)
		*/	
				
		public function get_leads_for_biz($biz_id = false)
		{
			global $db;
			
			if ( ! is_numeric($biz_id) )
			{
				return "error: invalid_biz_id";
			}
			
			return $db->get_results("SELECT * FROM biz_leads WHERE biz_id = $biz_id");
			
		}

		/********************************************************************
		* biz_lead->get_leads_by_biz_type($biz_type)
		*/	
				
		public function get_leads_by_biz_type($biz_type = false, $search = false)
		{
			global $db;
			
			if ( ! $biz_type = trim($biz_type) )
			{
				return "error: no_biz_type";
			}
			
			$TYPE_SQL  = '';
			$MATCH_SQL = '';
			$WHERE_SQL = '';
			$ORDER_BY  = '';
			
			if ( $search = $db->escape(trim($search)) )
			{
				$MATCH_SQL = ", MATCH (bl.first_name,bl.last_name,bl.phone,bl.zip,bl.email,bl.coupon) AGAINST ('$search') AS rating";
				$WHERE_SQL = "MATCH (bl.first_name,bl.last_name,bl.phone,bl.zip,bl.email,bl.coupon) AGAINST ('$search' IN BOOLEAN MODE)";
				$ORDER_BY  = 'ORDER BY rating DESC';
			}
			else
			{
				$TYPE_SQL = "bl.type = '".$db->escape($biz_type)."'";
			}
			
			$SQL = 
			"
			SELECT 
				*,
				bl.*
				$MATCH_SQL 
			FROM 
				biz_leads bl
			JOIN businesses ON bl.biz_id = businesses.biz_id
			WHERE 
				$WHERE_SQL 
				$TYPE_SQL 
				$ORDER_BY
			";

			// "SELECT * $MATCH_SQL FROM biz_leads WHERE $WHERE_SQL $TYPE_SQL $ORDER_BY"
			return $db->get_results($SQL);
			
		}
	}
?>

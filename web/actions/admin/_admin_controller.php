<?php

	include "lib/state_utils.inc";
	include "interfaces/biz/biz_details.inc";
	include "interfaces/biz/biz_reviews.inc";
	include "interfaces/picklist.inc";
	include "interfaces/biz/biz_leads.inc";
	include "interfaces/biz/biz_link_to_user.inc";

	/*
	* Dealing with all admin related functions for this website
	*/

	class admin_controller
	{

		/*
		* Default action for /admin
		*/

		public function admin_index()
		{
			// If already logged in then send em in
			if ( session_is_admin() )
			{
				notify_redirect('/admin/home');
			}
			else
			{
				notify_redirect('/member/login');
			}
		}
		
		/*
		* After logging in to admin, this is the page you get
		*/
		
		public function admin_home()
		{
			global $db;
			
			$this->_validate();
			
			$today = "DATE_SUB(CURDATE(),INTERVAL 1 DAY) <= creation_date";
			
			$stats = array 
			(
				// Todo 
				'todo_leads'     => "biz_leads WHERE is_approved = 0",
				'todo_biz'       => "users WHERE is_biz_owner = 1 AND approval_status = 'new_user'",
				'todo_u_biz'     => "users WHERE is_biz_owner = 1 AND approval_status = 'edited_details'",
				'todo_members'   => "users WHERE is_biz_owner = 0 AND approval_status != 'approved'",
				'todo_u_members' => "users WHERE is_biz_owner = 0 AND approval_status = 'edited_details'",
				'todo_claims'    => "biz_link_to_user WHERE is_linked = 0",
				'todo_reviews'   => "reviews WHERE is_approved = 0",
				// Todays Activity
				'todays_leads'   => "biz_leads WHERE ".$today,
				'todays_biz'     => "users WHERE is_biz_owner = 1 AND ".$today,
				'todays_members' => "users WHERE is_biz_owner = 0 AND is_biz_owner = 0 AND ".$today,
				'todays_claims'  => "biz_link_to_user WHERE ".$today,
				'todays_reviews' => "reviews WHERE ".$today,
				// Leads Stats
				
				//'last_weeks_leads'   => "biz_leads WHERE DATE_SUB(CURDATE(),INTERVAL 1 WEEK) <= creation_date",
				'last_weeks_leads'   => "biz_leads WHERE creation_date >= DATE_SUB(CURDATE(),INTERVAL 1 WEEK)",
				'current_months_leads' => "biz_leads WHERE creation_date >= DATE_ADD(LAST_DAY(DATE_SUB(LAST_DAY(CURDATE()),INTERVAL 1 MONTH)),INTERVAL 1 DAY)",
				'last_months_leads' => "biz_leads WHERE creation_date > DATE_ADD(LAST_DAY(DATE_SUB(LAST_DAY(DATE_SUB(CURDATE(),INTERVAL 1 MONTH)),INTERVAL 1 MONTH)),INTERVAL 1 DAY) AND creation_date <= LAST_DAY(DATE_SUB(LAST_DAY(CURDATE()),INTERVAL 1 MONTH))",	
				'all_time' => 'biz_leads',
			);
			
			foreach ($stats as $field => $sql)
			{
				$this->v->stats[$field] = $db->get_var("SELECT count(*) FROM ".$sql);
			}
		}
		
		private function _get_global_rate()
		{
			/*
			* Dealing with setting the global lead rate
			*/

			if ( isset($_REQUEST['set-global-rate']) )
			{
				if ( ! is_numeric($_REQUEST['global-rate']) )
				{
					error_redirect("/admin/billing","Global rate must be a float/number");	
				}
				
				set_pref('global-lead-rate',$_REQUEST['global-rate']);
				$GLOBAL_LEAD_RATE = $_REQUEST['global-rate'];

				// Update leads for anyone in current month who is on global lead rate
				global $db;

				if ( $users = $db->get_col("SELECT user_id FROM users WHERE lead_rate = 0") )
				{
					foreach ( $users as $user_id )
					{
						// Get all the biz for this user then alter lead rates based on those id'
						if ( $businesses = $db->get_results("SELECT * FROM biz_link_to_user WHERE user_id = {$user_id}") )
						{
							foreach ($businesses as $business)
							{
								$db->query("UPDATE biz_leads AS bl SET bl.lead_rate = '$GLOBAL_LEAD_RATE' WHERE biz_id = '{$business->biz_id}' AND " . get_range_sql(0));
							}
						}
					}
				}
				
			}
			
			if ( ! $GLOBAL_LEAD_RATE = get_pref('global-lead-rate') )
			{
				$GLOBAL_LEAD_RATE = 4.99;
			}
			
			return $GLOBAL_LEAD_RATE;
		}

		public function admin_billing_errors()
		{
			global $db;
			$this->v->errors = $db->get_results("SELECT * FROM billing_history AS bh JOIN users AS u ON bh.user_id=u.user_id WHERE bh.error IS NOT NULL AND bh.error != 'Success'");

			if ( isset($_GET['rebill']) && $_GET['rebill'] == 'true' && isset($_GET['pid']) && is_numeric($_GET['pid']) )
			{
				if ( $payment = $db->get_row("SELECT * FROM billing_history WHERE payment_id = '{$_GET['pid']}' ") )
				{
					if ( ! trim($payment->error) )
					{
						error_me("This transaction appears to already be successfully processed (where payment_id = {$_GET['pid']})");
					}
					
					include "interfaces/paypal.inc";
					$paypal = new paypal;
					$paypal->do_ref_transaction($payment->user_id,$payment->billing_amount,$payment->payment_id);
					notify_me("Transaction processed! If the error record no longer exists below, then it worked!");
				}
				else
				{
					error_me("Could not find record in billing history with payment_id = {$_GET['pid']}");
					exit;
				}

			}


		}

		public function admin_billing_show_custom_rate_users()
		{
			global $db, $TARGET;

			$this->_validate();
			
			// Now, for display, we only want with full filter (including letter)
			$this->v->users = $db->get_results("SELECT * FROM users WHERE lead_rate != 0 ORDER BY biz_name");
			


		}

		public function admin_billing()
		{
			global $db, $TARGET;

			$this->_validate();
			
			include "actions/admin/admin_billing.func";
	
			$this->v->global_rate = $GLOBAL_LEAD_RATE = $this->_get_global_rate();
			
			/*
			* Dealing with the search stuff
			*/

			// Set date range to select leads by
			$this->v->range = isset($_REQUEST['range']) && is_numeric($_REQUEST['range']) ? $_REQUEST['range'] : 0;
	
			$RANGE_SQL = get_range_sql($this->v->range);
	
			// This filters by letter
			$LETTER_SQL = '';
			$this->v->letter = '';
			if ( isset($_REQUEST['letter']) && $_REQUEST['letter'])
			{
				$LETTER_SQL = "AND u.biz_name LIKE '{$_REQUEST['letter']}%'";
				$this->v->letter = $_REQUEST['letter'];
			}
			
			// Tis is search related
			if ( isset($_REQUEST['clear-search']) )
			{
				$_REQUEST['bill-search'] = '';
			}
			$SEARCH_SQL = '';
			if ( isset($_REQUEST['bill-search']) && $_REQUEST['bill-search'])
			{
				$SEARCH_SQL = "AND u.biz_name LIKE '%{$_REQUEST['bill-search']}%'";
			}
			
			$SHOW_CUSTOM_SQL = '';
			if ( isset($_REQUEST['show-custom']) && $_REQUEST['show-custom'] == 'true' )
			{
				$SHOW_CUSTOM_SQL = "AND u.lead_rate > 0";
			}

			$this->v->show_custom = (isset($_REQUEST['show-custom'])?$_REQUEST['show-custom']:'');
			$this->v->bill_search = (isset($_REQUEST['bill-search'])?$_REQUEST['bill-search']:'');

			// Variables used for output
			$this->v->leads = '';
			$this->v->letter_bar = '';
			$letters = array('a'=>'','b'=>'','c'=>'','d'=>'','e'=>'','f'=>'','g'=>'','h'=>'','i'=>'','j'=>'','k'=>'','l'=>'','m'=>'','n'=>'','o'=>'','p'=>'','q'=>'','r'=>'','s'=>'','t'=>'','u'=>'','v'=>'','w'=>'','x'=>'','y'=>'','z'=>'');
			
			// This gets all the leads for the specified range
			if ( $leads_for_range = $db->get_results("SELECT u.user_id, u.biz_name, count(b.biz_id) num_leads, SUM(bl.lead_rate) AS amount FROM biz_leads bl LEFT JOIN businesses b ON bl.biz_id=b.biz_id LEFT JOIN biz_link_to_user k ON bl.biz_id=k.biz_id LEFT JOIN users u ON k.user_id=u.user_id WHERE bl.is_approved = 1 AND u.user_id is NOT NULL AND $RANGE_SQL $SEARCH_SQL $SHOW_CUSTOM_SQL GROUP BY u.user_id ORDER BY u.biz_name") )
			{

				// All leads are used to build letter bar
				foreach ( $leads_for_range as $lead )
				{
					$letters[strtolower($lead->biz_name[0])] = true;
				}
				
				foreach ($letters as $letter => $is_set )
				{
					$letter = strtoupper($letter);
					if ( $is_set )
					{
						$this->v->letter_bar .= '<a href="/'.$TARGET->action_orig.'?letter='.$letter.'&range='.$this->v->range.'&bill-search='.$this->v->bill_search.'&show-custom='.$this->v->show_custom.'">'.$letter.'</a> ';
					}
					else
					{
						$this->v->letter_bar .= '<span>'.$letter.'</span> ';	
					}
				}
				
				$this->v->letter_bar .= '<span class="small" >(<a style="font-weight: normal;" href="/'.$TARGET->action_orig.'?letter=&range='.$this->v->range.'&bill-search='.$this->v->bill_search.'&show-custom='.$this->v->show_custom.'">clear</a>)</span>';
				
				// Now, for display, we only want with full filter (including letter)
				$this->v->leads = $db->get_results("SELECT u.user_id, u.biz_name, count(b.biz_id) num_leads, SUM(bl.lead_rate) - (SUM(bl.credit_lead) * bl.lead_rate) AS amount FROM biz_leads bl LEFT JOIN businesses b ON bl.biz_id=b.biz_id LEFT JOIN biz_link_to_user k ON bl.biz_id=k.biz_id LEFT JOIN users u ON k.user_id=u.user_id WHERE bl.is_approved = 1 AND u.user_id is NOT NULL AND $RANGE_SQL $SEARCH_SQL $LETTER_SQL $SHOW_CUSTOM_SQL GROUP BY u.user_id ORDER BY u.biz_name");

				/*
				* This deals with the CSV export
				*/

				if ( isset($_REQUEST['export']) && $this->v->leads )
				{

					header('Content-type: application/octet-stream');
					header('Content-Disposition: attachment; filename="admin-leads-overview.csv"');
					
					$keys = array();
					
					foreach ( $this->v->leads as $idx => $lead )
					{
						$cols = '';
						$vals = '';
						
						foreach ( get_object_vars($lead) as $k => $v )
						{
							$k = ucwords(str_replace('_',' ',$k));
							$cols .=	"$k, ";
							$vals .=	"$v, ";
							$keys[]=$k;
						}
						
						if ( $idx == 0 )
						{
							print substr($cols,0,-2)."\n";
						}
						print substr($vals,0,-2)."\n";
						
					}

					exit;
				}
			
			}

		}
		
		/*
		* Admin Billing Detail
		*/

		public function admin_billing_detail()
		{
			global $db, $TARGET;
			
			$this->_validate();
			
			include "actions/admin/admin_billing.func";
			
			if ( ! isset($TARGET->arg3) || ! is_numeric($TARGET->arg3) || ! $account = $db->get_row("SELECT * FROM users WHERE user_id = '{$TARGET->arg3}'") )
			{
				notify_redirect('/admin/billing','Invalid Account ID');	
			}

			/*
			* Dealing with setting the custom lead rate
			*/

			if ( isset($_REQUEST['set-custom-rate']) )
			{
				if ( ! is_numeric($_REQUEST['custom-rate']) )
				{
					error_redirect("/admin/billing-detail/{$TARGET->arg3}","Custom rate must be a float/number");	
				}

				if ( $_REQUEST['custom-rate'] == 0 )
				{
					$use_lead_rate = $this->_get_global_rate();

					// Reset to global lead rate by setting to 0
					$db->query("UPDATE users SET lead_rate = '0' WHERE user_id = '{$TARGET->arg3}'");
				}
				else
				{
					$use_lead_rate = $_REQUEST['custom-rate'];
					
					// Use custom lead rate
					$db->query("UPDATE users SET lead_rate = '{$use_lead_rate}' WHERE user_id = '{$TARGET->arg3}'");
				}
				
				// Get all the biz for this user then alter lead rates based on those id'
				if ( $businesses = $db->get_results("SELECT * FROM biz_link_to_user WHERE user_id = {$TARGET->arg3}") )
				{
					foreach ($businesses as $business)
					{
						$db->query("UPDATE biz_leads AS bl SET bl.lead_rate = '{$use_lead_rate}' WHERE biz_id = '{$business->biz_id}' AND " . get_range_sql(0));
					}
				}
				
				$account->lead_rate = $use_lead_rate;
			}
			
			$this->v->biz_name     = $account->biz_name;
			$this->v->biz_position = $account->biz_position;
			$this->v->first_name   = $account->first_name;
			$this->v->last_name    = $account->last_name;
			$this->v->email        = $account->email;
			$this->v->phone        = $account->phone;
			$this->v->lead_rate    = $account->lead_rate;
			
			/*
			* Dealing with results query
			*/

			// Set date range to select leads by
			$this->v->range = isset($_REQUEST['range']) && is_numeric($_REQUEST['range']) ? $_REQUEST['range'] : 0;
			$RANGE_SQL      = get_range_sql($this->v->range);

			$GLOBAL_LEAD_RATE = $this->_get_global_rate();
			
			$this->v->lead_total = 0;
			$this->v->leads = '';

			// Now, for display, we only want with full filter (including letter)
			if ( $results = $db->get_results("SELECT bl.credit_lead, u.user_id, bl.biz_id, bl.lead_id, b.name, bl.lead_rate, bl.first_name, bl.last_name, bl.phone, bl.email, bl.zip, bl.coupon, bl.creation_date date, b.street biz_street, b.city biz_city, b.state biz_state, b.zip biz_zip FROM biz_leads bl LEFT JOIN businesses b ON bl.biz_id=b.biz_id LEFT JOIN biz_link_to_user k ON bl.biz_id=k.biz_id LEFT JOIN users u ON k.user_id=u.user_id WHERE k.user_id={$TARGET->arg3} AND bl.is_approved = 1 AND u.user_id is NOT NULL AND $RANGE_SQL ORDER BY b.name ASC, bl.lead_id DESC") )
			{

				$leads = array();

				foreach ($results as $result)
				{
					if ( ! $result->credit_lead )
					{
						$this->v->lead_total += $result->lead_rate;
					}
					$leads[$result->biz_id][] = $result;
				}
				// $db->debug();
				$this->v->leads = $leads;

			}

		}
				
		/*
		* This is the page used to update businesses
		*/
		
		public function admin_business()
		{
			global $TARGET, $db, $BIZ_FIELDS, $LISTING_TYPES;
			
			// Hack needed to give LISTING_TYPES same key as values
			$new_lt = array();
			foreach ( $LISTING_TYPES as $k => $v )
			{
				$new_lt[$v] = $v;
			}
			$LISTING_TYPES = $new_lt;
			
			$this->_validate();

			$this->_maybe_delete_business('/admin/business','Successfully deleted business');

			// If this is an edit (or add)
			if ( isset($_POST['do_add_edit']) )
			{
		
				// Sets all the parms ready for update function
				$parms = array();
				foreach ( $BIZ_FIELDS as $field )
				{
					$parms[$field] = isset($_POST[$field])?$_POST[$field]:'';
				}

				if ( isset($parms['name']) )
				{
					$parms['name'] = str_replace('/','-',$parms['name']);
					$_SESSION['LAST_POST']['name'] = $parms['name'];
				}

				if (isset($_POST['type']))
				{
					$parms['type'] =  $LISTING_TYPES[$_POST['type']];
				}

				// If existing record (& not an insert)
				if ( isset($TARGET->arg3) && is_numeric($TARGET->arg3) )
				{
					$parms['biz_id'] = $TARGET->arg3;
				}
				
				// Do it! :)
				$business = new business;
				if ( $error = is_error($biz_id = $business->add_or_update($parms)) )
				{
					error_redirect('/admin/business',$error);
				}
		
				notify_redirect('/admin/business/'.$biz_id);
			}

			// If editing a business - note also doubles up as structure for form
			if ( isset($TARGET->arg3) && is_numeric($TARGET->arg3) && $EDIT_BIZ = $db->get_row("SELECT * FROM businesses WHERE biz_id = {$TARGET->arg3}") )
			{
				// Rather clever bit of script to instantly populate post values
				$_POST = (array) $EDIT_BIZ;
			}
			
		}

		/*
		* This is the page used to fix geocodes
		*/
		
		public function admin_fix_geocodes()
		{

			global $TARGET, $db;
			
			$this->_validate();

			$this->_maybe_delete_business('/admin/fix-geocodes','Successfully deleted business');

			/*
			* If a record has been edited lets try to fix the geocode stuff
			*/
		
			if ( isset($TARGET->arg3) && is_numeric($TARGET->arg3) && isset($_POST['do_update']) )
			{

				$business = new business;

				// Update this business
				$error = is_error
				(
					$business->add_or_update
					(
						array
						(
							'name'   => $_POST['name'],
							'street' => $_POST['street'],
							'zip'    => $_POST['zip'],
							'state'  => $_POST['state'],
							'city'   => $_POST['city'],
							'biz_id' => $TARGET->arg3,
							'type'   => $_POST['type'],
						)
					)
				);
	
				if ( $error )
				{
					error_redirect('/'.$TARGET->action_orig,$error);
				}
				else
				{
					notify_redirect('/admin/fix-geocodes');
				}

			}
			
			/*
			* This loads the current bad addresses ready for display
			*/
		
			if ( $this->v->num_bad_addresses = $db->get_var("SELECT count(*) FROM businesses WHERE geocode_status = 'retrieve_fail'") )
			{
		
				$this->v->bad_addresses = $db->get_results("SELECT * FROM businesses WHERE geocode_status = 'retrieve_fail'");

				if ( isset($TARGET->arg3) && is_numeric($TARGET->arg3) )
				{
					$edit_id = $TARGET->arg3;
				}
				else
				{
					notify_redirect("/admin/fix-geocodes/".$this->v->bad_addresses[0]->biz_id);
				}
		
				$this->v->edit = $db->get_row("SELECT * FROM businesses WHERE biz_id = $edit_id");

			}
		}

		/*
		* Picklists administration
		*/
		
		public function admin_picklists()
		{
			global $TARGET, $db, $PICKLISTS;

			$this->_validate();
			
			/*
			* VALIDATE query string
			*/
	
			if (isset($TARGET->arg3) && isset($TARGET->arg4))
			{
				$picklist_name = $TARGET->arg3.'/'.$TARGET->arg4;

				// This shouldn't happen
				if ( ! in_array($picklist_name, $PICKLISTS))
				{
					error_redirect('/admin/picklists/'.$PICKLISTS[0]);
				}
			}
			else 
			{
				notify_redirect('/admin/picklists/'.$PICKLISTS[0]);
			}
			
			$picklist = new picklist_admin($picklist_name);

			/*
			* EDIT an item to the picklist
			*/

			if ( isset($TARGET->arg5) && is_numeric($TARGET->arg5) && isset($_POST['do_add_or_edit']))
			{
				
				if (! isset($_POST['item_value']) || ! ($_POST['item_value']=trim($_POST['item_value'])) )
				{
					error_redirect('/'.$TARGET->action_orig,'Please enter a value!');
				} 
				
				// Edit the value 
				$error = is_error($picklist->edit($TARGET->arg5,$_POST['item_value']));

				error_redirect('/admin/picklists/'.$picklist_name,$error);

			}

			/*
			* ADD an item to the picklist
			*/
						
			if (! isset($TARGET->arg5) && isset($_POST['do_add_or_edit']) )
			{
				
				if (! isset($_POST['item_value']) || ! ($_POST['item_value']=trim($_POST['item_value'])) )
				{
					error_redirect('/'.$TARGET->action_orig,'error: value_can_not_be_empty');
				} 
				
				//Add to picklist
				$error = is_error($picklist->add($_POST['item_value']));
				
				if ( $error )
				{
					error_redirect('/'.$TARGET->action_orig,$error);
				}
				else
				{
					notify_redirect('/'.$TARGET->action_orig);
				}
			}
			
			/*
			* GET VALUE TO BE EDITED FOR FORM DISPLAY
			*/
			
			if ( isset($TARGET->arg5) )
			{
				if (! is_numeric($TARGET->arg5) )
				{
					error_redirect('/admin/picklists/'.$PICKLISTS[0]);
				}
				
				$error = is_error($this->v->edit = $picklist->get($TARGET->arg5));
				
				if ( $error )
				{
					error_redirect('/'.$TARGET->action_orig,$error);
				}
			}

			/*
			* DELETE an item
			*/

			if (isset($_GET['delete']) && is_numeric($_GET['delete']) )
			{
				$picklist->remove($_GET['delete']);
			}

			/*
			* We pass ?bump_up=item_id for the item to be positioned up
			*/
						
			if (isset($_GET['bump_up']) && is_numeric($_GET['bump_up']) )
			{
				$picklist->bump_down($_GET['bump_up']);
			}

			/*
			* We pass ?bump_up=item_id for the item to be positioned down
			*/

			if (isset($_GET['bump_down']) && is_numeric($_GET['bump_down']) )
			{
				$picklist->bump_up($_GET['bump_down']);
			}
			
			/*
			* Always redraw after above operations. 
			* Retrieve all the items from the corresponding list
			*/
			
			$this->v->picklist_items = $picklist->get_all();
			
			$this->v->picklist = $picklist_name;
		}

		/*
		* This is the page to Approve/Delete reviews.
		*/
		
		public function admin_reviews()
		{
			global $TARGET, $db;
			
			$this->_validate();
			
			// review_type is  used in admin_reviews.htm to set the links to review/all or review/unapproved.
			$this->v->review_type = 'all';
		
			$unapproved_flag = '';
			
			/*
			* We use GET to delete a review, delete field is the review id
			*/
			if ( isset($_GET['delete']) && is_numeric($_GET['delete']) )
			{
				$review = new review;
				if ( ! is_numeric($error = $review->delete_review($_GET['delete']) ) )
				{
					notify_redirect('/'.$TARGET->action_orig,"error: Could not remove review ".$_GET['delete']);					
				}
				notify_redirect('/'.$TARGET->action_orig);
			
			}

			/*
			* Approve a review.  Get here when clicked to the approve icon.
			*/
			if ( isset($_GET['approved']) && $_GET['approved'] == 'true' && isset($TARGET->arg4) && is_numeric($TARGET->arg4) )
			{
				$review = new review;
				
				// Approve review
				$error = is_error
				(
					$review->add_or_update
					(
						array
						(
							'review_id'    => $TARGET->arg4,
							'is_approved'  => true
						)
					)
				);

				if ( $error )
				{
					error_redirect('/{$TARGET->action_orig}/{$TARGET->arg4}',$error);
				}
				else
				{
					notify_redirect('/admin/reviews/'.$TARGET->arg3);
				}
			}
			
			/*
			* Verify the 3rd argument is "unapproved" or "all".
			*/
			if ( !(isset($TARGET->arg3) && ( $TARGET->arg3 == 'unapproved' || $TARGET->arg3 == 'all') ))
			{
				notify_redirect("/admin/reviews/unapproved");
			}
			if ( $TARGET->arg3 == 'unapproved' )
			{
				$unapproved_flag = " WHERE is_approved = 0";
				
				$this->v->review_type = 'unapproved';
			}
			$review = new review;

			/*
			* POST is used to update a message. The argument is the review id.
			*/
			if ( isset($TARGET->arg4) && is_numeric($TARGET->arg4) && isset($_POST['do_update']) )
			{
				// Update this business review
				$error = is_error
				(
					$review->add_or_update
					(
						array
						(
							'review_id'    => $TARGET->arg4,
							'rating'       => $_POST['rating'],
							'email'        => $_POST['email'],
							'name'         => $_POST['name'],
							'review'       => $_POST['review'],
							'is_approved' => $_POST['is_approved']
						)
					)
				);

				if ( $error )
				{
					error_redirect('/'.$TARGET->action_orig,$error);
				}
				else
				{
					notify_redirect('/'.$TARGET->action_orig);
				}
			}

			if ( $this->v->reviews = $db->get_results("SELECT review_id,name, review, rating, email, is_approved FROM reviews" . $unapproved_flag) )
			{
				
				$this->v->edit_id = false;
				
				if ( isset($TARGET->arg4) && is_numeric($TARGET->arg4) )
				{
					$this->v->edit = $db->get_row('SELECT name, review, rating, email, is_approved FROM reviews WHERE review_id = '. $TARGET->arg4);
				}
			}
			
		}

		/*
		* Leads administration
		*/
		
		public function admin_leads()
		{
			global $TARGET, $LISTING_TYPES, $db;
			
			$biz_lead = new biz_lead();
			
			$this->_validate();
			
			/*
			* Verify the 3rd argument is a valid listing type.
			*/
			
			if ( !isset($TARGET->arg3) || ! in_array($TARGET->arg3,$LISTING_TYPES))
			{
				notify_redirect('/admin/leads/'.$LISTING_TYPES[0].'/unapproved');
			}
			
			$this->v->listing_type = $TARGET->arg3;
		
			/*
			* Verify the 4th argument is "unapproved" or "all".
			*/
			if ( !(isset($TARGET->arg4) && ( $TARGET->arg4 == 'unapproved' || $TARGET->arg4 == 'all') ))
			{
				notify_redirect('/admin/leads/'.$TARGET->arg3.'/unapproved');
			}
			
			$this->v->lead_type = $TARGET->arg4;

			/*
			* Delete a lead. Get[Delete] field is the lead id
			*/
			if ( isset($_GET['delete']) && is_numeric($_GET['delete']) )
			{
				$biz_lead->remove($_GET['delete']);
				
				notify_redirect('/'.$TARGET->action_orig);
			}

			/*
			* Approve a lead.
			*/
			
			if ( isset($_GET['approved']) && $_GET['approved'] == 'true' && isset($TARGET->arg5) && is_numeric($TARGET->arg5) )
			{
				// Approve lead
				$error = is_error
				(
					$biz_lead->add_or_update
					(
						array
						(
							'lead_id'    => $TARGET->arg5,
							'is_approved'  => true
						)
					)
				);
			
				if ( $error )
				{
					error_redirect('/{$TARGET->action_orig}/{$TARGET->arg4}',$error);
				}
				else
				{
				 	// Send  email to biz owner

					// Get biz-owner info.
					$user = $db->get_row('SELECT users.* FROM (users INNER JOIN biz_link_to_user on users.user_id=biz_link_to_user.user_id ) INNER JOIN biz_leads on biz_link_to_user.biz_id=biz_leads.biz_id WHERE biz_leads.lead_id='. $TARGET->arg5);
					// Get business name
					$biz_name = $db->get_var('SELECT businesses.name FROM (businesses INNER JOIN biz_link_to_user on businesses.biz_id=biz_link_to_user.biz_id ) INNER JOIN biz_leads on biz_link_to_user.biz_id=biz_leads.biz_id WHERE biz_leads.lead_id='. $TARGET->arg5);

					$lead = $biz_lead->get($TARGET->arg5);

					include "interfaces/mail.inc";
					$msg_parms = array
					(
						'from'              => APP_EMAIL,
						'first_name'        => $user->first_name,
						'last_name'         => $user->last_name,
						'biz_name'          => $biz_name,
						'lead_first_name'   => $lead->first_name,
						'lead_last_name'    => $lead->last_name,
						'lead_phone'        => $lead->phone,
						'lead_zip'          => $lead->zip,
						'is_18'             => $lead->i_am_18?'18 or older':'is a minor',
						'coupon'            => $lead->coupon,
					);
					send_mail_helper($user->email,"template/mail/lead-notify.txt",$msg_parms);
					send_mail_helper(SEND_LEAD_CC_TO,"template/mail/lead-notify.txt",$msg_parms);

					notify_redirect('/admin/leads/'.$TARGET->arg3.'/'.$TARGET->arg4);
				}
			}

			/*
			* Form POSTED to updated lead.
			*/
			
			if ( isset($TARGET->arg5) && is_numeric($TARGET->arg5) && isset($_POST['do_update_lead']) )
			{

				// Update this business review
				$error = is_error
				(
					$biz_lead->add_or_update
					(
						array
						(
							'lead_id'      => $TARGET->arg5,
							'first_name'   => $_POST['first_name'],
							'last_name'    => $_POST['last_name'],
							'email'        => $_POST['email'],
							'phone'        => $_POST['phone'],
							'zip'          => $_POST['zip'],
							'is_approved'  => $_POST['is_approved'],
							'coupon'       => $_POST['coupon']
						)
					)
				);

				if ( $error )
				{
					error_redirect('/'.$TARGET->action_orig,$error);
				}
				else
				{
					notify_redirect("/{$TARGET->arg1}/{$TARGET->arg2}/{$TARGET->arg3}/{$TARGET->arg4}");
				}
			}

			/*
			* Get all leads by type specified by argument3
			*/
			
			if ($this->v->leads = $biz_lead->get_leads_by_biz_type($TARGET->arg3, isset($_POST['lead_search'])? $_POST['lead_search'] : '' ) )
			{
					
				if ( isset($TARGET->arg5) && is_numeric($TARGET->arg5) && ! isset($_POST['do_search']) )
				{
					$this->v->edit = $biz_lead->get($TARGET->arg5);
					
					if ($this->v->edit)
					{
						$this->v->biz_name= $db->get_var("SELECT name FROM businesses WHERE biz_id = ".$this->v->edit->biz_id);
					}
				}
			}
		}
		
		
		/*
		* Members administration
		*/

		public function admin_members()
		{
			global $TARGET, $db;
			
			$user = new user();
			
			$this->_validate();
			
			$this->v->MEMBER_STATUS = array
			(
				'new_user/member'       => 'Member &raquo; Unapproved &raquo; New ',
				'edited_details/member' => 'Member &raquo; Unapproved &raquo; Updated',
				'approved/member'       => 'Member &raquo; Approved',
				'new_user/biz'          => 'Biz &raquo; Unapproved &raquo; New',
				'edited_details/biz'    => 'Biz &raquo; Unapproved &raquo; Updated',
				'approved/biz'          => 'Biz &raquo; Approved',
			);
						
			/*
			* Verify the 3rd argument is a valid user status.
			*/

			if ( !isset($TARGET->arg3) || ! in_array($TARGET->arg3.'/'.$TARGET->arg4,array_keys($this->v->MEMBER_STATUS)))
			{
				notify_redirect('/admin/members/new_user/member');
			}

			$this->v->status = $TARGET->arg3;
			$this->v->option = $this->v->MEMBER_STATUS[$TARGET->arg3.'/'.$TARGET->arg4];

			/*
			* Delete a user. Get[Delete] field is the user id
			*/
			if ( isset($_GET['delete']) && is_numeric($_GET['delete']) )
			{
				$user->remove($_GET['delete']);
				
				notify_redirect('/'.$TARGET->action_orig,'');
			}

			/*
			* Approve a user: change approval status to 'approved'
			*/
			
			if ( isset($_GET['approval_status']) && $_GET['approval_status'] == 'approved' && isset($TARGET->arg5) && is_numeric($TARGET->arg5) )
			{
				
				// Set approval status to 'approved'
				if ($error = is_error($user->update_value($TARGET->arg5, 'approval_status', 'approved')))
				{
					error_redirect('/{$TARGET->action_orig}/{$TARGET->arg5}',$error);
				}
				else
				{
					notify_redirect('/admin/members/'.$TARGET->arg3.'/'.$TARGET->arg4);
				}
			}

			/*
			* Form POSTED to updated user.
			*/
			
			if ( isset($TARGET->arg5) && is_numeric($TARGET->arg5) && isset($_POST['do_update_user']) )
			{

				$parms = make_parms_from_post
				(
					array
					(
						'login',
						'email',
						'url',
						'is_biz_owner',
						'on_mailing_list',
						'approval_status',
						'first_name',
						'last_name',
						'biz_name',
						'biz_position',
						'phone',
						'lead_rate'
					)
				);

				if ( ($_POST['password'] = trim($_POST['password']) ) )
				{
					$parms['password'] = md5($_POST['password']);
				}

				// Update this user
				if ( $error = is_error($user->update($TARGET->arg5,$parms)) )
				{
					error_redirect('/'.$TARGET->action_orig,$error);
				}
				else
				{
					notify_redirect("/{$TARGET->arg1}/{$TARGET->arg2}/{$TARGET->arg3}/{$TARGET->arg4}");
				}
			}
			
			/*
			* Get all users by approval status specified by argument3
			*/
			if ($this->v->users = $user->get_users($TARGET->arg3, isset($_POST['member_search'])? $_POST['member_search'] : '' , $TARGET->arg4 == 'biz' ? true : false) )
			{
				if ( isset($TARGET->arg5) && is_numeric($TARGET->arg5) && ! isset($_POST['do_search']) )
				{
					$this->v->edit = $user->get($TARGET->arg5);
				}
			}
			
		}
		
		/*
		* Listing Claims
		*/

		public function admin_listing_claims()
		{
			global $db, $TARGET;
			
			$biz_link_to_user = new biz_link_to_user();
			
			$this->_validate();
			
			$this->v->CLAIM_STATUS = array('new','approved');
			
			/*
			* Verify the 3rd argument is a valid claim status.
			*/

			if ( !isset($TARGET->arg3) || ! in_array($TARGET->arg3,$this->v->CLAIM_STATUS) )
			{
				notify_redirect('/admin/listing-claims/new');
			}

			$this->v->status = $TARGET->arg3;

			/*
			* Delete a claim. Get[Delete] field is the user_id-biz_id
			*/
			if ( isset($_GET['delete']))
			{
				list($user_id, $biz_id) = explode("-", $_GET['delete']);
				
				if ( is_numeric($user_id) && is_numeric($biz_id) )
				{
					$biz_link_to_user->un_approve($user_id,$biz_id);
					$biz_link_to_user->remove($user_id,$biz_id);
					notify_redirect('/'.$TARGET->action_orig);
				}
			}
			else if ( isset($_GET['approve']))
			{
				list($user_id, $biz_id) = explode("-", $_GET['approve']);
				
				if ( is_numeric($user_id) && is_numeric($biz_id) )
				{
					$biz_link_to_user->approve($user_id,$biz_id);
					notify_redirect('/'.$TARGET->action_orig);
				}
			}

			/*
			* Get all claims by claim status specified by argument 3
			*/
			$is_linked = array_search($TARGET->arg3, $this->v->CLAIM_STATUS); 
			
			$this->v->claims = $db->get_results("SELECT biz_link_to_user.biz_id,biz_link_to_user.user_id,users.login,users.is_biz_owner,users.approval_status,businesses.name FROM (biz_link_to_user INNER JOIN users on biz_link_to_user.user_id=users.user_id) INNER JOIN businesses on biz_link_to_user.biz_id=businesses.biz_id WHERE biz_link_to_user.is_linked = ".$is_linked);
			
		}

		
		/*
		* Link user <=> business
		*/

		public function admin_link_biz()
		{
			$this->_validate();
		}

		/*
		* Export Email
		*/

		public function admin_export_email()
		{
			global $db, $TARGET;
			
			// How many are on each mailing list?
			$this->v->num_on_member_mailing_list = $db->get_var("SELECT count(*) FROM users WHERE is_biz_owner = 0 AND is_validated = 1 AND on_mailing_list = 1");
			$this->v->num_on_biz_mailing_list    = $db->get_var("SELECT count(*) FROM users WHERE is_biz_owner = 1 AND is_validated = 1 AND on_mailing_list = 1");
	
			if ( isset($TARGET->arg3) && $TARGET->arg3 == 'member' )
			{
				header("Content-Type: text/plain");
				foreach ( $db->get_col("SELECT email FROM users WHERE is_biz_owner = 0 AND is_validated = 1 AND on_mailing_list = 1") as $email )
				{
					print "$email\n";	
				}
				exit;
			}
			
			if ( isset($TARGET->arg3) && $TARGET->arg3 == 'biz' )
			{
				header("Content-Type: text/plain");
				foreach ( $db->get_col("SELECT email FROM users WHERE is_biz_owner = 1 AND is_validated = 1 AND on_mailing_list = 1") as $email )
				{
					print "$email\n";	
				}
				exit;
			}
		
		}

		
		/*
		* Shared action used to delete a business
		*/

		private function _maybe_delete_business($success_action,$success_message)
		{
			if ( isset($_GET['delete']) && is_numeric($_GET['delete']) )
			{
				$business = new business;
				$business->delete_business($_GET['delete']);
				notify_redirect($success_action,$success_message);
			}
		}

		/*
		* Make sure it's an active admin session
		*/
		
		private function _validate()
		{
			if ( ! session_is_admin() )
			{
				notify_redirect('/member/login');
			}
		}
	
	}

?>

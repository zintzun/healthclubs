<?php
	// $db->trace = true;

	class cron_controller
	{
	
		public function __construct()
		{
			if ( ! session_is_admin() )
			{
				error_redirect('/');
			}
			
			if ( isset($_GET['process']) && $_GET['process'] == 'true' )
			{
				;
			}
			else
			{
				print '<div style="background: #eee; border: solid #ccc 1px; padding: 3px; position: absolute; margin: 10px; "><a href="?process=true" style="font-family: arial; color: #777; text-decoration: none; font-size: 12px;">Process Payments</a></div>';
				exit;
			}
		}
	
		public function cron_index()
		{
			exit;
		}

		public function cron_billing()
		{

			// - This script is called via http://server.com/cron/billing	
			// - This script can only be called on the first of each month

			if ( ! $this->can_run_billing_script() )
			{
				return;	
			}

			$this->generate_list_of_users_to_be_billed();
						
			$this->process_payments();		
			
			exit;

		}
		
		//  - 4) Loop through the list and process the payments
		private function process_payments()
		{
			global $db;
			
			if ( $payments = $db->get_results("SELECT * FROM billing_history WHERE status = 'initialized'") )
			{
			
				include "interfaces/paypal.inc";
				$paypal = new paypal;
				$paypal->debug = true;
				
				print "<h1>Send This To PayPal</h1><ul>";
				
				foreach ( $payments as $payment )
				{

					if ( $payment->billing_amount > 0 )
					{

						$paypal->do_ref_transaction($payment->user_id,$payment->billing_amount,$payment->payment_id);
	
						// This is where the real payment processing happens
						print "<li>$payment->biz_name - $payment->description</li>";

					}
					
				}

				print "</ul><hr>";

			}
			else
			{
				print "<h1>Nothing To Process</h1>";
			}

			?>
				<ul>
					<li><a href="/cron/billing-reset">Clear the <b>billing_history</b> table and run cron from scratch</a></li>
					<li><a href="/cron/billing">Re-run cron wihout clearing table to see if it double processes</a></li>
				</ul>
			<?

			$db->get_results("SELECT * FROM billing_history");
			$db->debug();
			
		}

		public function cron_billing_reset()
		{
			include "interfaces/schema_helpers.inc";
			create_table(include "db/billing_history.inc");
			notify_redirect('/cron/billing');
		}

		private function can_run_billing_script()
		{
			return date('j') == BILLING_DAY_OF_MONTH;
		}

		private function generate_list_of_users_to_be_billed()
		{
			global $db;

			// - 1) This script generates list of users to be billed, along with amounts
			if ( $results = $db->get_results($this->get_last_months_billing_summary_sql()) )
			{
				
				$debug_pp_pnref = '12345';
				
				foreach ( $results as $result )
				{
					// - 2) The list is inserted into a payment record table (one row per customer)
					if ( ! $db->get_var("SELECT count(*) FROM billing_history WHERE user_id = {$result->user_id} AND billing_month = '{$result->billing_month}'") )
					{
						$lead_rate = $result->lead_rate;

						//    - 3) The payment record table has the following format
						//       - payment_id
						//       - user_id
						//			- biz_name
						//			- leads_total
						//			- leads_credited
						//			- leads_billed
						//       - status = initialized/processed
						//       - description = (i.e 34 leads @ $4.99)
						//       - billing_amount
						//       - pp_pnref = paypal reference id
						//       - pp_respmsg
						//       - pp_authcode
						//       - pp_result
						//       - pp_authcode
						//       - pp_avsaddr
						//       - pp_avszip
						//
						
						$db->query("INSERT INTO billing_history SET pp_pnref = '$debug_pp_pnref', status = 'initialized', user_id = {$result->user_id}, billing_month = '{$result->billing_month}', biz_name = '".$db->escape($result->biz_name)."', leads_total = {$result->leads_total}, leads_credited = {$result->leads_credited}, leads_billed = {$result->leads_billed}, billing_amount = {$result->billing_amount}, lead_rate = $lead_rate, description = '".$db->escape("{$result->month_text} {$result->year_text} - {$result->leads_billed} leads @ \$$lead_rate - Total: \${$result->billing_amount}")."'");
					}
				}
			}
		}
		
		private function get_last_months_billing_summary_sql()
		{
			return "SELECT 
				u.user_id, 
				u.biz_name, 
				count(b.biz_id) leads_total, 
				SUM(bl.credit_lead) leads_credited, 
				count(b.biz_id) - SUM(bl.credit_lead) leads_billed, 
				ROUND(SUM(bl.lead_rate) - (SUM(bl.credit_lead) * bl.lead_rate),2) AS billing_amount,
				bl.lead_rate AS lead_rate,
				DATE_FORMAT(LAST_DAY(DATE_SUB(LAST_DAY(CURDATE()),INTERVAL 1 MONTH)),'%Y-%m')  billing_month,
				DATE_FORMAT(LAST_DAY(DATE_SUB(LAST_DAY(CURDATE()),INTERVAL 1 MONTH)),'%Y')  year_text,
				DATE_FORMAT(LAST_DAY(DATE_SUB(LAST_DAY(CURDATE()),INTERVAL 1 MONTH)),'%b')  month_text
			FROM 
				biz_leads bl 
			LEFT JOIN businesses b ON bl.biz_id=b.biz_id 
			LEFT JOIN biz_link_to_user k ON bl.biz_id=k.biz_id 
			LEFT JOIN users u ON k.user_id=u.user_id 
				WHERE 
					bl.is_approved = 1 
				AND 
					u.user_id is NOT NULL 
				AND 
					bl.creation_date >= DATE_ADD(LAST_DAY(DATE_SUB(LAST_DAY(DATE_SUB(CURDATE(),INTERVAL 1 MONTH)),INTERVAL 1 MONTH)),INTERVAL 1 DAY) 
				AND 
					bl.creation_date <= LAST_DAY(DATE_SUB(LAST_DAY(CURDATE()),INTERVAL 1 MONTH)) GROUP BY u.user_id ORDER BY u.biz_name
			";	
		}
		
	}

?>

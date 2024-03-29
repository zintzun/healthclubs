<?php
	/*
	* Toggles is_done status for lead
	*
	* Parameters expected:
	*
	* 	lead_id
	*/

	$error = 'OK';
  $is_done = '';

	if ( ! isset($_POST['lead_id']) )
	{
		$error = 'Unexpected error: Missing lead id.';
	}
	else if ( ! is_numeric($_POST['lead_id']) )
	{
			$error = 'Unexpected error: Invalid lead id.';
	}
	else
	{
		include "interfaces/biz/biz_leads.inc";
		
		$lead = new biz_lead;

		$is_credit = $lead->toggle_credit_lead($_POST['lead_id']);
	}
	
	print json_encode(array('error' => $error,'isCredit' => $is_credit));

?>

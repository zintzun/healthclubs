<?php

exit;

	// https://cms.paypal.com/cms_content/US/en_US/files/developer/PP_NVPAPI_DeveloperGuide.pdf
	include "interfaces/paypal.inc";

	$paypal = new paypal;

	/*
	* Parms for a sale
	*/

	$parms = array
	(
		'METHOD'            => 'DoDirectPayment',
		'PAYMENTACTION'     => 'Sale', // Authorization OR Sale  
		'CREDITCARDTYPE'    => 'Visa', // Visa, MasterCard, Discover, Amex, 
		'ACCT'              => '4772937262644918', // cc number
		'EXPDATE'           => '122019', // MMYYYY
		'CVV2'              => '123',
		'FIRSTNAME'         => 'Justin', // 25 single-byte characters
		'LASTNAME'          => 'Vincent', // 25 single-byte characters
		'AMT'               => '1',
		'CURRENCYCODE'      => 'USD',
		'SOFTDESCRIPTOR'    => 'Healthclubs.net', // On cc bill
	);

	$sale_result = $paypal->do_call($parms);

	/*
	* Parms for a reference transaction ID
	*/

	$parms = array
	(
		'METHOD'            => 'DoReferenceTransaction',
		'PAYMENTACTION'     => 'Sale',
		'REFERENCEID'       => $sale_result['TRANSACTIONID'], // Original auth ID
		'AMT'               => '4.99',
	);

	$reference_result = $paypal->do_call($parms);

	
	/*
	* Parms for a void
	*/

//	$parms = array
//	(
//		'METHOD'            => 'DoVoid',
//		'AUTHORIZATIONID'   => $sale_result['TRANSACTIONID'], // Original auth ID
//	);
//		
//	$void_result = $paypal->do_call($parms);
//	
	exit;

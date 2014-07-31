<?php

//print PPHttpPost('DoDirectPayment',);

$environment = 'sandbox';	// or 'beta-sandbox' or 'live'

/**
 * Send HTTP POST Request
 *
 * @param	string	The API method name
 * @param	string	The POST Message fields in &name=value pair format
 * @return	array	Parsed HTTP Response body
 */
function PPHttpPost($methodName_, $nvpStr_) {
	global $environment;

	//API Username:  	zintzu_1243016178_biz_api1.gmail.com
	//API Password: 	1243016185
	//Signature: 	ANXOQ4KpCB.JKCpHa3QVc85nEVERAF0GY2UeT9pUUUxixfGaMBZyZebw 

	$API_UserName = urlencode('zintzu_1243016178_biz_api1.gmail.com');
	$API_Password = urlencode('1243016185');
	$API_Signature = urlencode('ANXOQ4KpCB.JKCpHa3QVc85nEVERAF0GY2UeT9pUUUxixfGaMBZyZebw');
	//
	$API_UserName = urlencode('ian_1243029816_biz_api1.gmail.com');
	$API_Password = urlencode('1243029828');
	$API_Signature = urlencode('AFcWxV21C7fd0v3bYYYRCpSSRl31AhqpFTI3rGJ26PjM9iCS76x.KFdO');

	$API_Endpoint = "https://api-3t.paypal.com/nvp";
	if("sandbox" === $environment || "beta-sandbox" === $environment) {
		$API_Endpoint = "https://api-3t.$environment.paypal.com/nvp";
	}
	$version = urlencode('51.0');

	// setting the curl parameters.
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $API_Endpoint);
	curl_setopt($ch, CURLOPT_VERBOSE, 1);

	// turning off the server and peer verification(TrustManager Concept).
	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
	curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);

	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_POST, 1);

	// NVPRequest for submitting to server
	$nvpreq = "METHOD=$methodName_&VERSION=$version&PWD=$API_Password&USER=$API_UserName&SIGNATURE=$API_Signature$nvpStr_";

	// setting the nvpreq as POST FIELD to curl
	curl_setopt($ch, CURLOPT_POSTFIELDS, $nvpreq);

	// getting response from server
	$httpResponse = curl_exec($ch);

	if(!$httpResponse) {
		exit("$methodName_ failed: ".curl_error($ch).'('.curl_errno($ch).')');
	}

	// Extract the RefundTransaction response details
	$httpResponseAr = explode("&", $httpResponse);

	$httpParsedResponseAr = array();
	foreach ($httpResponseAr as $i => $value) {
		$tmpAr = explode("=", $value);
		if(sizeof($tmpAr) > 1) {
			$httpParsedResponseAr[$tmpAr[0]] = $tmpAr[1];
		}
	}

	if((0 == sizeof($httpParsedResponseAr)) || !array_key_exists('ACK', $httpParsedResponseAr)) {
		exit("Invalid HTTP Response for POST request($nvpreq) to $API_Endpoint.");
	}

	return $httpParsedResponseAr;
}

$token = urlencode("4590655962607288");
$token = urlencode("token_from_setExpressCheckout");
$paymentAmount = urlencode("100");
$currencyID = urlencode("USD");						// or other currency code ('GBP', 'EUR', 'JPY', 'CAD', 'AUD')
$startDate = urlencode("2009-5-23T0:0:0");
$billingPeriod = urlencode("Month");				// or "Day", "Week", "SemiMonth", "Year"
$billingFreq = urlencode("1");						// combination of this and billingPeriod must be at most a year

$nvpStr="&TOKEN=$token&AMT=$paymentAmount&CURRENCYCODE=$currencyID&PROFILESTARTDATE=$startDate";
$nvpStr .= "&BILLINGPERIOD=$billingPeriod&BILLINGFREQUENCY=$billingFreq";

//$httpParsedResponseAr = PPHttpPost('CreateRecurringPaymentsProfile', $nvpStr);
$httpParsedResponseAr = PPHttpPost('SetExpressCheckout', $nvpStr);

if("Success" == $httpParsedResponseAr["ACK"]) {
	return('CreateRecurringPaymentsProfile Completed Successfully: '.print_ar($httpParsedResponseAr, true));
} else  {
	return('CreateRecurringPaymentsProfile failed: ' . print_ar($httpParsedResponseAr, true));
}

?>


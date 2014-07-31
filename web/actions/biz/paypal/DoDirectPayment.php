<?php

/** DoDirectPayment NVP example; last modified 08MAY23.
 *
 *  Process a credit card payment. 
*/

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

	global $API_UserName, $API_Password, $API_Signature;

	// Set up your API credentials, PayPal end point, and API version.

	$API_Endpoint = "https://api-3t.paypal.com/nvp";
	if("sandbox" === $environment || "beta-sandbox" === $environment) {
		//$API_Endpoint = "https://api-3t.$environment.paypal.com/nvp";
		$API_Endpoint = "https://api-3t.sandbox.paypal.com/nvp";
	}
	$version = urlencode('57.0');

	// Set the curl parameters.
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $API_Endpoint);
	curl_setopt($ch, CURLOPT_VERBOSE, 1);

	// Turn off the server and peer verification (TrustManager Concept).
	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
	curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);

	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_POST, 1);

	// Set the API operation, version, and API signature in the request.
	$nvpreq = "METHOD=$methodName_&VERSION=$version&PWD=$API_Password&USER=$API_UserName&SIGNATURE=$API_Signature$nvpStr_";

	// Set the request as a POST FIELD for curl.
	curl_setopt($ch, CURLOPT_POSTFIELDS, $nvpreq);

	// Get response from the server.
	$httpResponse = curl_exec($ch);

	if(!$httpResponse) {
		exit("$methodName_ failed: ".curl_error($ch).'('.curl_errno($ch).')');
	}

	// Extract the response details.
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

// Set request-specific fields.
$paymentType = urlencode('Authorization');				// or 'Sale'
$firstName = urlencode('Justin');
$lastName = urlencode('vincent');
$creditCardType = urlencode('visa');
$creditCardNumber = urlencode('4590655962607288');
$creditCardNumber = urlencode('4111111111111111');
//personal account
$creditCardNumber = urlencode('4372827183821030');
$expDateMonth = '5';
// Month must be padded with leading zero
$padDateMonth = urlencode(str_pad($expDateMonth, 2, '0', STR_PAD_LEFT));

$expDateYear = urlencode('2019');
$cvv2Number = urlencode('289');
$address1 = urlencode('3161 Waverly Dr.');
$address2 = urlencode('');
$city = urlencode('Los Angeles');
$state = urlencode('CA');
$zip = urlencode('90027');
$country = urlencode('US');				// US or other valid country code
$amount = urlencode('15');
$currencyID = urlencode('USD');	// or other currency ('GBP', 'EUR', 'JPY', 'CAD', 'AUD')

// Add request-specific fields to the request string.
$nvpStr =	
	"&PAYMENTACTION=$paymentType".
	"&AMT=$amount".
	"&CREDITCARDTYPE=$creditCardType".
	"&ACCT=$creditCardNumber".
	"&EXPDATE=$padDateMonth$expDateYear".
	"&CVV2=$cvv2Number".
	"&FIRSTNAME=$firstName".
	"&LASTNAME=$lastName".
	"&STREET=$address1".
	"&CITY=$city&STATE=$state&ZIP=$zip&COUNTRYCODE=$country&CURRENCYCODE=$currencyID";
print_ar($nvpStr);

// Execute the API operation; see the PPHttpPost function above.
$httpParsedResponseAr = PPHttpPost('DoDirectPayment', $nvpStr);

if("Success" == $httpParsedResponseAr["ACK"]) {
	return('Direct Payment Completed Successfully: '.print_ar($httpParsedResponseAr, true));
} else  {
	return('DoDirectPayment failed: ' . print_ar($httpParsedResponseAr, true));
}

/*
Array
(
    [TIMESTAMP] => 2009%2d05%2d26T19%3a15%3a23Z
    [CORRELATIONID] => 2288c0b622bb2
    [ACK] => Success
    [VERSION] => 57%2e0
    [BUILD] => 904483
    [AMT] => 15%2e00
    [CURRENCYCODE] => USD
    [AVSCODE] => X
    [CVV2MATCH] => M
    [TRANSACTIONID] => 4LR667694H198705V
    [PIMP_RC] => 0
)
*/

?>

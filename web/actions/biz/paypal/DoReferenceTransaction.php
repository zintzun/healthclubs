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
$paymentType = urlencode('Sale');				// or 'Sale'
$amount = urlencode('35');
$referenceid = urlencode('4LR667694H198705V');

// Add request-specific fields to the request string.
$nvpStr =	
	"&PAYMENTACTION=$paymentType".
	"&AMT=$amount".
	"&REFERENCEID=$referenceid";
print_ar($nvpStr);

// Execute the API operation; see the PPHttpPost function above.
$httpParsedResponseAr = PPHttpPost('DoReferenceTransaction', $nvpStr);

if("Success" == $httpParsedResponseAr["ACK"]) {
	return('DoReferenceTransaction Payment Completed Successfully: '.print_ar($httpParsedResponseAr, true));
} else  {
	return('DoReferenceTransaction failed: ' . print_ar($httpParsedResponseAr, true));
}

/*
&PAYMENTACTION=Sale&AMT=35&REFERENCEID=4LR667694H198705V

Array
(
    [AVSCODE] => X
    [CVV2MATCH] => M
    [TIMESTAMP] => 2009%2d05%2d26T19%3a58%3a30Z
    [CORRELATIONID] => 17f98ebbc5e7
    [ACK] => Success
    [VERSION] => 57%2e0
    [BUILD] => 904483
    [TRANSACTIONID] => 8YP31624GC1931040
    [AMT] => 35%2e00
    [CURRENCYCODE] => USD
)

-------------------------------------------------------------- 
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
Array
(
    [TIMESTAMP] => 2009%2d05%2d26T19%3a57%3a25Z
    [CORRELATIONID] => 94a75a8092610
    [ACK] => Success
    [VERSION] => 57%2e0
    [BUILD] => 904483
    [AMT] => 15%2e00
    [CURRENCYCODE] => USD
    [AVSCODE] => X
    [CVV2MATCH] => M
    [TRANSACTIONID] => 1N053694H29412740
    [PIMP_RC] => 0
)

*/

?>

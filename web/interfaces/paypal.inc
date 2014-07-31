<?php


	class paypal
	{

		var $data = false;

		function __construct($target_user=false)
		{
		}

		function save_order($user_id)
		{

			global $db;

			//foreach( array( 'user_id', 'amount', 'transaction_id', 'correlation_id', 'ack', 'version', 'avs_code', 'cvv2_match', 'timestamp', 'pimp_rc',))
			$sql = 
				"INSERT into orders ".
				"SET".
				" user_id        ='" .$user_id."'".
				",transaction_id ='" .urldecode($this->response['TRANSACTIONID'])."'".
				",correlation_id ='" .urldecode($this->response['CORRELATIONID'])."'".
				",amount         ='" .urldecode($this->response['AMT'])."'".
				",ack            ='" .urldecode($this->response['ACK'])."'".
				",avs_code       ='" .urldecode($this->response['AVSCODE'])."'".
				",cvv2_match     ='" .urldecode($this->response['CVV2MATCH'])."'".
				",timestamp      = NOW()".
				",version        ='" .urldecode($this->response['VERSION'])."'".
				",pimp_rc        ='" .urldecode($this->response['PIMP_RC'])."'";
				//",timestamp      ='" .urldecode($this->response['TIMESTAMP'])."'".

			$db->query($sql);
		}


		/**
		 * Valid password.
		 */

		public function reference_transaction($amount,$reference_id)
		{

			// Add request-specific fields to the request string.
			$nvpStr =	
				"&PAYMENTACTION="  .urlencode('Sale').
				"&AMT="            .urlencode($amount).
				"&REFERENCEID="    .urlencode($reference_id);
			print_ar($nvpStr);

			// Execute the API operation; see the PPHttpPost function above.
			$this->response = $this-> PPHttpPost('DoReferenceTransaction', $nvpStr);

			if("Success" != $this->response["ACK"])
			{
				return('error: Transaction Failed');
			}

		}

		public function direct_payment($parms)
		{

			$address2 = urlencode('');
			// Month must be padded with leading zero
			$exp_date = str_pad($parms['exp_month'], 2, '0', STR_PAD_LEFT).$parms['exp_year'];

			// Add request-specific fields to the request string.
			$nvpStr =	
				"&PAYMENTACTION="  .urlencode('Sale').
				"&AMT="            .urlencode($parms['amount']).
				"&CREDITCARDTYPE=" .urlencode($parms['card_type']).
				"&ACCT="           .urlencode($parms['card_number']).
				"&EXPDATE="        .urlencode($exp_date).
				"&CVV2="           .urlencode($parms['cvv_number']).
				"&FIRSTNAME="      .urlencode($parms['first_name']).
				"&LASTNAME="       .urlencode($parms['last_name']).
				"&STREET="         .urlencode($parms['address']).
				"&CITY="           .urlencode($parms['city']).
				"&STATE="          .urlencode($parms['state']).
				"&ZIP="            .urlencode($parms['zip']).
				"&COUNTRYCODE="    .urlencode('US').
				"&CURRENCYCODE="   .urlencode('USD');

			// Execute the API operation; see the PPHttpPost function above.
			//$httpParsedResponseAr = PPHttpPost('DoDirectPayment', $nvpStr);

			$this->response = $this->PPHttpPost('DoDirectPayment', $nvpStr);

			if("Success" != $this->response["ACK"])
			{
				return('error: Transaction Failed');
			}
			
		}

		function PPHttpPost($methodName_, $nvpStr_)
		{
			//global $environment;

			$environment = 'sandbox';

			// Set up API credentials, PayPal end point, and API version.
			$API_Endpoint = "https://api-3t.paypal.com/nvp";
			if("sandbox" === $environment || "beta-sandbox" === $environment)
			{
				$API_Endpoint = "https://api-3t.$environment.paypal.com/nvp";

				$api_pwd = urlencode(API_Password);
				$api_user = urlencode(API_UserName);
				$api_signature =urlencode(API_Signature);
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
			$nvpreq = "METHOD=$methodName_&VERSION=$version&PWD=".$api_pwd."&USER=".$api_user."&SIGNATURE=".$api_signature."$nvpStr_";

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
				return ("error: Invalid HTTP Response for POST request($nvpreq) to $API_Endpoint.");
			}

			return $httpParsedResponseAr;
		}

	}

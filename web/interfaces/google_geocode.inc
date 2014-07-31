<?php

	/*
	* Class to deal with Google Geocoding -- Caches requests using ezSQL
	*
	* Note: This is for use with google maps API
	*
	*
	* Methods:
	*
	*  Public...
	*
	*    $gg->get_coords_as_js_args($address)
	*    $gg->get_geocode_obj($address)
	*
	*  Private...
	*
	*    $gg->install_cache()
	*    $gg->dump_cache()
	*    $gg->cache_geocode_obj($address,$coords_obj)
	*    $gg->get_geocode_obj_from_cache($address)
	*    $gg->parse_coords($response)
	*    $gg->clean_up_address($address)
	*    $gg->make_key_from_address($address)
	*
	* Usage:
	*
	* print_r($gg->get_geocode_obj('10601 W Battaglia Dr, Arizona City, Arizona 85223'));
	*
	*/

	if ( ! isset($db) ) die("google_geocode class requires ezSQL");

	class google_geocode
	{

		var $debug = false;

		/*
		* Gets the lat, long as javascript args. 
		* To be used with google maps API something like this:
		*
		* <script>var mapPoint1 = GLatLng(<?= $geocode->get_coords_as_js_args('10601 W Battaglia Dr, Arizona City, Arizona 85223') ?>);</script>
		*
		*/

		function get_coords_as_js_args($address)
		{
			if ( $geocode_obj = $this->get_geocode_obj($address) )
			{
				return "{$geocode_obj->latitude}, {$geocode_obj->longitude}" . ($this->debug ? ' /*'.($geocode_obj->cached?'cached':'uncached').'*/' : '');
			}
			return false;
		}

		/*
		* Main function to get geocode object. Looks like...
		*
		*	stdClass Object
		*	(
		*	    [addr_key] => e9c213ef2ee6e7b983e71473e548700e
		*	    [address] => 10601 W Battaglia Dr, Arizona City, Arizona 85223
		*	    [latitude] => 32.7622760
		*	    [longitude] => -111.6830210
		*	    [cached] => 1
		*	)
		*
		*/

		function get_geocode_obj($address)
		{
			
			global $db;
			
			if ( $geocode_obj = $this->get_geocode_obj_from_cache($address) )
			{
				$geocode_obj->cached = true;
				return $geocode_obj;
			}

			if ( $coords_obj = $this->parse_coords(file_get_contents("http://maps.google.com/maps/geo?q=".urlencode($this->clean_up_address($address))."&output=csv&oe=utf8&sensor=false&key=".GMAPS_API_KEY)) )
			{	
				$geocode_obj = $this->cache_geocode_obj($address,$coords_obj);
				$geocode_obj->cached = false;
				return $geocode_obj;
			}
		}

		/*
		* Get code from cache as db row object
		*/

		function get_geocode_obj_from_cache($address)
		{
			global $db;
			return $db->get_row("SELECT * FROM google_geocache WHERE addr_key = '".$this->make_key_from_address($address)."'");
		}

		/*
		* Clean up the address before we submit it to google (or make a key from it)
		*/

		function clean_up_address($address)
		{
			return ucwords(trim(strtolower(preg_replace('/[\s]+/',' ',$address))));
		}

		/*
		* Make a key from the address
		*/

		function make_key_from_address($address)
		{
			return md5($this->clean_up_address($address));
		}

		/*
		* Parse the response from google
		* Response looks like: 200,6,42.730070,-73.690570
		*/
		
		function parse_coords($response)
		{
			list($http_stat, $accuracy, $lat, $long) = split(',',$response);

			if ( $http_stat == 200 )
			{
				$coords_obj->lat = $lat;
				$coords_obj->long = $long;
				return $coords_obj;
			}
			
			// Got here? Then it was a bad response
			return false;
		}

		/*
		* Assuming we have the correct coords passed in as an array
		* cache them in the database
		*/

		function cache_geocode_obj($address,$coords_obj)
		{
			global $db;

			$addr_key = $this->make_key_from_address($address);
			$address  = $this->clean_up_address($address);
			
			$SET_SQL = "SET address = '".$db->escape($address)."', latitude = '".$db->escape($coords_obj->lat)."', longitude = '".$db->escape($coords_obj->long)."'";
					
			if ( ! $db->get_var("SELECT count(*) FROM google_geocache WHERE addr_key = '$addr_key'") )
			{
				$db->query("INSERT INTO google_geocache $SET_SQL, addr_key = '$addr_key'");
			}
			else
			{
				$db->query("UPDATE google_geocache $SET_SQL WHERE addr_key = '$addr_key'");
			}

			$geocode_obj->addr_key  = $addr_key;
			$geocode_obj->address   = $address;
			$geocode_obj->latitude  = $coords_obj->lat;
			$geocode_obj->longitude = $coords_obj->long;
			return $geocode_obj;
		}

	}

	function get_geocode($biz,$force=false)
	{
		global $db;

		$GG = new google_geocode;

		if ( $biz->geocode_status == 'un_retrieved' || $force )
		{

			if ( $geocode_obj = $GG->get_geocode_obj("{$biz->street}, {$biz->city}, {$biz->state} {$biz->zip}") )
			{
				$biz->geocode_status = 'retrieve_success';
				$biz->latitude       = $geocode_obj->latitude;
				$biz->longitude      = $geocode_obj->longitude;
				$db->query("UPDATE businesses SET geocode_status = 'retrieve_success', latitude = '{$biz->latitude}', longitude = '{$biz->longitude}' WHERE biz_id = {$biz->biz_id}");
			}
			else
			{
				$biz->geocode_status = 'retrieve_fail';
				$db->query("UPDATE businesses SET geocode_status = 'retrieve_fail' WHERE biz_id = {$biz->biz_id}");
			}
		}
		
		return $biz;
	}

?>
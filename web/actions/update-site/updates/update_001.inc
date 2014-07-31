<?php

	/******************************************
	* Install the tables and set everything up to bring 
	* us from scratch to up-to-date
	*/

	update_message('Installing initial database tables');

	$tables = array
	(
		'updates',
		'users',
		'cache',
		'businesses',
		'google_geocache',
		'reviews',
		'picklists',
		'picklists_lookup',
		'biz_leads',
		'biz_pics',
		'biz_link_to_user',
		'biz_bookmarks',
	);

	foreach ($tables AS $table)
	{
		update_message("Installing table $table");
		create_table(include "db/$table.inc");
	}

	/******************************************
	* Add users
	*/

	update_message("Add default db users");

	$users = array
	(
		array
		(
			'login'    => 'jv',
			'email'    => 'jv@vip.ie',
			'password' => 'qwerty',
			'is_admin' => 1,
			'is_validated' => 1,
			'url' => 'http://www.jvmultimedia.com',
			'approval_status' => 'approved',
		),
		array
		(
			'login'    => 'tyler',
			'email'    => 'tyler.nichols@gmail.com',
			'password' => 'tyler',
			'is_admin' => 1,
			'is_validated' => 1,
			'url' => 'http://www.healthclubnetwork.com',
			'approval_status' => 'approved',
		)
	);

	foreach ( $users as $parms )
	{
		$user = new user;
		if ( $error = is_error($user->add($parms)) )
		{
			update_message("<font color=red>".$error."</font>");
		}
	}

	for ( $i=1; $i<=100 ; $i++ )
	{
		$parms = array
		(
			'login'    => 'user'.$i,
			'email'    => 'user'.$i.'@members.com',
			'password' => 'qwerty',
			'is_admin' => 0,
			'is_validated' => 1,
			'url' => 'http://www.members.com',
			'on_mailing_list' => 1
		);
		if ($i%2==0)
		{
			$parms['is_biz_owner'] = 1;
			$parms['first_name'] = 'John';
			$parms['last_name']  = 'Smith';
		}
		$user->add($parms);
	}

	/***************************************************************
	* This func is used by below procedural to migrate address to the new DB
	*/

	include_once "lib/state_utils.inc";

	function migrate_biz_address($biz,$type)
	{
		global $db, $STATES_CAPITALISED;
		$SET_SQL = "
		name = '".$db->escape($biz->BizName)."',
		street = '".$db->escape($biz->BizAddr)."',
		city = '".$db->escape($biz->BizCity)."',
		state = '".$db->escape($biz->BizState)."',
		zip = '".$db->escape($biz->BizZip)."',
		phone = '".$db->escape($biz->BizPhone)."',
		homepage = '".$db->escape($biz->URL)."',
		hours = '".$db->escape($biz->Hours)."',
		type = '$type',
		description = '".$db->escape($biz->Desc)."',
		search_bucket = '".$db->escape(padd_keywords("{$biz->BizName} {$biz->BizAddr} {$biz->BizCity} ".get_long_state($biz->BizState)." {$biz->BizZip} {$biz->BizPhone} {$biz->Desc} "))."',
		date_updated = NOW(),
		is_live = 1,
		biz_id = {$biz->BizID}
		";

		// Now using old ID schema
		$db->query("INSERT INTO businesses SET $SET_SQL");

	}

	/***************************************************************
	* This part migrates listings from the old db to the new one
	*/

	if ( in_array('businesses',$tables) )
	{

		$db->query("INSERT INTO businesses SET biz_id = 1");

		if ( $db->get_var("SHOW TABLES LIKE 'biz_health_clubs_and_gyms'") )
		{
			foreach ( $db->get_results("SELECT * FROM biz_health_clubs_and_gyms") as $biz )
			{
				migrate_biz_address($biz,'health-clubs');
			}
		}
		
		if ( $db->get_var("SHOW TABLES LIKE 'biz_personal_trainers'") )
		{
			foreach ( $db->get_results("SELECT * FROM biz_personal_trainers") as $biz )
			{
				migrate_biz_address($biz,'personal-trainers');
			}
		}
	
	}

	/***************************************************************
	* Populate picklists
	*/

	$default_picklists = array
	(
		'health-clubs/amenities' => array
		(
			'Aerobics',
			'Aqua Aerobics',
			'Basketball',
			'Boxing',
			'Childcare',
			'Circuit Training',
			'Climbing Wall',
			'Communal Showers',
			'Dance',
			'Dry Sauna',
			'Elliptical Trainers',
			'Free Weights',
			'Group Cycling',
			'Kick Boxing',
			'Lockers',
			'Martial Arts',
			'Massage',
			'Multi-Screen Video',
			'Nutritional Program',
			'Personal Training',
			'Physical Therapy',
			'Pilates',
			'Plate Loaded Machines',
			'Private Showers',
			'Pro Shop',
			'Racquetball',
			'Running Track',
			'Sauna',
			'Selectorized Machines',
			'Snack / Juice Bar',
			'Spinning',
			'Stair Climbers',
			'Stationary Cycling',
			'Steam Sauna',
			'Step Aerobics',
			'Swimming Pool',
			'Tanning',
			'Tennis',
			'Treadmills',
			'Volleyball',
			'Weight Loss Program',
			'Whirlpool',
			'Women Only Section',
			'Yoga'
		),
		
		'health-clubs/coupons' => array
		(
			'Free 1 Day Guest Pass',
			'Free 7 Day Guest Pass',
			'Waived Initiation Fee',
			'50% off Initiation Fee',
		),
		
		'personal-trainers/amenities' => array
		(
			'Nutritional Consulting',
			'In-Home Training',
			'Fitness Testing',
		),
		
		'personal-trainers/coupons' => array
		(
		  '1 Free Personal Training Session',
		  '2 Free Personal Training Sessions',
		  '3 Free Personal Training Sessions',
		),
	
	);
	
	include "interfaces/picklist.inc";
	
	foreach ( $default_picklists as $picklist_name => $items )
	{
		$picklist_admin = new picklist_admin($picklist_name);
		foreach ($items as $item)
		{
			$picklist_admin->add($item);
		}	
	}

	/***************************************************************
	* Add some sampel data into the biz_leads table
	*/

	include "interfaces/biz/biz_leads.inc";
	
	$first_names    = array('Bob','Bill','Simon','Jason','Ian','John');
	$sur_names      = array('Smith','Jones','Garcia','Vincent','Nichols','Cheung');
	$mail_providers = array('yahoo','gmail','ask','hotmail');
	$bizs           = array(63,237,271,1091,29432);
	$creation_dates = array('NOW()',"DATE_SUB(NOW(),INTERVAL 1 DAY)","DATE_SUB(NOW(),INTERVAL 1 MONTH)");

	for ( $i=0; $i<20; $i++ )
	{
		$first = $first_names[rand(0,5)];
		
		$parms = array
		(
			'biz_id'      => $bizs[rand(0,count($bizs)-1)],
			'first_name'  => $first,
			'last_name'   => $sur_names[rand(0,5)],
			'phone'       => rand(100,900).'-'.rand(100,900).'-'.rand(1000,5000),
			'zip'         => rand(100000,900000),
			'is_approved' => rand(0,1),
			'email'       => strtolower("$first@".$mail_providers[rand(0,3)].".com"),
			'coupon'      => $default_picklists['health-clubs/coupons'][rand(0,2)],
			'i_am_18'     => rand(0,1),
			'type'        => ' health-clubs'
		);

	  $set_sql = get_set_sql($parms);

		//$db->query("INSERT INTO biz_leads SET $set_sql,creation_date= ".$creation_dates[rand(0,2)]);

		$biz_lead = new biz_lead;
		$biz_lead->add_or_update($parms);

	}

	/***************************************************************
	* Add some sampel biz to user links for admin tests
	*/

	include "interfaces/biz/biz_link_to_user.inc";
	
	$biz_link_to_user = new biz_link_to_user;
	
	foreach ( $bizs as $biz_id )
	{	
		$biz_link_to_user->request(4,$biz_id);
	}

	/***************************************************************
	* Add some default reviews for user1 for testing
	*/

	include "interfaces/biz/biz_reviews.inc";

	$review = new review;

//	$ipsum = array
//	(
//		"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris sollicitudin. In porta. Phasellus quis orci id turpis vestibulum pulvinar. Duis tincidunt sapien vitae nisi.", 
//		"Sed vel neque eget velit porttitor condimentum. Suspendisse egestas aliquam quam. Aliquam a nibh. Nam eget dui ut elit auctor fringilla. Morbi faucibus mattis libero.",
//		"Vivamus malesuada bibendum metus. Vivamus lacinia, velit in eleifend aliquam, mauris nibh tempor arcu, quis sagittis sem purus eget mauris.",
//		"Aliquam lectus. Vivamus ligula velit, aliquet non, lobortis et, consequat molestie, metus. Vestibulum commodo.",
//		"Etiam leo leo, vulputate at, adipiscing vel, gravida id, ligula. In lacinia, magna vel ultricies lobortis, mauris mi viverra felis, interdum molestie lectus velit vitae risus.",
//		"Phasellus scelerisque dignissim metus. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae.",
//	);
//
//	for ( $i = 0; $i < 20; $i++ )
//	{
//		$parms = array
//		(
//			'biz_id' =>  $bizs[rand(0,4)],
//			'user_id' => 3,
//			'rating' => 3,
//			'type' => 'healthclub',
//			'email' => 'user1@members.com',
//			'name' => 'user1',
//			'review' => $ipsum[rand(0,5)]
//		);
//		
//		$review->add_or_update($parms);
//	}

	/***************************************************************
	* Add some sample data into the biz_bookmarks table
	*/

	include "interfaces/biz/biz_bookmarks.inc";
	
	// This user is created above
	$user_id  = 3;
	// This biz_id values are taken from the previously existing DB
  $biz_list = array (2,16,33,34,61,63,64,66,136,142,167,182,234,237,239,243,263,267,270,271,294,447,493);

	$biz_bookmark = new biz_bookmark;
  
  foreach ($biz_list as $biz_id)
  {
  	$biz_bookmark->add($user_id,$biz_id);
  } 


?>

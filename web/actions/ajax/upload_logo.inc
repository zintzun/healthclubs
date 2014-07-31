<?php

	/* Note: This thumbnail creation script requires the GD PHP Extension.  
		If GD is not installed correctly PHP does not render this page correctly
		and SWFUpload will get "stuck" never calling uploadSuccess or uploadError
	 */

	// Get the session Id passed from SWFUpload. We have to do this to work-around the Flash Player Cookie Bug
	if (isset($_POST["PHPSESSID"])) {
		session_id($_POST["PHPSESSID"]);
	}

	//session_start();
	ini_set("html_errors", "0");

	// Check the upload
	if (!isset($_FILES["Filedata"]) || !is_uploaded_file($_FILES["Filedata"]["tmp_name"]) || $_FILES["Filedata"]["error"] != 0) {
		echo "ERROR:invalid upload";
		exit(0);
	}

	//////////////////////////////////////////////////
	include "interfaces/biz/biz_pics.inc";

	$biz_pic = new biz_pic;

	$parms = array
	(
		'biz_id'       => $_SESSION['BIZ_LISTING']['biz_id'],
		'img_src_path' => $_FILES['Filedata']['tmp_name'],
		'mime_type'    => get_regex('/(jpeg|jpg|gif|png)/',$_FILES['Filedata']['name']),
		'pic_type'     => 'logo',
	);

	echo "FILEID:" .  $biz_pic->store($parms);// Return the file id to the script

	return;

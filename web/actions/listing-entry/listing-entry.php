<?php

	/*
	* This includes the required JS & CSS for the rating
	*/

	use_js("/template/js/stars/stars.js");
	use_css("/template/js/stars/stars.css");

	/*
	* thickbox files for image gallery
	*/
	use_js("/third_party/thickbox//thickbox-compressed.js");
	use_css("/third_party/thickbox//thickbox.css");
	

	/*
	* All the stuff required to make the map & title work
	*/

	include "actions/listing-entry/_listing-entry-init.inc";

	/*
	* All the stuff required to update / edit reviews
	*/

	include "actions/listing-entry/_reviews.inc";

?>

<?php

	include "lib/state_utils.inc";

	$TARGET->page_title = $GLOBAL_NAV[$TARGET->action_orig]['titles']['listing-root']['page-title'];
	$TARGET->meta_desc = $GLOBAL_NAV[$TARGET->action_orig]['titles']['listing-root']['meta-desc'];
	$TARGET->meta_keys =  $GLOBAL_NAV[$TARGET->action_orig]['titles']['listing-root']['meta-keys'];
	$__title = $GLOBAL_NAV[$TARGET->action_orig]['titles']['listing-root']['content-title'];
	$__intro = $GLOBAL_NAV[$TARGET->action_orig]['intro'];

?>

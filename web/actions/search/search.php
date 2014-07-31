<?php

	$TARGET->page_title    = "Search this site";
	$TARGET->content_title = $TARGET->page_title;
	$TARGET->meta_keys = 'search health clubs';

	include "interfaces/paging.inc";

	$RESULTS_PER_PAGE = 10;

	// Set up current start row
	$CUR_PAGE = isset($_GET['page'])&&is_numeric($_GET['page'])?$_GET['page']:1;
	$START_ROW = ($CUR_PAGE - 1) * $RESULTS_PER_PAGE;
	
	$BUSINESSES = false;

	if ( isset($_REQUEST['q']) )
	{
		
		if ( $TARGET->action != 'admin' && is_numeric($_REQUEST['q']) && strlen($q = trim($_REQUEST['q'])) == 5 )
		{
			$biz_type = 'health-clubs';
			if ( isset($_REQUEST['biz_type_search']) && in_array($_REQUEST['biz_type_search'],$LISTING_TYPES) )
			{
				$biz_type = $_REQUEST['biz_type_search'];
			}
			
			notify_redirect("/$biz_type/by-zip/$q");	
		}

		$TARGET->page_title = "Search results for \"{$_REQUEST['q']}\"";
		$TARGET->content_title = $TARGET->page_title;
		
		$query  = padd_keywords($_REQUEST['q']);	
		$bquery  = '+'.str_replace(' ',' +',$query);

		if ( isset($SHOW_ADMIN_CONTROLS) )
		{
			$MATCH_SQL = "";
			$WHERE_SQL = "b.search_bucket like '%".$db->escape($query)."%'";
			$ORDER_BY  = '';
		}
		else
		{
			$MATCH_SQL = ", MATCH (b.search_bucket) AGAINST ('$query') AS rating";
			$WHERE_SQL = "MATCH (b.search_bucket) AGAINST ('$bquery' IN BOOLEAN MODE)";
			$ORDER_BY  = 'ORDER BY rating DESC';
		}

		if ( $NUM_RESULTS = $db->get_var("SELECT count(*) FROM businesses b WHERE $WHERE_SQL") )
		{

			$NUM_PAGES = ceil($NUM_RESULTS/$RESULTS_PER_PAGE);
			
			include_once "lib/state_utils.inc";
			
			$BUSINESSES = $db->get_results("SELECT b.*, p.*, b.biz_id $MATCH_SQL FROM businesses b LEFT JOIN biz_pics p on p.biz_id=b.biz_id AND p.pic_type = 'logo' WHERE $WHERE_SQL $ORDER_BY LIMIT $START_ROW,$RESULTS_PER_PAGE");

			foreach ( $BUSINESSES as $idx => $item )
			{

				$BUSINESSES[$idx]->logo = false;
				if ( isset($item->pic_id) )
				{
					$BUSINESSES[$idx]->logo = "/data/pics/{$item->biz_id}-{$item->pic_id}.jpg";
				}
				
				$item->state = get_long_state($item->state);
				$link  = "/{$item->type}/".format_link_subject($item->state).'/'.format_link_subject($item->city).'/'.$item->biz_id.'/'.format_link_subject($item->name);
				
				foreach ( (array) $item as $k => $v )
				{
					$item->{$k} = highlight_content($v,$_REQUEST['q'],"b");
				}
				
				$item->link = $link;
				
				$BUSINESSES[$idx] = $item;
			}
		}
				
	}

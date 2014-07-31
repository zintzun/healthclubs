<?php

	/*

	biz_bookmark Class
	. . . . . . . . . . . . . . . . . . . . . . . . . . . 
	Usage:

		biz_bookmark->add($user_id,$biz_id)
		biz_bookmark->remove($user_id,$biz_id)
		biz_bookmark->get($user_id,$biz_id)
		biz_bookmark->get_by_user($user_id)
	*/

	class biz_bookmark
	{
		private function test_for_bad_parms($user_id,$biz_id)
		{
			if ( ! is_numeric($user_id) )
			{
				return 'error: invalid_user_id';
			}
			else if ( ! is_numeric($biz_id) )
			{
				return 'error: invalid_biz_id';
			}
			else
			{
				return false;
			}
			
		}
		/********************************************************************
		* biz_bookmark->add($user_id,$biz_id)
		*/

		public function add($user_id,$biz_id)
		{
			global $db;
			
			if ( ($error = $this->test_for_bad_parms($user_id,$biz_id)) )
			{
				return $error;
			}
			
			// Test if record exists
			if ( $this->get($user_id,$biz_id) )
			{
				return true;
			}
			else
			{
				return $db->get_row("INSERT INTO biz_bookmarks(user_id,biz_id) VALUES ($user_id,$biz_id)");
			}
		}

		/********************************************************************
		* biz_bookmark->remove($user_id,$biz_id)
		*/

		public function remove($user_id,$biz_id)
		{
			global $db;

			if ( ($error = $this->test_for_bad_parms($user_id,$biz_id)) )
			{
				return $error;
			}
			
			return $db->query("DELETE FROM biz_bookmarks WHERE user_id = $user_id AND biz_id = $biz_id");
		}

		/********************************************************************
		* biz_bookmark->get($user_id,$biz_id)
		*/

		public function get($user_id,$biz_id)
		{
			global $db;

			if ( ($error = $this->test_for_bad_parms($user_id,$biz_id)) )
			{
				return $error;
			}
			
			return $db->get_row("SELECT * FROM biz_bookmarks WHERE user_id = $user_id AND biz_id = $biz_id");
		}

		/********************************************************************
		* biz_bookmark->get_by_user($user_id)
		*/

		public function get_by_user($user_id)
		{
			global $db;
			$sql ="
			    SELECT
			      biz_bookmarks.*,
			      businesses.*
			    FROM
			        biz_bookmarks JOIN businesses 
			        ON biz_bookmarks.biz_id = businesses.biz_id
			    WHERE
			      biz_bookmarks.user_id = $user_id
			    ORDER BY
			      businesses.name
			";

			if ( ! is_numeric($user_id) )
			{
				return 'error: invalid_user_id';
			}

			return $db->get_results($sql);
		}
	}
?>
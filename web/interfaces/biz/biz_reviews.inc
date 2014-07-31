<?php

	/*
	* Class to reviews
	*/

	class review
	{
		var $data = false;

		/*
		* Generic add_or_update
		*/

		/*
		
		Example of input array:
		$parms = array
		(
			'biz_id' =>  0,
			'user_id' => 0,
			'rating' => 3,
			'type' => 'healthclub',
			'email' => 'jv@vip.ie',
			'name' => 'JV',
			'review' => 'This was a great place'
		);
		
		*/

		function add_or_update($parms)
		{
			global $db;
	
			$parms['last_updated'] = 'NOW()';
			
			if ( isset($parms['review']) )
			{
				$parms['review'] = strip_tags($parms['review']);
			}

			// Create SET sql based on input array
			$set_sql = get_set_sql($parms);
			
			// If entry exists update
			if (isset($parms['review_id']) && is_numeric($parms['review_id']) )
			{
				if ( !( session_is_admin() || $parms['user_id'] == $db->get_var("SELECT user_id FROM reviews WHERE review_id = {$parms['review_id']}") ) )
				{
					return "error: user_id_different_from_user_in_session";
				}
				
				$db->query("UPDATE reviews SET $set_sql WHERE review_id = {$parms['review_id']}");
				
				return $parms['review_id'];
			}
			// If biz_id and review)
			else if ( isset($parms['biz_id']) )
			{
				if ( !is_numeric($parms['biz_id']) )
				{
					return "error: invalid_biz_id";	
				}
				
				if ( !isset($parms['review']) || ! trim($parms['review']) )
				{
					return "error: no_review";	
				}
				
				$db->query("INSERT INTO reviews SET $set_sql,creation_date = NOW()");
				return $db->insert_id;

			}
					
		}
	
		/*
		* Specific update_review (based on add_or_update)
		*/
	
		function update_review($review_id, $review)
		{
			if ( !is_numeric($review_id) ) 
			{
				return 'error: invalid_review_id';
			}
			
			if ( !trim($review) ) 
			{
				return 'error: no_review';
			}
	
			$parms = array 
			(
				'review_id' => $review_id, 
				'review' => $review 
			);

			return $this->add_or_update($parms);

		}

		/*
		* delete_review
		*/

		function delete_review($review_id)
		{
			global $db;
		
			if ( !is_numeric($review_id))
			{
				return 'error: invalid_review_id';
			}
		
			return $db->query('DELETE FROM reviews WHERE review_id = ' . $review_id);
		}

		/*
		* Get review
		*/
		function get($review_id)
		{
			global $db;

			$review_id = trim($review_id);

			if ( !ctype_digit($review_id) )
			{
				 return "error: invalid_review_id";
			}

			$this->data = $db->get_row("SELECT * FROM reviews WHERE review_id = $review_id");

			return($this->data);
		}

		function get_by_biz_and_user($biz_id, $user_id)
		{
			global $db;

			$biz_id = trim($biz_id);
			$user_id = trim($user_id);

			if ( !$biz_id || !ctype_digit($biz_id) )
			{
				return "error: invalid_user_id";
			}
			if ( !$user_id || !ctype_digit($user_id) )
			{
				return "error: invalid_user_id";
			}

			$this->data = $db->get_results("SELECT * FROM reviews WHERE biz_id = $biz_id AND user_id = $user_id");

			return($this->data);
		}
				

		/*
		* Get all reviews for a business
		*/
		function get_biz_reviews($biz_id)
		{
			global $db;

			$biz_id = trim($biz_id);

			if ( !ctype_digit($biz_id) )
			{
				return "error: invalid_biz_id";
			}

			$this->data = $db->get_results("SELECT reviews.*,users.name FROM reviews,users WHERE biz_id = $biz_id AND users.user_id = reviews.user_id");

			return($this->data);
		}
		
		/*
		* Get all reviews for a business
		*/
		function get_by_user_id($user_id)
		{
			global $db;

			if ( !is_numeric($user_id) )
			{
				return "error: invalid_user_id";
			}

			$this->data = $db->get_results("SELECT r.*,b.*,r.last_updated FROM reviews r,users u, businesses b WHERE r.user_id=$user_id AND u.user_id=r.user_id AND b.biz_id=r.biz_id");

			return($this->data);
		}
	}

?>

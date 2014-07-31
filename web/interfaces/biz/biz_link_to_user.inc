<?php

	/*

	biz_link_to_user Class
	. . . . . . . . . . . . . . . . . . . . . . . . . . . 
	Usage:

		biz_link_to_user->request($user_id,$biz_id)
		biz_link_to_user->approve($user_id,$biz_id)
		biz_link_to_user->unaprove($user_id,$biz_id)
		biz_link_to_user->remove($user_id,$biz_id)
		biz_link_to_user->is_approved($user_id$biz_id)
		biz_link_to_user->is_requested($user_id,$biz_id)
	*/

	class biz_link_to_user
	{
		/********************************************************************
		* biz_link_to_user->request($user_id,$biz_id)
		*/

		public function request($user_id,$biz_id)
		{
			global $db;
			
			if ( ! is_numeric($user_id) )
			{
				return 'error: invalid_user_id';
			}
			else if ( ! is_numeric($biz_id) )
			{
				return 'error: invalid_biz_id';
			}
			else if ( $this->is_approved($user_id,$biz_id) )
			{
				return 'error: biz_id_is_approved';
			}
			
			return $db->query("INSERT INTO biz_link_to_user(user_id,biz_id,is_linked,creation_date) VALUES ($user_id,$biz_id,0,NOW())");
		}

		/********************************************************************
		* biz_link_to_user->approve($user_id,$biz_id)
		*/	
		
		public function approve($user_id,$biz_id)
		{
			global $db, $APP_DOMAIN;
			
			if ( ! is_numeric($user_id) )
			{
				return 'error: invalid_user_id';
			}
			else if ( ! is_numeric($biz_id) )
			{
				return 'error: invalid_biz_id';
			}
						
			/*
			* Send an email to this user letting them know that the business 
			* has been approved
			*/
	
			$user = new user($user_id);
			
			include_once "interfaces/mail.inc";
	
			$msg_parms = array
			(
				'biz_name'     => $db->get_var("SELECT name FROM businesses WHERE biz_id = $biz_id"),
				'first_name'   => $user->data->first_name,
				'last_name'    => $user->data->last_name,
				'login_link'   => "http://$APP_DOMAIN/login",
			);

			send_mail_helper($user->data->email,"template/mail/biz_link_approval_notice.txt",$msg_parms);

			if ( $this->is_requested($user_id,$biz_id) || $this->is_requested($user_id,$biz_id) )
			{
				return $db->query("UPDATE biz_link_to_user SET is_linked = 1 WHERE user_id = $user_id AND biz_id = $biz_id");
			}
			else
			{
				return $db->query("INSERT INTO biz_link_to_user(user_id,biz_id,is_linked,creation_date) VALUES ($user_id,$biz_id,1,NOW())");
			}
	
		}

		/********************************************************************
		* biz_link_to_user->un_approve($user_id,$biz_id)
		*/	
		
		public function un_approve($user_id,$biz_id)
		{
			global $db,$APP_DOMAIN;
			
			if ( ! is_numeric($user_id) )
			{
				return 'error: invalid_user_id';
			}
			else if ( ! is_numeric($biz_id) )
			{
				return 'error: invalid_biz_id';
			}
			
			/*
			* Send an email to this user letting them know that the business 
			* has been approved
			*/
	
			$user = new user($user_id);
			
			include_once "interfaces/mail.inc";
	
			$msg_parms = array
			(
				'biz_name'     => $db->get_var("SELECT name FROM businesses WHERE biz_id = $biz_id"),
				'first_name'   => $user->data->first_name,
				'last_name'    => $user->data->last_name,
				'contact_link' => "http://$APP_DOMAIN/contact",
			);

			send_mail_helper($user->data->email,"template/mail/biz_link_unapproval_notice.txt",$msg_parms);
		
			return $db->query("UPDATE biz_link_to_user SET is_linked = 0 WHERE user_id = $user_id AND biz_id = $biz_id");
		}
		
		/********************************************************************
		* biz_link_to_user->remove($user_id,$biz_id)
		*/	
		
		public function remove($user_id,$biz_id)
		{
			global $db;
			
			if ( ! is_numeric($user_id) )
			{
				return 'error: invalid_user_id';
			}
			else if ( ! is_numeric($biz_id) )
			{
				return 'error: invalid_biz_id';
			}
			
			return $db->query("DELETE FROM biz_link_to_user WHERE user_id = $user_id AND biz_id = $biz_id");
		}
		
		/********************************************************************
		* biz_link_to_user->is_approved($user_id,$biz_id)
		*/	
		
		public function is_approved($user_id,$biz_id)
		{
			global $db;
			return $db->get_var("SELECT count(*) FROM biz_link_to_user WHERE user_id = '".$db->escape($user_id)."' AND biz_id = '".$db->escape($biz_id)."' AND is_linked = 1");
		}

		/********************************************************************
		* biz_link_to_user->is_requested($user_id,$biz_id)
		*/	
		
		public function is_requested($user_id,$biz_id)
		{
			global $db;
			return $db->get_var("SELECT count(*) FROM biz_link_to_user WHERE user_id = '".$db->escape($user_id)."' AND biz_id = '" . $db->escape($biz_id) . "'");
		}
		
	}
?>

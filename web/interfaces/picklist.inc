<?php

	/*

	Picklist Class
	. . . . . . . . . . . . . . . . . . . . . . . . . . . 

	Usage:

	- Dealing with picklist from users point of view
	
		$picklist = new picklist($picklist_name);
		$picklist->get($owner_id);
		$picklist->assign_val($pick_val_id,$owner_id);
		$picklist->revoke_val($pick_val_id,$owner_id);
		$picklist->get_text($pick_val_id);
	
	- Dealing with picklist from admins point of view
	
		$picklist_admin = new picklist_admin($picklist_name);
		$picklist_admin->add($new_text)
		$picklist_admin->bump_down($pick_val_id)
		$picklist_admin->bump_up($pick_val_id)
		$picklist_admin->edit($pick_val_id,$new_text)
		$picklist_admin->get($pick_val_id)
		$picklist_admin->get_all()
		$picklist_admin->get_num()
		$picklist_admin->remove($pick_val_id)
		$picklist_admin->remove_by_text($text)
		$picklist_admin->set_order($pick_val_id,$new_position)
		$picklist_admin->get_all_incl_pick_val_id()

	*/

	class picklist
	{
		
		var $picklist_name = false;
		
		/********************************************************************
		* picklist_admin->__construct($picklist_name)
		*/	
		
		public function __construct($picklist_name)
		{
			$this->picklist_name = $picklist_name;
		}

		/********************************************************************
		* picklist->assign_val($pick_val_id,$owner_id)
		*/	

		public function assign_val($pick_val_id,$owner_id)
		{
			global $db;
			
			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}
			
			if (! is_numeric($owner_id) )
			{
				return 'error: invalid_owner_id';	
			}
			
			//Insert  feature only if it doesn't alredy exists in the table
			if ( ! $db->get_var("SELECT count(*) FROM picklists_lookup WHERE pick_val_id = $pick_val_id AND owner_id = $owner_id") )
			{
				$db->query("INSERT INTO picklists_lookup SET pick_val_id = $pick_val_id, owner_id = $owner_id" );
			}
		}
		
		/********************************************************************
		* picklist->revoke_val($pick_val_id,$owner_id)
		*/	
		
		public function revoke_val($pick_val_id,$owner_id)
		{
			global $db;
			
			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}
			
			if (! is_numeric($owner_id) )
			{
				return 'error: invalid_owner_id';	
			}
			
			return $db->query("DELETE FROM picklists_lookup WHERE pick_val_id = $pick_val_id AND owner_id = $owner_id");
		}
		
		/********************************************************************
		* picklist->get_vals($owner_id)
		*/	
		
		public function get($owner_id)
		{
			global $db;
			
			if (! is_numeric($owner_id) )
			{
				return 'error: invalid_owner_id';	
			}
			
			$return_array = array();
			
			if ( $items = $db->get_results("SELECT p.pick_val_id, p.value FROM picklists p, picklists_lookup pl WHERE pl.owner_id=$owner_id AND p.picklist_name = '".$db->escape($this->picklist_name)."' AND pl.pick_val_id=p.pick_val_id ORDER BY p.sort_order") )
			{
				foreach ( $items as $item )
				{
					$return_array[$item->pick_val_id] = $item->value;
				}
			}
			
			return $return_array;
		}
		
		public function delete_all($owner_id)
		{
			global $db;
			
			if (! is_numeric($owner_id) )
			{
				return 'error: invalid_owner_id';	
			}
			
			$db->get_results("DELETE pl.* FROM picklists p, picklists_lookup pl WHERE pl.owner_id=$owner_id AND p.picklist_name = '".$db->escape($this->picklist_name)."' AND pl.pick_val_id=p.pick_val_id");

		}
		
		/********************************************************************
		* picklist_admin->get_text($pick_val_id)
		*/	
		
		public function get_text($pick_val_id)
		{
			global $db;
			
			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}
			
			return $db->get_row("SELECT * FROM picklists WHERE pick_val_id = '$pick_val_id'");
		}

	}

	class picklist_admin
	{

		var $picklist_name = false;
		
		/********************************************************************
		* picklist_admin->add($new_text)
		*/	
		
		public function __construct($picklist_name)
		{
			$this->picklist_name = $picklist_name;
		}

		/********************************************************************
		* picklist_admin->add($new_text)
		*/	

		public function add($new_text)
		{
			global $db;
			
			if (! $new_text = trim($new_text) )
			{
				return 'error: missing_text';	
			}
			
			$db->query("INSERT INTO picklists SET picklist_name = '".$db->escape($this->picklist_name)."', value = '".$db->escape($new_text)."', sort_order = '".($this->get_num()+1)."'");
		
			return $db->insert_id;
		}

		/********************************************************************
		* picklist_admin->edit($pick_val_id,$new_text)
		*/	

		public function edit($pick_val_id,$new_text)
		{
			global $db;
			
			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}
			
			if (! $new_text = trim($new_text) )
			{
				return 'error: missing_text';	
			}
			
			if (! $this->get($pick_val_id) )
			{
				return 'error: pick_val_not_found';
			}
			
			return $db->query("UPDATE picklists SET value = '".$db->escape($new_text)."' WHERE pick_val_id = '".$db->escape($pick_val_id)."'");
		}
		
		/********************************************************************
		* picklist_admin->remove($pick_val_id)
		*/	
		
		public function remove($pick_val_id)
		{
			global $db;

			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}
			
			$this->set_order($pick_val_id,$this->get_num());
			
			return $db->query('DELETE FROM picklists WHERE pick_val_id = ' . $pick_val_id);
		}
		
		/********************************************************************
		* picklist_admin->remove_by_text($text)
		*/	
		
		public function remove_by_text($text)
		{
			global $db;
			
			if (! $text = trim($text) )
			{
				return 'error: missing_text';	
			}
			
			$pick_val_id = $db->get_var("SELECT pick_val_id FROM picklists WHERE picklist_name = '".$db->escape($this->picklist_name)."' AND value = '".$db->escape($text)."'");
			
			if ( ! is_numeric($pick_val_id) )
			{
				return 'error: item_not_found_in_list';
			}
			
			return $this->remove($pick_val_id);
		}
		
		/********************************************************************
		* picklist_admin->set_order($pick_val_id,$new_position)
		*/	
		
		public function set_order($pick_val_id,$new_position)
		{
			global $db;
			
			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}
			
			if (! is_numeric($new_position) )
			{
				return 'error: invalid_new_position';	
			}
			
			if (! $pick_val = $this->get($pick_val_id) )
			{
				return 'error: pick_val_not_found';
			}
			
			if ($new_position < $pick_val->sort_order)
			{
				$db->query("UPDATE picklists SET sort_order = sort_order + 1 WHERE picklist_name = '".$db->escape($this->picklist_name).
				"' AND sort_order >= $new_position AND sort_order < {$pick_val->sort_order}");
			}
			else
			{
				$db->query("UPDATE picklists SET sort_order = sort_order - 1 WHERE picklist_name = '".$db->escape($this->picklist_name).
				"' AND sort_order <= $new_position AND sort_order > {$pick_val->sort_order}");
			}
			$db->query("UPDATE picklists SET sort_order = $new_position WHERE pick_val_id = $pick_val_id");
			
			return $new_position;
			
		}
		
		/********************************************************************
		* picklist_admin->bump_up($pick_val_id)
		*/	
		
		public function bump_up($pick_val_id)
		{

			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}

			if (!$current = $this->get($pick_val_id))
			{
				return "error: pick_val_not_found";
			}

			if ( $current->sort_order < $this->get_num() )
			{
				return $this->set_order($pick_val_id,$current->sort_order+1);
			}
		}
		
		/********************************************************************
		* picklist_admin->bump_down($pick_val_id)
		*/	
		
		public function bump_down($pick_val_id)
		{
			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}
			
			if (!$current = $this->get($pick_val_id))
			{
				return "error: pick_val_not_found";
			}

			if ( $current->sort_order > 1 )
			{
				return $this->set_order($pick_val_id,$current->sort_order-1);
			}
		}
		
		/********************************************************************
		* picklist_admin->get($pick_val_id)
		*/	
		
		public function get($pick_val_id)
		{
			global $db;
			
			if (! is_numeric($pick_val_id) )
			{
				return 'error: invalid_pick_val_id';	
			}
			
			return $db->get_row("SELECT * FROM picklists WHERE pick_val_id = '$pick_val_id'");
		}
		
		/********************************************************************
		* picklist_admin->
		*/	
		
		public function get_num()
		{
			global $db;
			return $db->get_var("SELECT count(*) FROM picklists WHERE picklist_name = '".$db->escape($this->picklist_name)."'");
		}
		
		/********************************************************************
		* picklist_admin->get_all
		*/	
		
		public function get_all()
		{
			global $db;
			
			$return_array = array();
			
			if ( $items = $db->get_results("SELECT pick_val_id, value FROM picklists WHERE picklist_name = '".$db->escape($this->picklist_name)."' ORDER BY sort_order") )
			{
				foreach ( $items as $item )
				{
					$return_array[$item->pick_val_id] = $item->value;
				}
			}
			
			return $return_array;
		}


		/********************************************************************
		* picklist_admin->get_all_incl_pick_val_id()
		*/	
		
		public function get_all_incl_pick_val_id()
		{
			global $db;

			return $db->get_results("SELECT pick_val_id, value FROM picklists WHERE picklist_name = '".$db->escape($this->picklist_name)."' ORDER BY sort_order");
			
		}

	}
	
?>

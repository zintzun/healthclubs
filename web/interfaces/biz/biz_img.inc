<?php

	include "/www/tds/web/interfaces/image.inc";
	include_once "/www/tds/web/lib/common.inc";
	include_once "/www/tds/web/lib/config.inc";
	include_once "/www/tds/web/lib/text_utils.inc";

  //define (BIZ_PICS_DIR,'data/pics');

	/*

	biz_pic Class
	. . . . . . . . . . . . . . . . . . . . . . . . . . . 
	Usage:

		biz_pics->store($parms)
		biz_pic->remove($biz_id,$pic_id)
		biz_pics->publish($pic_id)
		biz_pics->unpublish($pic_id)
		biz_pic->bump_up($pic_id)
		biz_pic->bump_down($pic_id)
		biz_pic->get($pic_id)
		biz_pic->get_num($biz_id)
		
	*/

	class biz_pic
	{
		/********************************************************************
		* biz_pic->store($parms)
		  
			Example of input array:
			$parms = array
			(
				'biz_id' => '0'),
				'pic_type' => 'listing'),
				'img_src_path' => '/unit-tests/gfx/uno.jpg'),
				'width' => '800'),
				'height' => '600'),
			);
			
		*/	

		public function store($parms)
		{
			global $db;
			
			$required_fields = array
			(
				array('biz_id',       '', 'missing_biz_id'),
				array('pic_type',     '', 'missing_pic_type'),
				array('img_src_path', '', 'missing_src_path'),
				array('mime_type',    '', 'missing_mime'),
			);
			
			// Test for missing required fields
			foreach ($required_fields as $A)
			{
				list($field, $value, $error)= $A;
				
				if ( ! isset($parms[$field]) || ($parms[$field] = trim($parms[$field])) == $value )
				{
					return 'error: '.$error;
				}
			}
			
			// Test for valid numeric types
			foreach ( array('biz_id') as $field)
			{
				if ( ! is_numeric($parms[$field]) )
				{
					return "error: invalid_$field";
				}
			}
			
			// Test for correct pic_type
			if ( ! in_array($parms['pic_type'], array('logo', 'listing')) )
			{
				return 'error: invalid_pic_type';
			}

			// Check image file exists
			if ( ! file_exists($parms['img_src_path']) )
			{
				return 'error: file_not_found';
			}

			// Check destination path exists
			if ( ! file_exists(BIZ_PICS_DIR) )
			{
				//mkdir(BIZ_PICS_DIR, 0700);
				mkdir('hello', 0700);
			}
			
			$filesize = filesize($parms['img_src_path']);
			
			if ( $filesize <= 0 || $filesize > MAX_PIC_UPLOAD_SIZE )
			{
				return 'error: file_is_too_big';
			}
			
			$size = getimagesize($parms['img_src_path']);

			//print_r( $size );
			//exit;
			
			// Order starts at 1
			//$set_sql = get_set_sql
			//(
				//array 
				//(
					//'biz_id'       => $parms['biz_id'],
					//'width'        => $size[0],
					//'height'       => $size[1],
					//'sort_order'   => $this->get_num($parms['biz_id']) + 1,
					//'pic_type'     => $parms['pic_type'],
					//'is_published' => (isset($parms['is_published'])? $parms['is_published']: 0)
				//)
			//);
			
			//$db->query("INSERT INTO biz_pics SET $set_sql");
			
			//$pic_id = $db->insert_id;

			$pic_id = 'he';
			
			$image = new image();
			
			
			// This stores the pic as a listing
			if ( $parms['pic_type'] == 'listing' ) 
			{
				
				//$error = is_error
				//(
					$image->convert_and_store
					(
						$from_path = $parms['img_src_path'],
						$to_path = BIZ_PICS_DIR.'/'.$parms['biz_id']."-$pic_id.jpg",
						$width = BIZ_PICS_SIZE,
						$height = BIZ_PICS_SIZE,
						$mime_type = $parms['mime_type'],
						true
					)
				//);
				
				//$error = is_error
				//(
					$image->convert_and_store
					(
						$from_path = $parms['img_src_path'],
						$to_path = BIZ_PICS_DIR.'/'.$parms['biz_id']."-$pic_id-thumb.jpg",
						$width = BIZ_PICS_THUMB_SIZE,
						$height = BIZ_PICS_THUMB_SIZE,
						$mime_type = $parms['mime_type'],
						true
					)
				//);
			}
			// This stores the pic as a logo
			else if ( $parms['pic_type'] == 'logo' )
			{				
				$error = is_error
				(
					$image->convert_and_store
					(
						$from_path = $parms['img_src_path'],
						$to_path = BIZ_PICS_DIR.'/'.$parms['biz_id']."-$pic_id.jpg",
						$width = BIZ_LOGO_SIZE,
						$height = BIZ_LOGO_SIZE,
						$mime_type = $parms['mime_type'],
						true
					)
				);			
			}

			if ($error) 
			{
				$this->remove($parms['biz_id'],$pic_id);
				return $error;
			}
			
			return $pic_id;
		}

		/********************************************************************
		* biz_pic->remove($biz_id,$pic_id)
		*/	
		
		public function remove($biz_id,$pic_id)
		{
			global $db;
		
			if ( !is_numeric($pic_id))
			{
				return 'error: invalid_pic_id';
			}
			
			if ( !is_numeric($biz_id))
			{
				return 'error: invalid_biz_id';
			}
			
			if ( ! ($current = $this->get($pic_id)) )
			{
				return "error: pic_id_not_found";
			}
			
			$this->set_order($pic_id,$this->get_num($current->biz_id));
			
			// Delete file
			@unlink(BIZ_PICS_DIR.'/'.$current->biz_id."-$pic_id.jpg");
			
			return $db->query('DELETE FROM biz_pics WHERE biz_id = ' . $biz_id . ' AND pic_id = ' . $pic_id);
			
		}

		/********************************************************************
		* biz_pics->publish($pic_id)
		*/	
		
		public function publish($pic_id)
		{
			global $db;
			
			if ($error = is_error( ($pic =$this->get(($pic_id = trim($pic_id)))) ) )
			{
				return $error;
			}
			if ( ! $pic) 
			{
				return "error: pic_id_not_found";
			}

			$db->query("UPDATE biz_pics SET is_published = 1 WHERE pic_id = $pic_id");
		}
		
		/********************************************************************
		* biz_pic->unpublish($pic_id)
		*/	
		
		public function unpublish($pic_id)
		{
			global $db;
			
			if ($error = is_error( ($pic =$this->get(($pic_id = trim($pic_id)))) ) )
			{
				return $error;
			}
			if ( ! $pic) 
			{
				return "error: pic_id_not_found";
			}
			
			$db->query("UPDATE biz_pics SET is_published = 0 WHERE pic_id = $pic_id");

		}
		
		/********************************************************************
		* biz_pic->set_order($pic_id,$new_position)
		*/	
		
		public function set_order($pic_id,$new_position)
		{
			global $db;
			
			if (! is_numeric($pic_id) )
			{
				return 'error: invalid_pic_id';	
			}
			
			if (! is_numeric($new_position) )
			{
				return 'error: invalid_new_position';	
			}
			
			if (! $pic = $this->get($pic_id) )
			{
				return 'error: pic_id_not_found';
			}
			
			if ($new_position < $pic->sort_order)
			{
				$db->query("UPDATE biz_pics SET sort_order = sort_order + 1 WHERE biz_id = ".$pic->biz_id.
					" AND sort_order >= $new_position AND sort_order < {$pic->sort_order}");
			}
			else
			{
				$db->query("UPDATE biz_pics SET sort_order = sort_order - 1 WHERE biz_id = ".$pic->biz_id.
					" AND sort_order <= $new_position AND sort_order > {$pic->sort_order}");
			}
			$db->query("UPDATE biz_pics SET sort_order = $new_position WHERE pic_id = $pic_id");
			
			return true;
			
		}

		/********************************************************************
		* biz_pic->bump_up($pic_id)
		*/	
				
		public function bump_up($pic_id)
		{
			if (! is_numeric($pic_id) )
			{
				return 'error: invalid_pic_id';	
			}

			if (! ($current = $this->get($pic_id)) )
			{
				return "error: pic_id_not_found";
			}

			if ( $current->sort_order < $this->get_num($current->biz_id) )
			{
				return $this->set_order($pic_id,$current->sort_order + 1);
			}
		}

		/********************************************************************
		* biz_pic->bump_down($pic_id)
		*/	
				
		public function bump_down($pic_id)
		{
			if (! is_numeric($pic_id) )
			{
				return 'error: invalid_pic_id';	
			}

			if ( ! ($current = $this->get($pic_id)) )
			{
				return "error: pic_id_not_found";
			}
			
			if ( $current->sort_order > 1 )
			{
				return $this->set_order($pic_id,$current->sort_order - 1);
			}
		}

		/*
		* biz_pic->get($pic_id)
		*/
		
		public function get($pic_id)
		{
			global $db;

			if ( !is_numeric($pic_id = trim($pic_id)) )
			{
				return "error: invalid_pic_id";
			}

			$this->data = $db->get_row("SELECT * FROM biz_pics WHERE pic_id = $pic_id");

			return($this->data);
		}
		
		/*
		* biz_pic->remove_logo($biz_id)
		*/
		
		public function remove_logo($biz_id)
		{
			global $db;

			if ( !is_numeric($biz_id = trim($biz_id)) )
			{
				return "error: invalid_biz_id";
			}

			if ( $logos = $db->get_col("SELECT pic_id FROM biz_pics WHERE biz_id = $biz_id AND pic_type = 'logo'") )
			{
				// Just in case there is more than 1...
				foreach ( $logos as $pic_id )
				{
					$this->remove($biz_id,$pic_id);
				}
			}
		}
		
		/*
		* biz_pic->remove_logo($biz_id)
		*/
		
		public function get_logo_path($biz_id)
		{
			global $db;

			if ( !is_numeric($biz_id = trim($biz_id)) )
			{
				return "error: invalid_biz_id";
			}

			if ( $pic_id = $db->get_var("SELECT pic_id FROM biz_pics WHERE biz_id = $biz_id AND pic_type = 'logo'") )
			{
				return "/data/pics/$biz_id-$pic_id.jpg";
			}
			
			return false;

		}
		
		/*
		* biz_pic->remove_logo($biz_id)
		*/
		
		public function get_image_paths($biz_id)
		{
			global $db;

			if ( !is_numeric($biz_id = trim($biz_id)) )
			{
				return "error: invalid_biz_id";
			}

			$paths = array();
			if ( $pic_ids = $db->get_col("SELECT pic_id FROM biz_pics WHERE biz_id = $biz_id AND pic_type = 'listing' ORDER BY sort_order") )
			{
				foreach ( $pic_ids as $pic_id )
				{
					$paths[] = array
					(
						'pic'    => "/data/pics/$biz_id-$pic_id.jpg",
						'thumb'  => "/data/pics/$biz_id-$pic_id-thumb.jpg",
						'pic_id' => $pic_id,
					);
				}
			}
			
			return $paths;

		}

		/********************************************************************
		* biz_pic->get_num($biz_id)
		*/

		public function get_num($biz_id)
		{
			global $db;
			return 100;
			//return $db->get_var("SELECT count(*) FROM biz_pics WHERE biz_id = '$biz_id' AND pic_type = 'listing'");
		}

	}


?>

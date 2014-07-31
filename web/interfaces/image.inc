<?php

	/*
   * Example usage:
	*
	*     // Note 1: requires <form enctype="multipart/form-data" method="post" etc />
	*     // Note 2: requires <input type="file" name="picture" /> in form
	*
	*		$image = new image;
	*
	*		if ( ! $_FILES['picture']['size'] )
	*		{
	*			$error = 'Please click "Browse" or "Choose file" and try again.';
	*		}
	*		else if ( ! $image->is_valid_type($_FILES['picture']['type']) )
	*		{
	*			$error = 'Sorry. The image must be of type jpeg,<br/>pjpeg, png, gif, jpg, bmp';
	*		}
	*		else if ( $_FILES['picture']['size'] > 1024000 )
	*		{
	*			$error = 'Sorry. The image was to large.<br/>We have a 1MB limit.';
	*		}
	*
	*		$image->convert_and_store
	*		(
	*			$from_path = $_FILES['picture']['tmp_name'],
	*			$to_path   = "data/pics/$biz_id$image_num$etc",
	*			$width     = 200,
	*			$height    = 200,
	*			$mime_type = $_FILES['picture']['type'],
	*			false
	*		);
	*
	*/

	class image
	{
		
		function is_valid_type($mime_type)
		{
			return preg_match('/(jpeg|pjpeg|jpg|png|gif|wbmp)/i',$mime_type);
		}

		function convert_and_store($from_path, $to_path, $width, $height, $mime_type, $maintain_ratio = true)
		{
			if ( ! $this->is_valid_type($mime_type) )
			{
				return 'error: invalid_image_mime_type';
			}
			
			if ( preg_match('/(jpeg|pjpeg|jpg)/i',$mime_type) )
			{
				$image = imagecreatefromjpeg($from_path);
			}
			else if ( preg_match('/png/i',$mime_type) )
			{
				$image = imagecreatefrompng($from_path);
			}
			else if ( preg_match('/gif/i',$mime_type) )
			{
				$image = imagecreatefromgif($from_path);
			}
			else if ( preg_match('/wbmp/i',$mime_type) )
			{
				$image = imagecreatefromwbmp($from_path);
			}
			
			if ( $maintain_ratio )
			{
				$image_x = imagesx($image);
				$image_y = imagesy($image);
				$scale = min($width/$image_x, $height/$image_y);
				$new_x = floor($scale*$image_x);
				$new_y = floor($scale*$image_y);
				$new_image = imagecreatetruecolor($new_x, $new_y);
				imagecopyresampled($new_image, $image, 0, 0, 0, 0, $new_x, $new_y, $image_x, $image_y);
			}
			else
			{
				$image_x = imagesx($image);
				$image_y = imagesy($image);
				$scale = min($width/$image_x, $height/$image_y);
				$new_x = floor($scale*$image_x);
				$new_y = floor($scale*$image_y);
				$new_image = imagecreatetruecolor($width, $height);
				imagecopyresampled($new_image, $image, 0, 0, 0, 0, $width, $height, $image_x, $image_y);
			}

			imagejpeg($new_image, $to_path);
			
		}
	}
	
?>
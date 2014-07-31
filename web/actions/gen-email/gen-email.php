<?php
header("Content-type: image/png");
$im = @imagecreate(500, 23)
    or die("Cannot Initialize new GD image stream");
$background_color = imagecolorallocate($im, 255, 255, 255 );
$text_color = imagecolorallocate($im, 100, 100, 100);
imagestring($im, 4, 3, 3,  APP_EMAIL, $text_color);
imagepng($im);
imagedestroy($im);
exit;
?> 
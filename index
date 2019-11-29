<?php

$method = $_SERVER['REQUEST_METHOD'];
$file = 'hasil.png';
switch($method){
		case 'GET':
			if(isset($_GET['draw'])){
					 $image = imagecreatefrompng($file);
					 $black = imagecolorallocate($image,0,0,0);
					 imagesetpixel($image, $_GET['x'], $_GET['y'], $black);
					 imagepng($image, $file);
			 }elseif(isset($_GET['reset'])){
					 $image = imagecreatetruecolor(320,240);
					 $white = imagecolorallocate($image,255,255,255);
					 imagefill($image,0,0,$white);
					 imagepng($image, $file);
			 }else{
					header("Content-Type: image/png");
					$image = imagecreatefrompng($file);
					imagepng($image);
				 }
		break;
	}
?>

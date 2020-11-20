<?php

//print_r($argv);

$name_file = $argv[1];
echo PHP_EOL;
echo "Имя файла ".$name_file.PHP_EOL;
//echo $argc.PHP_EOL;

$nam = explode(".", $name_file);

$fileExtension = strtolower(end($nam));

if($fileExtension == "png")
{
	$im = imagecreatefrompng($name_file);
}
else if($fileExtension == "jpg")
{
	$im = imagecreatefromjpeg($name_file);
}

$width = imagesx($im);
$height = imagesy($im);

$size_img = $width * $height * 2;

$fd_bin = fopen("/home/dima/".$nam[0].".bin", 'wb') or die("не удалось создать файл");

$fd = fopen("/home/dima/".$nam[0].".h", 'w') or die("не удалось создать файл");

fwrite($fd, ' '.PHP_EOL);
fwrite($fd, '#define IMG_WIDTH  '.$width.PHP_EOL);
fwrite($fd, '#define IMG_HEIGHT  '.$height.PHP_EOL);
fwrite($fd, '#define SIZE_IMG  '.$size_img.PHP_EOL);
fwrite($fd, ' '.PHP_EOL);
fwrite($fd, 'const uint8_t '.$nam[0].'[] = {');

$is = 0;

$flag = 0;

for($y = 0; $y < $height; $y++) 
{ 
    for($x = 0; $x < $width; $x++) 
    {
        $is ++;
        
        if($is % 10 == 0) 
        {
            fwrite($fd, PHP_EOL);
        }

        $color = imagecolorat($im, $x, $y);

        $color_tran = imagecolorsforindex($im, $color);
        //var_dump($color_tran);

        $red = $color_tran['red'];
        $green = $color_tran['green'];
        $blue = $color_tran['blue'];
        
        $r = (($red >> 3) & 0x1f) << 11; 
        $g = (($green >> 2) & 0x3f) << 5;
        $b = ($blue >> 3) & 0x1f;
        
        $mycol = ($r | $g | $b);
        
        /*if($x == 0) echo "0x";
		else echo ",0x";
		 
		echo dechex(($mycol & 0xFF00) >> 8);
		echo ",";
		echo "0x";
		echo dechex($mycol & 0xFF);*/
		
		if($flag == 0) fwrite($fd, '0x');
		else fwrite($fd, ',0x');
		 
		$byte1 = dechex(($mycol & 0xFF00) >> 8); 
		fwrite($fd, $byte1);
		$bin1 = ($mycol & 0xFF00) >> 8;
		fwrite($fd_bin, pack("C", $bin1));
		fwrite($fd, ',0x');

		$byte2 = dechex($mycol & 0xFF);
		fwrite($fd, $byte2);
		$bin2 = ($mycol & 0xFF);
		fwrite($fd_bin, pack("C", $bin2));

		$flag = 1;
    }
}

fwrite($fd, '};');
fclose($fd);
fclose($fd_bin);







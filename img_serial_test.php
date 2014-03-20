<?php
/**
 * Proof of concept.
 * Send an image to the arduino via serial
 * Arduino echos back the img
 * The image is unchanged.
 *
 */


/*
cs8 = 8 bit characters
-cstopb = one stop bit
raw = no messing with the data, like adding newlines
500000 = BAUD
*/
exec('stty -F /dev/ttyUSB0 cs8 -cstopb raw 500000');

$fp =fopen("/dev/ttyUSB0", "w+b");
if( !$fp) {
        echo "Error";die();
}

$filename = "smiley.jpg";
$size = filesize($filename);
$imgFp = fopen($filename, "rb");
// Need to read past the file size
$contents = fread($imgFp, $size+10);
fclose($imgFp);

// Send the data to the arduino via serial.
fwrite($fp, $contents);

// Get the response.
$got = fread($fp, $size);

// Save the file.
file_put_contents("got.jpg", $got);


fclose($fp);

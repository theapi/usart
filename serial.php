<?php

/*
cs8 = 8 bit characters
-cstopb = one stop bit
raw = no messing with the data, like adding newlines
500000 = BAUD
*/
exec('stty -F /dev/ttyUSB0 cs8 -cstopb raw 500000');
$fp =fopen("/dev/ttyUSB0", "w+");
if( !$fp) {
        echo "Error";die();
}


$last_send = 0;
while (1) {
  echo fread($fp, 1);
  
  $now = time();
  if ($now - $last_send > 5) {
    $last_send = $now;
    fwrite($fp, date("H:i:s", $now));
  }
}

fclose($fp);
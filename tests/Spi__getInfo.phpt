--TEST--
Spi::getInfo() member function
--SKIPIF--
<?php

if(!extension_loaded('spi')) die('skip ');

 ?>
--FILE--
<?php

$spi = new Spi(0, 0);
$info = $spi->getInfo();
if(count($info) == 6 &&
   is_int($info['descriptor']) &&
   preg_match('/\/dev\/spidev/', $info['device']) &&
   is_int($info['spi_mode']) && $info['spi_mode'] < 4 &&
   is_int($info['bits_per_word']) &&
   is_int($info['speed']) &&
   is_int($info['delay'])) {
       echo 'OK';
}

?>
--EXPECT--
OK
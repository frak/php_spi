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
if(count($info) == 5) {
    echo '1';
}
if($info['device'] == '/dev/spidev0.0') {
    echo '2';
}
if($info['spi_mode'] == 0) {
    echo '3';
}
if($info['bits'] == 8) {
    echo '4';
}
if($info['speed'] == 500000) {
    echo '5';
}
if($info['delay'] == 0) {
    echo '6';
}
?>
--EXPECT--
123456
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
if(count($info) == 6) {
    echo 'OK';
}

?>
--EXPECT--
OK
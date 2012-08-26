--TEST--
Spi::transfer() member function
--SKIPIF--
<?php

if(!extension_loaded('spi')) die('skip ');

 ?>
--FILE--
<?php

$spi = new Spi(0, 0);
$data = array(1, 2, 3, 4, 5, 6, 7, 8);
$sent = $spi->transfer($data);
if($sent == $data) {
    echo "OK";
}

?>
--EXPECT--
OK
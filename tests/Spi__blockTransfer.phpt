--TEST--
Spi::blockTransfer() member function
--SKIPIF--
<?php

if(!extension_loaded('spi')) die('skip ');

 ?>
--FILE--
<?php

$spi = new Spi(0, 0);
$data = array(array(1, 2), array(3, 4), array(5, 6), array(7, 8));
$sent = $spi->blockTransfer($data);
if($sent == $data) {
    echo "1";
}

$sent = $spi->blockTransfer($data, 1, true);
if($sent == 8) {
    echo "2";
}

?>
--EXPECT--
12
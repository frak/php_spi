--TEST--
Spi::__construct() member function
--SKIPIF--
<?php

if(!extension_loaded('spi')) die('skip ');

 ?>
--FILE--
<?php

$spi = new Spi(0, 0);
if(get_class($spi) === "Spi") {
    echo "OK";
}

?>
--EXPECT--
OK
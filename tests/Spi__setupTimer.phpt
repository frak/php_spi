--TEST--
Spi::setupTimer() member function
--SKIPIF--
<?php

if(!extension_loaded('spi') || posix_getuid() > 0) die('skip ');

 ?>
--FILE--
<?php

$spi = new Spi(0, 0);
if($spi->setupTimer()) {
    echo "OK";
}

?>
--EXPECT--
OK
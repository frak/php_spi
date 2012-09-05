--TEST--
Spi::usecDelay() member function
--SKIPIF--
<?php

if(!extension_loaded('spi') || posix_getuid() > 0) die('skip ');

 ?>
--FILE--
<?php

$spi = new Spi(0, 0);
if($spi->setupTimer()) {
    echo "1";
}
$start = microtime(true);
$spi->usecDelay(1000000);
$end =  microtime(true);
if($end - $start > 1 && $end - $start < 1.001) {
    echo "2";
}
?>
--EXPECT--
12
<?php
error_reporting(-1);
ini_set('display_errors', 1);

// $nonExistent = new Spi(5, 4, array());

$class = new Spi(0, 1, array('mode' => SPI_MODE_0));
$class->write(array(1, 2, 3, 4, 5, 6, 7, 8));

<?php
error_reporting(-1);
ini_set('display_errors', 1);

// $nonExistent = new Spi(5, 4, array());

$spi = new Spi(0, 0, array(
    'mode' => SPI_MODE_2,
    'bits' => 8,
    'speed' => 500000,
));
var_dump($spi);

$data = array(1, 2, 3, 4, 5, 6, 7, 8);
$data = $spi->transfer($data);
var_dump($data);


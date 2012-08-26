<?php
error_reporting(-1);
ini_set('display_errors', 1);

$noParams = new Spi(0, 1);

$spi = new Spi(0, 0, array(
    'mode' => SPI_MODE_1,
    'speed' => 10000000,
    'delay' => 5
));

$info = $spi->getInfo();
foreach($info as $key => $value) {
    echo "{$key} => {$value}\n";
}

$data = array(
    0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
);
$data = $spi->transfer($data);

var_dump($data);
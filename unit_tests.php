#!/usr/bin/env php
<?php

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
$read = $spi->transfer($data);

if($read == $data) {
    echo "Received data the same as sent\n";
} else {
    echo "Have you connected MOSI to MISO?\n";
}

if($spi->setupTimer()) {
    $times = array();
    $times[0] = microtime(true);
    $spi->usecDelay(1000000);
    $times[1] =  microtime(true);
    usleep(1000000);
    $times[2] = microtime(true);

    $method   = $times[1] - $times[0];
    $function = $times[2] - $times[1];
    echo "usecDelay delayed for {$method}\n";
    echo "usleep delayed for {$function}\n";
} else {
    echo "You need to run as root to test the timer\n";
}

$data = array(
    array(1, 2, 3, 4),
    array(5, 6, 7, 8),
    array(9, 10, 11, 12)
);
$read = $spi->blockTransfer($data);
if($read == $data) {
    echo "Block transferred\n";
} else {
    echo "Block transfer failed\n";
}

$read = $spi->blockTransfer($data, 1, true);
if($read == 12) {
    echo "12 items sent, 12 items returned\n";
} else {
    echo "{$read} data items returned!\n";
}
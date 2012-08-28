<?php

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
$read = $spi->transfer($data);

if($read == $data) {
    // MOSI connected to MISO
    var_dump($read);
}

if($spi->setupTimer()) {
    $times = array();
    $times[0] = microtime(true);
    $spi->usecDelay(1000000);
    $times[1] =  microtime(true);
    usleep(1000000);
    $times[2] = microtime(true);

    foreach($times as $time) {
        echo "Time: {$time}\n";
    }
} else {
    echo "You need to run as root to test the timer\n";
}


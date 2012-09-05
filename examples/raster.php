<?php

include_once 'classes/Image.php';

$image = new Image('test.png');
$spi   = new Spi(0, 0, array('speed' => 1000000));
while(true) {
    $spi->blockTransfer($image->getAll(), 1, true);
    usleep(500000);
}
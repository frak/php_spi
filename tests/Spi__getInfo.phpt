--TEST--
Spi::getInfo() member function
--SKIPIF--
<?php

if(!extension_loaded('spi')) die('skip ');

 ?>
--FILE--
<?php

$spi = new Spi(0, 0);
$info = $spi->getInfo();
if(count($info) == 6) {
    if(is_int($info['descriptor'])) {
        if(preg_match('/\/dev\/spidev.*/', $info['device'])) {
            if(is_int($info['spi_mode']) && $info['spi_mode'] < 4) {
                if(is_int($info['bits_per_word'])) {
                    if(is_int($info['speed'])) {
                        if(is_int($info['delay'])) {
                            echo 'OK';
                        } else {
                            echo 'No delay';
                        }
                    } else {
                        echo 'No speed';
                    }
                } else {
                    echo 'No bits per word';
                }
            } else {
                echo 'Bad SPI mode';
            }
        } else {
            echo 'No device';
        }
    } else {
        echo 'No descriptor';
    }
} else {
    echo 'Not 6 elements';
}

?>
--EXPECT--
OK
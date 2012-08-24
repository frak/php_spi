<?php
error_reporting(-1);
ini_set('display_errors', 1);

// $nonExistent = new Spi(5, 4, array());

$settings = new Spi(0, 1, array('mode' => 2));
var_dump($settings);


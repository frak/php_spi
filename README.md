This is a standalone PHP extension for accessing SPI on Linux systems.  I have
no idea if this will build or work on Windows as I have developed it for my RaspberryPi

BUILDING ON UNIX etc.
=====================

To compile your new extension, you will have to execute the following steps:

1.  $ ./phpize
2.  $ ./configure --enable--spi
3.  $ make
4.  $ make test
5.  $ [sudo] make install

TESTING
=======

You can now load the extension using a php.ini directive

    extension="spi.so"

or load it at runtime using the dl() function

    dl("spi.so");

The extension should now be available, you can test this
using the extension_loaded() function:

    if (extension_loaded("spi"))
        echo "spi loaded :)";
    else
        echo "something is wrong :(";

The extension will also add its own block to the output
of phpinfo();

USAGE
=====
You may find that by default that you need to run any SPI scripts as root,
to fix this you can either:

    sudo chmod 666 /dev/spidev*

Or alternatively you can create /etc/udev/rules.d/spidev.rules with the
following line in it:

    KERNEL="spidev[0-9]*", MODE="0666"

To access an SPI interface, you need to instantiate an Spi object:

    /**
     * First argument is the bus, always 0 on a Raspberry Pi
     * Second argument is the chipselect, either 0 or 1 on a Raspberry Pi
     * Third argument is an associate array of the following options:
     * "mode": The SPI mode constant - SPI_MODE_0..3
     * "bits": How many bits per word
     * "speed": The bus speed in Hz
     * "delay": Delay between bit sending
     *
     * The default values are shown in this example
     */
    $spi = new Spi(0, 0, array(
        'mode'  => SPI_MODE_0,
        'bits'  => 8,
        'speed' => 1000000,
        'delay' => 0
    ));

However, if you are OK with the default values, all you need to do is supply the
bus and chipselect parameters:

    $spi = new Spi(0, 1);

Once you are connected to the SPI device, you can then transfer data as follows:

    $spi = new Spi(0, 0, array(
        'mode' => SPI_MODE_0,
        'bits' => 8,
        'speed' => 10000000,
        'delay' => 0
    ));
    $data = array(
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
        0xF0, 0x0D,
    );
    $data = $spi->transfer($data);
    var_dump($data);

This is an alpha release, please report any issues you experience :o)

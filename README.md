This is a standalone PHP extension for accessing SPI on Linux systems.
This has been developed solely for my RaspberryPi, but it should run
on any Unix like system with SPI hardware enabled.

My inspiration for building this is the [Light Painting](http://learn.adafruit.com/light-painting-with-raspberry-pi)
project on AdaFruit. I want to build a similar device, but I want to
be able to leverage my existing PHP skills rather than feel my way
around another language. Please check the examples folder for a PHP
script for driving the display in the same way that they have done
with Python.

BUILDING
========

To be able to build PHP extensions you will need to have the dev package installed:

    $ sudo apt-get install php5-dev

To compile your new extension, you will have to execute the following steps:

    $ phpize
    $ ./configure --enable-spi
    $ make
    $ sudo make test  #this is because of the timer tests that need access to /dev/mem
    $ sudo make install

TESTING
=======

You can now load the extension using a php.ini directive

    extension="spi.so"

or load it at runtime using the dl() function

    dl("spi.so");

The extension should now be available, you can test this using the
extension_loaded() function:

    if (extension_loaded("spi"))
        echo "spi loaded :)";
    else
        echo "something is wrong :(";

The extension will also add its own block to the output of phpinfo();

USAGE
=====
You may find that by default that you need to run any SPI scripts as root,
to fix this you need to:

    sudo chmod 666 /dev/spidev*

You can fix this permanently adding this to your /etc/rc.local:

    chmod 666 /dev/spidev*

***The setupTimer/usecDelay methods make use of /dev/mem and therefore
will always need root access to run.***

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

1) display a red line on first line :  

		$data = array(
        0x00, 0xFF, 0xFF, 0x01
		);
		$data = $spi->transfer($data);
		var_dump($data);
    
2) array detail :

		$data[0] => define Red byte column
		$data[1] => define Blue byte column
		$data[2] => define Geen byte column
		$data[3] => define byte line aimed by previous color

3) if you need to display a red letter or any image, you must create a loop with a quick sleep as follows :
		
		$data = array(0xFF, 0xFF, 0xFF, 0xFF);  /* matrix 8x8 is set as Blank
		# set a array with 8 lines for your char or other.
		# sample :
		$Char_A = array(0xFF, 0xDF, OxAF, 0x77, 0x07, 0x77, 0x77, 0x77)
		while(1)
			for ($i=0, $i<8, $i++)
				$data[0] =  $Char_A[$i] /* red color value set by char_A array  
				$data[1] =  0xFF /* blue color value not set
				$data[2] =  0xFF /* green color value not set 
				$data[3] = 0x01 << $i
				$data = $spi->transfer($data);
				usleep(20)
			next
		do

Data is sent full-duplex, so if you connect your MOSI and MISO pins to each other
(GPIO pins 9 and 10) the data received will exactly match the data sent.

Whilst my tests reveal that no method for timing is ever going to be truly reliable
I have implemented a timer based on code I found for this [Magic Wand project](http://www.thebox.myzen.co.uk/Raspberry/Magic_Wand.html).
It works based on a free-running timer on the BCM2835 chip,
the delay code is very crude, and will peg your CPU at 100% whilst running, but it
does to seem work more accurately than usleep() most of the time.

To send blocks of data in sequence to the SPI bus, you can use the blockTransfer
method. This takes an array of arrays of 'columns' of data and sends each column
in sequence with a user-specified delay in milliseconds (as the optional second
argument, default is 1 millisecond).  As this method has been developed primarily
for sending data to a string of LEDs, there is an optional 3rd parameter which
when set to true discards the data read from the SPI bus and simply returns the
number of bytes sent.

This is an alpha release, please report any issues you experience :o)

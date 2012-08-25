/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Michael Davey <frak.off@gmail.com>                          |
   +----------------------------------------------------------------------+
*/

/* $ Id: $ */

#include "php_spi.h"

#if HAVE_SPI

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

/* {{{ Class definitions */

/* {{{ Class Spi */

static zend_class_entry * Spi_ce_ptr = NULL;

/* {{{ Methods */


/* {{{ proto void __construct(int bus, int chipselect[, array options])
   */
PHP_METHOD(Spi, __construct)
{
    zend_class_entry * _this_ce;
    zval * _this_zval;

    long bus = 0;
    long chipselect = 0;
    zval * options = NULL;
    HashTable * options_hash = NULL;



    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lla", &bus, &chipselect, &options) == FAILURE) {
        return;
    }

    _this_zval = getThis();
    _this_ce = Z_OBJCE_P(_this_zval);

    options_hash = HASH_OF(options);

    char device[32];
    sprintf(device, "/dev/spidev%d.%d", bus, chipselect);

    // If the device doesn't exists, error!
    if(access(device, F_OK) == -1) {
        char error[128];
        sprintf(error, "The device %s does not exist", device);
        php_error(E_ERROR, error);
    }

    // If we can't open it, error!
    long fd = open(device, O_RDWR);
    if (fd < 0) {
        char error[128];
        sprintf(error, "Could not open %s for read/write operations, are you running as root?", device);
        php_error(E_ERROR, error);
    }

    // Set the file descriptor as a class property
    zend_update_property_long(_this_ce, _this_zval, "device", 6, fd TSRMLS_DC);

    // Default property values
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = 500000;
    uint16_t delay = 0;

    // Loop through the options array
    zval **data;
    for(zend_hash_internal_pointer_reset(options_hash);
        zend_hash_get_current_data(options_hash, (void **)&data) == SUCCESS;
        zend_hash_move_forward(options_hash)) {

        char *key;
        int len;
        long index;
        long value = Z_LVAL_PP(data);

        if(zend_hash_get_current_key_ex(options_hash, &key, &len, &index, 1, NULL) == HASH_KEY_IS_STRING) {
            // Assign the value accordingly
            if(strncmp("mode", key, len) == 0) {
                switch(value) {
                    case SPI_MODE_1:
                        mode = SPI_MODE_1;
                        break;
                    case SPI_MODE_2:
                        mode = SPI_MODE_2;
                        break;
                    case SPI_MODE_3:
                        mode = SPI_MODE_3;
                        break;
                    default:
                        mode = SPI_MODE_0;
                        break;
                }
            }
            else if(strncmp("bits", key, len) == 0) {
                bits = value;
            }
            else if(strncmp("speed", key, len) == 0) {
                speed = value;
            }
            else if(strncmp("delay", key, len) == 0) {
                delay = value;
            }
        }
    }

    // Now set the options
    int ret;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if(ret == -1) {
        php_error(E_ERROR, "Couldn't set SPI mode");
    }
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1) {
        php_error(E_ERROR, "Couldn't get SPI mode");
    }
    zend_update_property_long(_this_ce, _this_zval, "mode", 4, mode TSRMLS_DC);

    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if(ret == -1) {
        php_error(E_ERROR, "Couldn't set bits per word");
    }
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1) {
        php_error(E_ERROR, "Couldn't get bits per word");
    }
    zend_update_property_long(_this_ce, _this_zval, "bits", 4, bits TSRMLS_DC);

    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
       php_error(E_ERROR, "Couldn't set speed");
    }
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
       php_error(E_ERROR, "Couldn't get speed");
    }
    zend_update_property_long(_this_ce, _this_zval, "speed", 5, speed TSRMLS_DC);

    zend_update_property_long(_this_ce, _this_zval, "delay", 5, delay TSRMLS_DC);

    php_printf("Device: %s - Mode: %d, Bits: %d, Speed: %d, Delay: %d\n", device, mode, bits, speed, delay);
}
/* }}} __construct */



/* {{{ proto void __destruct()
   */
PHP_METHOD(Spi, __destruct)
{
    zend_class_entry * _this_ce;

    zval * _this_zval = NULL;



    if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Spi_ce_ptr) == FAILURE) {
        return;
    }

    _this_ce = Z_OBJCE_P(_this_zval);

    int fd = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "device", 6, 0 TSRMLS_CC));
    close(fd);
}
/* }}} __destruct */



/* {{{ proto array transfer(array data)
   */
PHP_METHOD(Spi, transfer)
{
    zend_class_entry * _this_ce;

    zval * _this_zval = NULL;
    zval * data = NULL;
    HashTable * data_hash = NULL;



    if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa/", &_this_zval, Spi_ce_ptr, &data) == FAILURE) {
        return;
    }

    _this_ce = Z_OBJCE_P(_this_zval);

    data_hash = HASH_OF(data);



    int fd = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "device", 6, 0 TSRMLS_CC));
    uint8_t mode = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "mode", 4, 0 TSRMLS_CC));
    uint8_t bits = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "bits", 4, 0 TSRMLS_CC));
    uint32_t speed = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "speed", 5, 0 TSRMLS_CC));
    uint16_t delay = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "delay", 5, 0 TSRMLS_CC));

    int count = zend_hash_num_elements(data_hash);
    php_printf("We were passed %d elements\n", count);

    uint8_t tx[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
        0xF0, 0x0D,
    };

    zval **arr_value;
    for(zend_hash_internal_pointer_reset(data_hash);
        zend_hash_get_current_data(data_hash, (void **)&arr_value) == SUCCESS;
        zend_hash_move_forward(data_hash)) {


    }

    uint8_t rx[ARRAY_SIZE(tx)] = {0, };

    struct spi_ioc_transfer tr {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = count,
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits
    };

    int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &rt);
    if(ret < 1) {
        php_error(E_WARNING, "Can't send SPI message");
    }

    array_init(return_value);

    efree(tx);
    efree(rx);
}
/* }}} transfer */


static zend_function_entry Spi_methods[] = {
    PHP_ME(Spi, __construct, Spi____construct_args, /**/ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Spi, __destruct, Spi____destruct_args, /**/ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(Spi, transfer, Spi__transfer_args, /**/ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};

/* }}} Methods */

static void class_init_Spi(void)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Spi", Spi_methods);
    Spi_ce_ptr = zend_register_internal_class(&ce);

    /* {{{ Property registration */

    /* }}} Property registration */

}

/* }}} Class Spi */

/* }}} Class definitions*/


/* {{{ spi_module_entry
 */
zend_module_entry spi_module_entry = {
    STANDARD_MODULE_HEADER,
    "spi",
    NULL,
    PHP_MINIT(spi),     /* Replace with NULL if there is nothing to do at php startup   */
    PHP_MSHUTDOWN(spi), /* Replace with NULL if there is nothing to do at php shutdown  */
    PHP_RINIT(spi),     /* Replace with NULL if there is nothing to do at request start */
    PHP_RSHUTDOWN(spi), /* Replace with NULL if there is nothing to do at request end   */
    PHP_MINFO(spi),
    PHP_SPI_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SPI
ZEND_GET_MODULE(spi)
#endif


/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(spi)
{
    REGISTER_LONG_CONSTANT("SPI_CPOL", 0x01, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("SPI_CPHA", 0x02, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("SPI_MODE_0", 0|0, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("SPI_MODE_1", 0|SPI_CPHA, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("SPI_MODE_2", SPI_CPOL|0, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("SPI_MODE_3", SPI_CPOL|SPI_CPHA, CONST_PERSISTENT | CONST_CS);
    class_init_Spi();

    /* add your stuff here */

    return SUCCESS;
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(spi)
{

    /* add your stuff here */

    return SUCCESS;
}
/* }}} */


/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(spi)
{
    /* add your stuff here */

    return SUCCESS;
}
/* }}} */


/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(spi)
{
    /* add your stuff here */

    return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(spi)
{
    php_printf("A PHP extension for accessing SPI\n");
    php_info_print_table_start();
    php_info_print_table_row(2, "Version",PHP_SPI_VERSION " (alpha)");
    php_info_print_table_row(2, "Released", "2012-08-20");
    php_info_print_table_row(2, "CVS Revision", "$Id: $");
    php_info_print_table_row(2, "Authors", "Michael Davey 'frak.off@gmail.com' (lead)\n");
    php_info_print_table_end();
    /* add your stuff here */

}
/* }}} */

#endif /* HAVE_SPI */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

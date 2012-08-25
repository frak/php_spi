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

    if(access(device, F_OK) == -1) {
        char error[128];
        sprintf(error, "The device %s does not exist", device);
        php_error(E_ERROR, error);
    }

    long fd = open(device, O_RDWR);
    if (fd < 0) {
        char error[128];
        sprintf(error, "Could not open %s for read/write operations, are you running as root?", device);
        php_error(E_ERROR, error);
    }

    zend_update_property_long(_this_ce, _this_zval, "device", 6, fd TSRMLS_DC);

    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = 500000;
    uint16_t delay = 0;

    zval **data;
    HashPosition pointer;
    for(zend_hash_internal_pointer_reset_ex(options_hash, &pointer);
        zend_hash_get_current_data_ex(options_hash, (void**) &data, &pointer) == SUCCESS;
        zend_hash_move_forward_ex(options_hash, &pointer)) {

        zval temp;
        char *key;
        int key_len;
        long index;

//        if (zend_hash_get_current_key_ex(options_hash, &key, &key_len, &index, 0, &pointer) == HASH_KEY_IS_STRING) {
//            if(strncmp("mode", key, key_len) == 0) {
//                temp = **data;
//                zval_copy_ctor(&temp);
//                long l = Z_LVAL_PP(temp);
//                char out[64];
//                sprintf(out, "mode set to %d", mode);
//                php_error(E_NOTICE, out);
//        }
    }


    php_error(E_NOTICE, device);
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



	array_init(return_value);

	php_error(E_WARNING, "transfer: not yet implemented"); RETURN_FALSE;

	array_init(return_value);
}
/* }}} transfer */



/* {{{ proto int write(array data)
   */
PHP_METHOD(Spi, write)
{
	zend_class_entry * _this_ce;

	zval * _this_zval = NULL;
	zval * data = NULL;
	HashTable * data_hash = NULL;



	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "0a/", &_this_zval, Spi_ce_ptr, &data) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);

	data_hash = HASH_OF(data);

    int fd = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "device", 6, 0 TSRMLS_CC));
    php_printf("The file descriptor is %d\n", fd);

    int count = zend_hash_num_elements(data_hash);
    php_printf("We were passed %d elements\n", count);

    void *buffer = (void *) emalloc(count);
    if(buffer == NULL) {
        php_error(E_ERROR, "Couldn't allocate memory");
    }

    zval **arr_value;
    for(zend_hash_internal_pointer_reset(data_hash);
        zend_hash_get_current_data(data_hash, (void **)&arr_value) == SUCCESS;
        zend_hash_move_forward(data_hash)) {

        
    }
    int written = write(fd, buffer, count);
    efree(buffer);
	RETURN_LONG(written);
}
/* }}} write */



/* {{{ proto array read(int length)
   */
PHP_METHOD(Spi, read)
{
	zend_class_entry * _this_ce;

	zval * _this_zval = NULL;
	long length = 0;



	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &_this_zval, Spi_ce_ptr, &length) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);


	array_init(return_value);

	php_error(E_WARNING, "read: not yet implemented"); RETURN_FALSE;

	array_init(return_value);
}
/* }}} read */


static zend_function_entry Spi_methods[] = {
	PHP_ME(Spi, __construct, Spi____construct_args, /**/ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Spi, __destruct, Spi____destruct_args, /**/ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
	PHP_ME(Spi, transfer, Spi__transfer_args, /**/ZEND_ACC_PUBLIC)
	PHP_ME(Spi, write, Spi__write_args, /**/ZEND_ACC_PUBLIC)
	PHP_ME(Spi, read, Spi__read_args, /**/ZEND_ACC_PUBLIC)
	{ NULL, NULL, NULL }
};

/* }}} Methods */

static void class_init_Spi(void)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Spi", Spi_methods);
	Spi_ce_ptr = zend_register_internal_class(&ce);

	/* {{{ Property registration */

	zend_declare_property_long(Spi_ce_ptr, 
		"device", 6, -1, 
		ZEND_ACC_PRIVATE TSRMLS_DC);

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

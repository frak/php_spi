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



	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|a/", &bus, &chipselect, &options) == FAILURE) {
		return;
	}

	_this_zval = getThis();
	_this_ce = Z_OBJCE_P(_this_zval);

	options_hash = HASH_OF(options);



	php_error(E_WARNING, "__construct: not yet implemented"); RETURN_FALSE;

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


	php_error(E_WARNING, "__destruct: not yet implemented"); RETURN_FALSE;

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



	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa/", &_this_zval, Spi_ce_ptr, &data) == FAILURE) {
		return;
	}

	_this_ce = Z_OBJCE_P(_this_zval);

	data_hash = HASH_OF(data);



	php_error(E_WARNING, "write: not yet implemented"); RETURN_FALSE;

	RETURN_LONG(0);
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
	PHP_ME(Spi, __destruct, NULL, /**/ZEND_ACC_PUBLIC)
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
	REGISTER_LONG_CONSTANT("SPI_MODE_0", 0, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SPI_MODE_1", 0, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SPI_MODE_2", 0, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SPI_MODE_3", 0, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SPI_CPOL", 0, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SPI_CPHA", 0, CONST_PERSISTENT | CONST_CS);
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

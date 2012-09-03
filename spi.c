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

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define BCM2708_PERI_BASE        0x20000000
#define TIMER_BASE               (BCM2708_PERI_BASE + 0x00B000)
volatile unsigned *timer;

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

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|a", &bus, &chipselect, &options) == FAILURE) {
        return;
    }

    _this_zval = getThis();
    _this_ce = Z_OBJCE_P(_this_zval);

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
    zend_update_property_string(_this_ce, _this_zval, "device_name", 11, device TSRMLS_DC);

    // Default property values
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = 500000;
    uint16_t delay = 0;

    if(options != NULL) {
        options_hash = HASH_OF(options);

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

    //php_printf("Device: %s - Mode: %d, Bits: %d, Speed: %d, Delay: %d\n", device, mode, bits, speed, delay);
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

    int count = zend_hash_num_elements(data_hash);

    int fd = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "device", 6, 0 TSRMLS_CC));
    uint8_t bits = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "bits", 4, 0 TSRMLS_CC));
    uint32_t speed = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "speed", 5, 0 TSRMLS_CC));
    uint16_t delay = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "delay", 5, 0 TSRMLS_CC));

    unsigned char *tx;
    tx = emalloc(count);

    int i = 0;
    zval **arr_value;
    for(zend_hash_internal_pointer_reset(data_hash);
        zend_hash_get_current_data(data_hash, (void **)&arr_value) == SUCCESS;
        zend_hash_move_forward(data_hash)) {

        int byte = (int)Z_LVAL_PP(arr_value);
        tx[i++] = byte;
    }

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)tx, // thanks to gordonDrogon for this tip
        .len = count,
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits
    };

    int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if(ret < 1) {
        php_error(E_WARNING, "Can't send SPI message");
    }

    array_init(return_value);
    for(i = 0; i < count; ++i) {
        int value = tx[i];
        add_next_index_long(return_value, value);
    }

}
/* }}} transfer */

/* {{{ proto array blockTransfer(array data[, int colDelay[, bool discard]])
   */
PHP_METHOD(Spi, blockTransfer)
{
    zend_class_entry * _this_ce;

    zval * _this_zval = NULL;
    zval * data = NULL;
    HashTable * data_hash = NULL;
    long colDelay = 1;
    zend_bool discard = 0;

    if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa|lb", &_this_zval, Spi_ce_ptr, &data, &colDelay, &discard) == FAILURE) {
        return;
    }

    _this_ce = Z_OBJCE_P(_this_zval);

    data_hash = HASH_OF(data);

    int fd         = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "device", 6, 0 TSRMLS_CC));
    uint8_t mode   = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "mode", 4, 0 TSRMLS_CC));
    uint8_t bits   = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "bits", 4, 0 TSRMLS_CC));
    uint32_t speed = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "speed", 5, 0 TSRMLS_CC));
    uint16_t delay = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "delay", 5, 0 TSRMLS_CC));

    int row_count    = zend_hash_num_elements(data_hash);
    int column_count = 0;

    unsigned char *buffer = NULL;
    unsigned char *start  = NULL;
    unsigned char *tx     = NULL;

    zval **arr_value;
    zval **sub_value;
    zval *sub_arr;
    HashTable *arr_value_hash;

    int i, j = 0;
    for(zend_hash_internal_pointer_reset(data_hash);
        zend_hash_get_current_data(data_hash, (void **)&arr_value) == SUCCESS;
        zend_hash_move_forward(data_hash)) {

        if(Z_TYPE_PP(arr_value) == IS_ARRAY) {
            arr_value_hash = Z_ARRVAL_PP(arr_value);
            if(buffer == NULL) {
                column_count = zend_hash_num_elements(arr_value_hash);
                buffer = start = emalloc(row_count * column_count);
            }

            for(zend_hash_internal_pointer_reset(arr_value_hash);
                zend_hash_get_current_data(arr_value_hash, (void **)&sub_value) == SUCCESS;
                zend_hash_move_forward(arr_value_hash)) {

                int byte = (int)Z_LVAL_PP(sub_value);
                *buffer++ = byte;
            }
        } else {
            php_error(E_NOTICE, "Row element was not an array, skipping");
            --row_count;
        }
    }

    int ret;
    struct timespec sleeper, dummy;
    colDelay = colDelay * 1000000;
    sleeper.tv_sec  = 0;
    sleeper.tv_nsec = colDelay;
    buffer = start;

    for(i = 0; i < row_count; ++i) {
        tx = buffer + (i * column_count);
        struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)tx,
            .rx_buf = (unsigned long)tx,
            .len = column_count,
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits
        };
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if(ret < 1) {
            php_error(E_WARNING, "Can't send SPI message");
        }
        nanosleep(&sleeper, &dummy);
    }

    if(discard) {
        RETURN_LONG(row_count * column_count);
    } else {
        buffer = start;
        array_init(return_value);
        for(i = 0; i < row_count; ++i) {
            zval *row;
            ALLOC_INIT_ZVAL(row);
            array_init(row);
            for(j = 0; j < column_count; ++j) {
                add_next_index_long(row, *buffer++);
            }
            add_next_index_zval(return_value, row);
        }
    }
}
/* }}} blockTransfer */

/* {{{ proto array getInfo(void)
   */
PHP_METHOD(Spi, getInfo)
{
    zend_class_entry * _this_ce;

    zval * _this_zval = NULL;

    if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Spi_ce_ptr) == FAILURE) {
        return;
    }

    _this_ce = Z_OBJCE_P(_this_zval);

    char *device = Z_STRVAL_P(zend_read_property(_this_ce, _this_zval, "device_name", 11, 0 TSRMLS_CC));
    uint8_t mode = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "mode", 4, 0 TSRMLS_CC));
    uint8_t bits = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "bits", 4, 0 TSRMLS_CC));
    uint32_t speed = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "speed", 5, 0 TSRMLS_CC));
    uint16_t delay = Z_LVAL_P(zend_read_property(_this_ce, _this_zval, "delay", 5, 0 TSRMLS_CC));

    array_init(return_value);
    add_assoc_string(return_value, "device", device, 1);
    add_assoc_long(return_value, "spi_mode", mode);
    add_assoc_long(return_value, "bits_per_word", bits);
    add_assoc_long(return_value, "speed_hz", speed);
    add_assoc_long(return_value, "delay_usec", delay);
}
/* }}} getInfo */

/* {{{ proto bool setupTimer(void)
   */
PHP_METHOD(Spi, setupTimer)
{
    zend_class_entry * _this_ce;

    zval * _this_zval = NULL;

    if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, Spi_ce_ptr) == FAILURE) {
        return;
    }

    _this_ce = Z_OBJCE_P(_this_zval);

    char *timer_mem, *timer_map;
    int mem_fd, mem_tmr;

    /* open /dev/mem */
    if ((mem_tmr = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        php_error(E_WARNING, "Can't open /dev/mem - are you running as root?\n");
        RETURN_FALSE;
    }

    /* mmap TIMER */

    // Allocate MAP block
    if ((timer_mem = emalloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL) {
        php_error(E_ERROR, "Allocation error\n");
        RETURN_FALSE;
    }

    // Make sure pointer is on 4K boundary
    if ((unsigned long)timer_mem % PAGE_SIZE)
    timer_mem += PAGE_SIZE - ((unsigned long)timer_mem % PAGE_SIZE);

    // Now map it
    timer_map = (char *)mmap(
        (caddr_t)timer_mem,
        BLOCK_SIZE,
        PROT_READ|PROT_WRITE,
        MAP_SHARED|MAP_FIXED,
        mem_tmr,
        TIMER_BASE
    );

    if ((long)timer_map < 0) {
        php_error(E_ERROR, "mmap error timer %d\n", (int)timer_map);
        RETURN_FALSE;
    }

    // Always use volatile pointer!
    timer = (volatile unsigned *)timer_map;
    *(timer + (0x408 >> 2)) = 0xF90200;

    RETURN_TRUE;
}
/* }}} setupTimer */

void free_timer_delay(long delay)
{
    unsigned count_at_start, current_count;
    count_at_start =  *(timer+(0x420>>2)); // the value of free running counter
    current_count = count_at_start;
    while((current_count - count_at_start) < (unsigned)delay) {
        current_count = *(timer + (0x420 >> 2));
    }
}

/* {{{ proto array usecDelay(int delay)
   */
PHP_METHOD(Spi, usecDelay)
{
    zend_class_entry * _this_ce;

    zval * _this_zval = NULL;

    long delay;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &delay) == FAILURE) {
        return;
    }

    free_timer_delay(delay);
}
/* }}} usecDelay */


static zend_function_entry Spi_methods[] = {
    PHP_ME(Spi, __construct, Spi____construct_args, /**/ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Spi, __destruct, Spi____destruct_args, /**/ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(Spi, transfer, Spi__transfer_args, /**/ZEND_ACC_PUBLIC)
    PHP_ME(Spi, blockTransfer, Spi__blockTransfer_args, /**/ZEND_ACC_PUBLIC)
    PHP_ME(Spi, getInfo, Spi__getInfo_args, /**/ZEND_ACC_PUBLIC)
    PHP_ME(Spi, setupTimer, Spi__setupTimer_args, /**/ZEND_ACC_PUBLIC)
    PHP_ME(Spi, usecDelay, Spi__usecDelay_args, /**/ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};

/* }}} Methods */

static void class_init_Spi(void)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Spi", Spi_methods);
    Spi_ce_ptr = zend_register_internal_class(&ce);
}

/* }}} Class Spi */

/* }}} Class definitions*/


/* {{{ spi_module_entry
 */
zend_module_entry spi_module_entry = {
    STANDARD_MODULE_HEADER,
    "spi",
    NULL,
    PHP_MINIT(spi),
    NULL,
    NULL,
    NULL,
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

    return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(spi)
{
    php_printf("A PHP extension for accessing SPI\n");
    php_info_print_table_start();
    php_info_print_table_row(2, "Version",PHP_SPI_VERSION " (alpha)");
    php_info_print_table_row(2, "Released", "2012-08-25");
    php_info_print_table_row(2, "Authors", "Michael Davey 'frak.off@gmail.com'\n");
    php_info_print_table_end();
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

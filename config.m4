dnl
dnl $ Id: $
dnl

PHP_ARG_ENABLE(spi, whether to enable spi functions,
[  --enable-spi         Enable spi support])

if test "$PHP_SPI" != "no"; then
  export OLD_CPPFLAGS="$CPPFLAGS"
  export CPPFLAGS="$CPPFLAGS $INCLUDES -DHAVE_SPI"

  AC_MSG_CHECKING(PHP version)
  AC_TRY_COMPILE([#include <php_version.h>], [
#if PHP_VERSION_ID < 50000
#error  this extension requires at least PHP version 5.0.0
#endif
],
[AC_MSG_RESULT(ok)],
[AC_MSG_ERROR([need at least PHP 5.0.0])])

  export CPPFLAGS="$OLD_CPPFLAGS"


  PHP_SUBST(SPI_SHARED_LIBADD)
  AC_DEFINE(HAVE_SPI, 1, [ ])

  PHP_NEW_EXTENSION(spi, spi.c , $ext_shared)

fi


dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([libeasy], [0.1])

#init

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PREFIX_DEFAULT(/usr)

m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR

AC_PROG_CC
AC_PROG_LN_S
AC_PROG_LIBTOOL
AC_PROG_RANLIB

#FLAGS
PRESET_CFLAGS="$CFLAGS"
PRESET_LDFLAGS="$LDFLAGS"
CFLAGS="$PRESET_CFLAGS -Wall -fPIC -D_GNU_SOURCE -fno-strict-aliasing"

AC_CHECK_LIB([pthread], [main])
AC_CHECK_LIB([tcmalloc], [main])

EASY_INCLUDES="-I`pwd`/include/ -I`pwd`/memory -I`pwd`/util/"
EASY_INCLUDES="$EASY_INCLUDES -I`pwd`/thread/ -I`pwd`/io/ -I`pwd`/packet/ -I`pwd`/packet/http -I."
AC_SUBST(EASY_INCLUDES)

#config
if test "$prefix" = "NONE"; then
        prefix="/usr"
fi

AC_CONFIG_FILES(Makefile
                 include/Makefile
                 memory/Makefile
                 util/Makefile
                 thread/Makefile
                 io/Makefile
                 packet/Makefile
               )
AC_OUTPUT

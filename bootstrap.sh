#!/bin/bash

if [ "$1" == "clean" ]; then
    make distclean > /dev/null 2>&1
    rm -rf aclocal.m4 autom4te.cache config.guess config.sub configure depcomp INSTALL install-sh ltmain.sh missing config.log  config.status libtool COPYING compile test-driver
    find . -name 'Makefile.in' -exec rm -f {} \;
    find . -name 'Makefile' -exec rm -f {} \;
    exit;
fi
LIBTOOLIZE=libtoolize
OS=$(uname -s)
if [ $OS = "Darwin" ]; then
    # mac
    LIBTOOLIZE=glibtoolize
fi
$LIBTOOLIZE --force
aclocal
autoconf
automake --add-missing --force


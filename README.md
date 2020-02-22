# libeasy

## Build

### Requirements

* libtoolize(glibtoolize under MacOS)
* aclocal
* autoconf
* automake

### Compile

```shell
prefix=$(pwd)
./bootstrap.sh
./configure --prefix=$prefix/.libs --libdir=$prefix/.libs/lib64 --enable-static=yes --enable-shared=no
make -j
```

### Test

```shell
test/gconv_test.sh
```

### Install

We specify a temporary installation location `./.libs`. We can install by:

```shell
make install
```

AM_CPPFLAGS=-I${top_srcdir}/test/

LDADD=${PRESET_LDADD}
if !OSX
if GCC_LEGACY
    LDADD+= -lrt -lstdc++
endif
endif
noinst_PROGRAMS = include_test
TESTS = $(noinst_PROGRAMS)
include_test_SOURCES = include_test.c \
    easy_atomic_test.c                \
    easy_list_test.c

am__DEPENDENCIES_1=${PRESET_LDADD}
${PRESET_LDADD}:
	rm -f ${top_srcdir}/src/libeasy.la
	cd ${top_srcdir}/src && make

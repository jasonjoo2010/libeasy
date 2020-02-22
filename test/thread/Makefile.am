AM_CPPFLAGS=-I${top_srcdir}/test/

LDADD=${PRESET_LDADD}
AM_LDFLAGS = -lpthread
noinst_PROGRAMS = thread_test
TESTS = $(noinst_PROGRAMS)
thread_test_SOURCES = thread_test.c \
	easy_uthread_test.c

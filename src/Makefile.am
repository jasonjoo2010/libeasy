LIBDIRS = util thread io packet memory
SUBDIRS = include $(LIBDIRS) 
EXTRA_DIST = libeasy.map

if !OSX
LDFLAGS += -Wl,--version-script=${VERSION_MAP}
endif
LDFLAGS += -version-number 1:0:17
lib_LTLIBRARIES=libeasy.la
libeasy_la_SOURCES=
libeasy_la_LIBADD=util/libeasy_util.la \
                  thread/libeasy_thread.la \
                  io/libeasy_io.la \
                  packet/libeasy_packet.la \
                  memory/libeasy_memory.la


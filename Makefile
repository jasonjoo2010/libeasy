ifeq ($(shell uname), Darwin)
NAME=libeasy.dylib
else
NAME=libeasy.so
endif
CC=gcc
OPTIMIZATION?=-O2
INCLUDES=-Iinclude \
		-Iio \
		-Iio/ev \
		-Ipacket \
		-Ipacket/http \
		-Ithread \
		-Imemory \
		-Iutil \
		-I.
CFLAGS+=-Wall -fPIC -D_GNU_SOURCE -fno-strict-aliasing $(OPTIMIZATION) $(INCLUDES)
LDFLAGS+=
SRC:=$(wildcard io/*.c memory/*.c thread/*.c packet/http/*.c util/*.c)
OBJ:=$(SRC:%.c=%.o)

default: libev.a $(OBJ)
	$(CC) $(LDFLAGS) -shared -fPIC -o $(NAME) $(OBJ) io/ev/.libs/ev.o io/ev/.libs/event.o

io/ev/.libs/libev.a:
	cd $(shell pwd)/io/ev && ./configure && make

.PHONY : clean libev.a
clean:
	rm -f $(OBJ) $(NAME)

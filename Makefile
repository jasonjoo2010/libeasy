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
HEADER:=$(wildcard include/*.h io/*.h smemory/*.h packet/*.h packet/http/*.h thread/*.h util/*.h)
SRC_TEST:=$(wildcard test/*.c)
TARGET_TEST:=$(SRC:%.c=%)
OBJ:=$(SRC:%.c=%.o)

default: libev.a $(OBJ)
	$(CC) $(LDFLAGS) -shared -fPIC -o $(NAME) $(OBJ) io/ev/.libs/ev.o io/ev/.libs/event.o

io/ev/.libs/libev.a:
	cd $(shell pwd)/io/ev && ./configure && make

.PHONY : clean libev.a install test

install: $(NAME)
	install -m 0755 $(NAME) /usr/local/lib
	install -m 0644 $(HEADER) /usr/local/include
	
test: 
	for i in $(SRC_TEST); do \
		cc $(CFLAGS) -o $${i%.*} -leasy $$i; \
	done

clean:
	rm -f $(OBJ) $(NAME)

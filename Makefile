ifeq ($(shell uname), Darwin)
NAME=libeasy.dylib
else
NAME=libeasy.so
endif
NAME_STATIC=libeasy.a
CC=cc
OPTIMIZATION?=
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

default: io/ev/.libs/libev.a $(OBJ)
	$(CC) $(LDFLAGS) -shared -fPIC -o $(NAME) $(OBJ) io/ev/.libs/ev.o io/ev/.libs/event.o
	ar -r $(NAME_STATIC) $(OBJ) io/ev/.libs/ev.o io/ev/.libs/event.o

io/ev/.libs/libev.a:
	cd $(shell pwd)/io/ev && ./configure && make

.PHONY : clean install test

install: $(NAME)
	install -m 0755 $(NAME) /usr/local/lib
	install -m 0644 $(HEADER) /usr/local/include
	
test: 
	for i in $(SRC_TEST); do \
		cc $(INCLUDES) -o $${i%.*} $$i -L. -leasy -lpthread -lm; \
	done

clean:
	rm -f $(OBJ) $(NAME)

NAME=libeasy.so
CC=gcc
OPTIMIZATION?=-O2
INCLUDES=-I$(shell pwd)/include \
		-I$(shell pwd)/io \
		-I$(shell pwd)/packet \
		-I$(shell pwd)/packet/http \
		-I$(shell pwd)/thread \
		-I$(shell pwd)/memory \
		-I$(shell pwd)/util \
		-I.
CFLAGS+=-Wall -fPIC -D_GNU_SOURCE -fno-strict-aliasing $(OPTIMIZATION) $(INCLUDES)
SRC:=$(wildcard io/*.c memory/*.c thread/*.c packet/http/*.c util/*.c)
OBJ:=$(SRC:%.c=%.o)

default: $(OBJ)

.PHONY : clean
clean:
	rm -f $(OBJ)

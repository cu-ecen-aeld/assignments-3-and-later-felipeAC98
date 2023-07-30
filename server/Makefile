CC = gcc
CFLAGS=  -Wall -Werror
CROSS= $(CCOMPILE)gcc

all: _default

_default: aesdsocket.c
	$(CC) -o aesdsocket aesdsocket.c

clean : 
	rm -f aesdsocket && rm -f aesdsocket.o

CROSS_COMPILE:
	$(CROSS) -c aesdsocket.c -o aesdsocket.o
	$(CROSS) aesdsocket.o -o aesdsocket

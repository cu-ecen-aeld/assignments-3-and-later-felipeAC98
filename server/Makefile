CC = gcc
CFLAGS= -Wall -Werror
CROSS= $(CCOMPILE)gcc

all: default

default: aesdsocket.c
	$(CC) -o aesdsocket aesdsocket.c

clean : 
	rm -f aesdsocket && rm -f aesdsocket.o

CROSS_COMPILE:
	$(CROSS) $(CFLAGS) aesdsocket.c -o aesdsocket.o
	$(CROSS) aesdsocket.o -o aesdsocket

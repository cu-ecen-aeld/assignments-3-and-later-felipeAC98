CC = gcc
CFLAGS= -c
CROSS= $(CCOMPILE)gcc

all: _default

_default: writer.c
	$(CC) $(CFLAGS) -o writer writer.c

clean : 
	rm -f writer && rm -f writer.o

CROSS_COMPILE:
	$(CROSS) $(CFLAGS) writer.c -o writer.o
	$(CROSS) writer.o -o writer

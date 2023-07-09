CC = gcc
CFLAGS= -Wall -Wextra
CROSS= aarch64-none-linux-gnu-gcc

all: _default

_default: writer.c
	$(CC) $(CFLAGS) -o writer writer.c

clean : 
	rm -f writer && rm -f writer.o

CROSS_COMPILE:
	$(CROSS) -g $(CFLAGS) -c -o writer.o writer.c && file writer.o > ./../assignments/assignment2/fileresult.txt

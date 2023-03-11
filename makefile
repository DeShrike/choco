CC=gcc
AR=ar
CFLAGS=-O3 -Wall
LIBS=-lm

all: choco

choco: choco.o ini.o
	$(CC) -o $@ choco.o ini.o $(LIBS)

choco.o: choco.c ini.h
	$(CC) $(CFLAGS) -c $<

ini.o: ini.c ini.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o

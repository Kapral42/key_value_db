ZLIB=
DZLIB=
ifdef zlib
ZLIB=-lz
DZLIB=-DZLIB
endif

CC=gcc
CFLAGS= -Wall -fopenmp -g -std=c11 $(DZLIB)
LDFLAGS= -fopenmp $(ZLIB)

SOURCES_S=src/database.c src/hash.c src/hashtab.c src/server.c src/io.c
SOURCES_C=src/client.c

OBJECTS_S=$(SOURCES_S:.c=.o)
OBJECTS_C=$(SOURCES_C:.c=.o)

all: $(SOURCES_S) server client

server: $(OBJECTS_S)
	$(CC) $(LDFLAGS) $(OBJECTS_S) -o $@

client: $(OBJECTS_C)
	$(CC) $(LFLAGS) $(OBJECTS_C) -o client

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f src/*.o server client

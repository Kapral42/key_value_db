CC=gcc
CFLAGS= -Wall -fopenmp -g
LDFLAGS= -fopenmp

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

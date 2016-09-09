CC=gcc
CFLAGS= -Wall -openmp -g
LDFLAGS= -openmp

SOURCES_S=src/database.c src/hash.c src/hashtab.c src/server.c src/io.c

OBJECTS_S=$(SOURCES_S:.c=.o)
	SERVER=server

all: $(SOURCES_S) $(SERVER) client

$(SERVER): $(OBJECTS_S)
	$(CC) $(LDFLAGS) $(OBJECTS_S) -o $@

client:
	$(CC) $(CFLAGS) src/client.c -o client

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f src/*.o server client

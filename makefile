CC=gcc
CFLAGS=-Wall -Wextra -g
LDFLAGS=-lpthread
SOURCES=src/main.c src/io.c src/utils.c src/network.c src/config.c src/commandManager.c src/list.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Trinity

all: $(SOURCES) $(EXECUTABLE)
	rm -f src/*.o *~

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.o:
	$(CC) $(CFLAGS) $< -o $@
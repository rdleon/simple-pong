CC=gcc
CFLAGS=-pipe -O2 -std=c11 -Wall -Wextra
LDFLAGS=-I. $(shell pkg-config --cflags --libs sdl2 SDL2_image)

DEPS = src/images.h src/fonts.h src/game.h src/config.h src/menu.h
OBJ = src/netpong.o src/images.o src/fonts.o src/game.o src/menu.o

EXECUTABLES=netpong

all: $(EXECUTABLES)

debug: CFLAGS += -g
debug: $(EXECUTABLES)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

netpong: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f src/*.o
	@rm -f $(EXECUTABLES)

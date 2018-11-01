CC=gcc
CFLAGS=-pipe -O2 -std=c11 -g -Wall -Wextra -I.
LDFLAGS=$(shell pkg-config --cflags --libs sdl2 SDL2_image)

DEPS = src/images.h src/fonts.h
OBJ = src/netpong.o src/images.c src/fonts.c

EXECUTABLES=netpong

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDFLAGS)

netpong: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	@rm -f $(EXECUTABLES)

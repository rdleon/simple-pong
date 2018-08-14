CC=gcc
CFLAGS=-pipe -O2 -std=c11 -g -Wall -Wextra -lm
LDFLAGS=$(shell pkg-config --cflags --libs sdl2 SDL2_image)

EXECUTABLES=netpong

all: $(EXECUTABLES)

netpong: src/netpong.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	@rm $(EXECUTABLES)

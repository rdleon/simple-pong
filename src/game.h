#ifndef GAME_H
#define GAME_H

#include "config.h"

#include <stdio.h>
#include <stdint.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

struct direction_vector {
    int8_t x;
    int8_t y;
};

struct ball {
    SDL_Rect rect;
    struct direction_vector direction;
    int8_t speed;
};

struct player {
    SDL_Rect rect;
    uint8_t score;
};

enum game_state {Menu, Controls, Running, Quit};

void game_init();
void game_loop(const Uint8 *keyboard_state);
void game_quit();

struct game {
    enum game_state state;

    struct {
        unsigned int width;
        unsigned int height;
        const char* name;
        SDL_Window* window;
        SDL_Renderer* renderer;
    } screen;

    struct {
        SDL_Texture *background;
        SDL_Texture *ball;
        SDL_Texture *paddle;
    } textures;

    SDL_Rect cursor;
    struct ball ball;
    struct player player1;
    struct player player2;

    unsigned int max_score;
};

#endif

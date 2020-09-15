#ifndef GAME_H
#define GAME_H

#include <stdio.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "config.h"

struct direction_vector {
    int x;
    int y;
};

struct ball_t {
    SDL_Rect rect;
    struct direction_vector direction;
    int speed;
};

struct player_t {
    SDL_Rect rect;
    Uint32 score;
};

Uint32 frame_limit(Uint32, const Uint32);
void game_init();
void game_loop(const Uint8*);
void game_quit();

struct game_t {
    SDL_bool running;

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

    struct ball_t ball;
    struct player_t player1;
    struct player_t player2;

    void (*init)();
    void (*loop)(const Uint8*);
    void (*quit)();
};

#endif

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

void check_collisions(struct player_t*, struct player_t*, struct ball_t*);
Uint32 frame_limit(Uint32, const Uint32);
void follow_ball(SDL_Rect*, SDL_Rect*);
void reset_ball(SDL_Rect*, int*, float*, int);
void check_events(const Uint8*, int*);
void game_init();
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
    void (*quit)();
};

#endif

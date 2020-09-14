#ifndef GAME_H
#define GAME_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "config.h"
void game_init();
void game_quit();

const Uint32 FRAMES_PER_SECOND = 60;

static struct {
    SDL_bool running;
    struct {
        unsigned int width;
        unsigned int height;
        const char* name;
        SDL_Window* window;
        SDL_Renderer* renderer;
    } screen;

    void (*init)();
    void (*quit)();
    Uint32 score1;
    Uint32 score2;
} Game = {
    SDL_FALSE,
    {
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SCREEN_NAME,
        NULL,
        NULL
    },
    game_init,
    game_quit,
    0, 0
};

#endif

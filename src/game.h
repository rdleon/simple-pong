#ifndef GAME_H
#define GAME_H

#include "game.h"

#ifndef M_PI
#define M_PI 3.14159
#endif

#ifndef M_PI_2
#define M_PI_2 1.5708
#endif

struct direction_vector {
    int8_t x;
    int8_t y;
};

struct ball {
    SDL_Rect rect;
    struct direction_vector direction;
    int8_t speed;
};

struct controls {
	SDL_Scancode up;
	SDL_Scancode down;
};

struct player {
    SDL_Rect rect;
    uint8_t score;
    struct controls controls;
};

enum game_state {Menu, Running, Options, Quit};

void game_init();
void game_reset();
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

    struct {
        Mix_Chunk *wallbounce;
        Mix_Chunk *paddlebounce;
        Mix_Chunk *scorepoint;
    } sounds;

    SDL_Rect cursor;
    struct ball ball;
    struct player player1;
    struct player player2;

    unsigned int max_score;
    int two_players;
};

#endif

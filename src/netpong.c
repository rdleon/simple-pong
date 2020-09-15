#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "config.h"
#include "game.h"
#include "images.h"
#include "fonts.h"

extern struct game_t Game;

int main()
{
    time_t t;
    srand((unsigned) time(&t));
    Uint32 last_tick = SDL_GetTicks();

    game_init();

    load_font(Game.screen.renderer, "images/good_neighbors.png", "data/font.txt");

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    while (Game.running) {
        // moving is the speed and direction of the paddle
        Game.loop(keyboardState);

        SDL_RenderPresent(Game.screen.renderer);

        // Add delay to match frame rate
        last_tick = frame_limit(last_tick, FRAMES_PER_SECOND);
    }

    Game.quit();

    return 0;
}

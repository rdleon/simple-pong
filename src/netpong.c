#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "config.h"
#include "menu.h"
#include "game.h"
#include "fonts.h"

extern struct game Game;

Uint32 frame_limit(Uint32 last_tick, const Uint32 frame_limit)
{
    Uint32 elapsed_ms = (SDL_GetTicks() - last_tick);

    if (elapsed_ms < (1000 / frame_limit))
    {
        SDL_Delay((1000 / frame_limit) - elapsed_ms);
    }

    return SDL_GetTicks();
}

int main()
{
    time_t t;
    srand((unsigned) time(&t));
    Uint32 last_tick = SDL_GetTicks();

    game_init();

    load_font(Game.screen.renderer, "images/good_neighbors.png", "data/font.txt");

    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);

    while (Game.state != Quit) {
        // moving is the speed and direction of the paddle
        switch (Game.state) {
        case Menu:
            menu_loop(keyboard_state);
            break;
        case Running:
            Game.loop(keyboard_state);
            break;
        case Quit:
        default:
            break;
        }

        SDL_RenderPresent(Game.screen.renderer);

        // Add delay to match frame rate
        last_tick = frame_limit(last_tick, FRAMES_PER_SECOND);
    }

    Game.quit();

    return EXIT_SUCCESS;
}

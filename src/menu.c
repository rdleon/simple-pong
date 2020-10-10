#include "menu.h"

extern struct game Game;

void menu_loop(const Uint8 *keyboard_state)
{
    SDL_Event event;

    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    if (keyboard_state[SDL_SCANCODE_RETURN]) {
        Game.state = Running;
    } else if (keyboard_state[SDL_SCANCODE_Q]) {
        Game.state = Quit;
    }

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Game.state = Quit;
                break;
        }
    }
}

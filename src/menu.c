#include "menu.h"

extern struct game Game;

void menu_loop(const Uint8 *keyboard_state)
{
    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    draw_text(Game.screen.renderer, "NetPong", SCREEN_WIDTH / 3, 30, 5);
    draw_text(Game.screen.renderer, "Press ENTER to start", SCREEN_WIDTH / 3, 260, 2);

    if (keyboard_state[SDL_SCANCODE_RETURN]) {
        Game.state = Running;
    } else if (keyboard_state[SDL_SCANCODE_Q]) {
        Game.state = Quit;
    }
}

#include "config_controls.h"
#include "config.h"

extern struct game Game;

void menu_controls_loop(const Uint8 *keyboard_state)
{
    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    draw_text(Game.screen.renderer, "Controls", SCREEN_WIDTH / 3, 30, 5);

    draw_text(Game.screen.renderer, "UP: Move up", SCREEN_WIDTH / 4, 160, 3);
    draw_text(Game.screen.renderer, "DOWN: Move down", SCREEN_WIDTH / 4, 230, 3);
    draw_text(Game.screen.renderer, "Q: Go back to menu", SCREEN_WIDTH / 4, 300, 3);

    if (keyboard_state[SDL_SCANCODE_RETURN] || keyboard_state[SDL_SCANCODE_Q]) {
        Game.state = Menu;
        SDL_Delay(DEBOUNCE_WAIT);
    }
}

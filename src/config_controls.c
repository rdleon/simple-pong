#include "simplepong.h"

extern struct game Game;

void menu_controls_loop(const Uint8 *keyboard_state)
{
    int quarter_screen = SCREEN_WIDTH / 4;
    int half_screen = SCREEN_WIDTH / 2;
    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    draw_text(Game.screen.renderer, "Configuration", SCREEN_WIDTH / 5, 30, 5);

    draw_text(Game.screen.renderer, "P1 UP:", quarter_screen, 160, 2);
    draw_text(Game.screen.renderer, "Arrow UP", half_screen, 160, 2);
    draw_text(Game.screen.renderer, "P1 DOWN:", quarter_screen, 200, 2);
    draw_text(Game.screen.renderer, "Arrow DOWN", half_screen, 200, 2);
    draw_text(Game.screen.renderer, "P2 UP:", quarter_screen, 280, 2);
    draw_text(Game.screen.renderer, "O", half_screen, 280, 2);
    draw_text(Game.screen.renderer, "P2 DOWN:", quarter_screen, 320, 2);
    draw_text(Game.screen.renderer, "L", half_screen, 320, 2);
    draw_text(Game.screen.renderer, "Q: Go back to menu", quarter_screen, 400, 2);

    if (keyboard_state[SDL_SCANCODE_RETURN] || keyboard_state[SDL_SCANCODE_Q]) {
        Game.state = Menu;
        SDL_Delay(DEBOUNCE_WAIT);
    }
}

#include "simplepong.h"

extern struct game Game;

enum control_menu_option {SetP1Up, SetP1Down, SetP2Up, SetP2Down, QuitConfig};
int option_heights[] = {160, 200, 280, 320, 400};
int option_heights_count = 5;

void menu_controls_loop(const Uint8 *keyboard_state)
{
	static enum control_menu_option option = SetP1Up;

    int quarter_screen = SCREEN_WIDTH / 4;
    int half_screen = SCREEN_WIDTH / 2;

    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    draw_text(Game.screen.renderer, "Configuration", SCREEN_WIDTH / 5, 30, 5);

    draw_text(Game.screen.renderer, "P1 UP:", quarter_screen, option_heights[0], 2);
    draw_text(Game.screen.renderer, "Arrow UP", half_screen, option_heights[0], 2);
    draw_text(Game.screen.renderer, "P1 DOWN:", quarter_screen, option_heights[1], 2);
    draw_text(Game.screen.renderer, "Arrow DOWN", half_screen, option_heights[1], 2);
    draw_text(Game.screen.renderer, "P2 UP:", quarter_screen, option_heights[2], 2);
    draw_text(Game.screen.renderer, "O", half_screen, option_heights[2], 2);
    draw_text(Game.screen.renderer, "P2 DOWN:", quarter_screen, option_heights[3], 2);
    draw_text(Game.screen.renderer, "L", half_screen, option_heights[3], 2);
    draw_text(Game.screen.renderer, "Go back to main menu", quarter_screen, option_heights[4], 2);

    Game.cursor.x = quarter_screen - 40;
    Game.cursor.y = option_heights[option];

    SDL_RenderCopy(Game.screen.renderer, Game.textures.ball, NULL, &Game.cursor);

    if (keyboard_state[SDL_SCANCODE_RETURN] || keyboard_state[SDL_SCANCODE_Q]) {
        Game.state = Menu;
        SDL_Delay(DEBOUNCE_WAIT);
    } else if (option < (enum control_menu_option) (option_heights_count - 1) && keyboard_state[SDL_SCANCODE_DOWN]) {
        option += 1;
        SDL_Delay(DEBOUNCE_WAIT);
    } else if (option > 0 && keyboard_state[SDL_SCANCODE_UP]) {
        option -= 1;
        SDL_Delay(DEBOUNCE_WAIT);
    }
}

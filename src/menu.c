#include "simplepong.h"

extern struct game Game;

void menu_main_loop(const Uint8 *keyboard_state)
{
    static enum menu_option selected = MenuStart;

    const int option_position = 260;
    const int option_spacing = 40;
    const int option_align = (SCREEN_WIDTH / 4) + option_spacing;

    const int start_y = option_position;
    const int controls_y = option_position + option_spacing;
    const int quit_y = option_position + (2 * option_spacing);

    int cursor_y = option_position;

    enum menu_option up_action = MenuStart;
    enum menu_option down_action = MenuControls;
    enum game_state enter_action = Running;

    switch (selected) {
    case MenuStart:
        cursor_y = start_y;
        up_action = MenuStart;
        down_action = MenuControls;
        enter_action = Running;
        break;
    case MenuControls:
        cursor_y = controls_y;
        up_action = MenuStart;
        down_action = MenuQuit;
        enter_action = Controls;
        break;
    case MenuQuit:
        cursor_y = quit_y;
        up_action = MenuControls;
        down_action = MenuQuit;
        enter_action = Quit;
        break;
    }

    Game.cursor.x = option_align - 40;
    Game.cursor.y = cursor_y;

    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    draw_text(Game.screen.renderer, "SimplePong", SCREEN_WIDTH / 3, 30, 5);

    draw_text(Game.screen.renderer, "Start", option_align, start_y, MENU_OPTION_TEXT_SCALE);
    draw_text(Game.screen.renderer, "Controls", option_align, controls_y, MENU_OPTION_TEXT_SCALE);
    draw_text(Game.screen.renderer, "Quit", option_align, quit_y, MENU_OPTION_TEXT_SCALE);

    SDL_RenderCopy(Game.screen.renderer, Game.textures.ball, NULL, &Game.cursor);

    if (keyboard_state[SDL_SCANCODE_RETURN]) {
        Game.state = enter_action;
        SDL_Delay(DEBOUNCE_WAIT);
    } else if (keyboard_state[SDL_SCANCODE_DOWN]) {
        selected = down_action;
        SDL_Delay(DEBOUNCE_WAIT);
    } else if (keyboard_state[SDL_SCANCODE_UP]) {
        selected = up_action;
        SDL_Delay(DEBOUNCE_WAIT);
    }
}

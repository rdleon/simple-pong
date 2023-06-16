#include "simplepong.h"

extern struct game Game;

void menu_main_loop(const Uint8 *keyboard_state)
{
    static enum menu_option selected = MenuSinglePlayer;

    const int option_position = 260;
    const int option_spacing = 40;
    const int option_align = (SCREEN_WIDTH / 4) + option_spacing;

    const int singleplayer_y = option_position;
    const int twoplayers_y = option_position + option_spacing;
    const int controls_y = option_position + (2 * option_spacing);
    const int quit_y = option_position + (3 * option_spacing);

    int cursor_y = option_position;

    enum menu_option up_action = MenuSinglePlayer;
    enum menu_option down_action = MenuOptions;
    enum game_state enter_action = Running;

    switch (selected) {
    case MenuSinglePlayer:
        cursor_y = singleplayer_y;
        up_action = MenuSinglePlayer;
        down_action = MenuTwoPlayers;
        enter_action = Running;
	Game.two_players = 0;
        break;
    case MenuTwoPlayers:
        cursor_y = twoplayers_y;
        up_action = MenuSinglePlayer;
        down_action = MenuOptions;
        enter_action = Running;
	Game.two_players = 1;
        break;
    case MenuOptions:
        cursor_y = controls_y;
        up_action = MenuTwoPlayers;
        down_action = MenuQuit;
        enter_action = Options;
        break;
    case MenuQuit:
        cursor_y = quit_y;
        up_action = MenuOptions;
        down_action = MenuQuit;
        enter_action = Quit;
        break;
    }

    Game.cursor.x = option_align - 40;
    Game.cursor.y = cursor_y;

    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    draw_text(Game.screen.renderer, "SimplePong", SCREEN_WIDTH / 3, 30, 5);

    draw_text(Game.screen.renderer, "Single player", option_align, singleplayer_y, MENU_OPTION_TEXT_SCALE);
    draw_text(Game.screen.renderer, "Two players", option_align, twoplayers_y, MENU_OPTION_TEXT_SCALE);
    draw_text(Game.screen.renderer, "Options", option_align, controls_y, MENU_OPTION_TEXT_SCALE);
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

#include "simplepong.h"

extern struct game Game;

enum control_menu_option {SetP1Up, SetP1Down, SetP2Up, SetP2Down, QuitConfig};
int option_heights[] = {160, 200, 280, 320, 400};
int option_heights_count = 5;

int is_permitted_key(SDL_Scancode key, SDL_Scancode old_key)
{
	int is_valid = 1;

	SDL_Scancode forbidden[] = {
		SDL_SCANCODE_ESCAPE,
		SDL_SCANCODE_RETURN,
		Game.player1.controls.up,
		Game.player1.controls.down,
		Game.player2.controls.up,
		Game.player2.controls.down
	};
	int forbidden_len = sizeof forbidden / sizeof forbidden[0];

	if (key == old_key) {
		return is_valid;
	}


	for (int i = 0; i < forbidden_len; i++) {
		if (forbidden[i] == key) {
			is_valid = 0;
			break;
		}
	}

	return is_valid;
}

SDL_Scancode get_pressed_key(SDL_Scancode original_key)
{
	SDL_Event event;
	SDL_Scancode pressed_key = original_key;
	int got_key = 0;

	while (got_key == 0) {
		SDL_PollEvent(&event);
		switch (event.type) {
		case SDL_KEYDOWN:
			pressed_key = event.key.keysym.scancode;
			got_key = 1;
			break;
		default:
			break;
		}
	}

	if (!is_permitted_key(pressed_key, original_key)) {
		return original_key;
	}

	return pressed_key;
}

void menu_controls_loop(const Uint8 *keyboard_state)
{
	static enum control_menu_option option = SetP1Up;
	static int setting_control = 0;

	const char *player1_up = SDL_GetScancodeName(Game.player1.controls.up);
	const char *player1_down = SDL_GetScancodeName(Game.player1.controls.down);

	const char *player2_up = SDL_GetScancodeName(Game.player2.controls.up);
	const char *player2_down = SDL_GetScancodeName(Game.player2.controls.down);

    int quarter_screen = SCREEN_WIDTH / 4;
    int half_screen = SCREEN_WIDTH / 2;

    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    draw_text(Game.screen.renderer, "Configuration", SCREEN_WIDTH / 5, 30, 5);

    draw_text(Game.screen.renderer, "P1 UP:", quarter_screen, option_heights[0], 2);
    draw_text(Game.screen.renderer, (char*) player1_up, half_screen, option_heights[0], 2);
    draw_text(Game.screen.renderer, "P1 DOWN:", quarter_screen, option_heights[1], 2);
    draw_text(Game.screen.renderer, (char*) player1_down, half_screen, option_heights[1], 2);

    draw_text(Game.screen.renderer, "P2 UP:", quarter_screen, option_heights[2], 2);
    draw_text(Game.screen.renderer, (char *) player2_up, half_screen, option_heights[2], 2);
    draw_text(Game.screen.renderer, "P2 DOWN:", quarter_screen, option_heights[3], 2);
    draw_text(Game.screen.renderer, (char *) player2_down, half_screen, option_heights[3], 2);

    draw_text(Game.screen.renderer, "Go back to main menu (ESC)", quarter_screen, option_heights[4], 2);

    Game.cursor.x = quarter_screen - 40;
    Game.cursor.y = option_heights[option];

    SDL_RenderCopy(Game.screen.renderer, Game.textures.ball, NULL, &Game.cursor);

	if (setting_control == 1) {
		switch(option) {
		case SetP1Up:
			Game.player1.controls.up = get_pressed_key(Game.player1.controls.up);
			break;
		case SetP1Down:
			Game.player1.controls.up = get_pressed_key(Game.player1.controls.up);
			break;
		default:
			break;
		}
		setting_control = 0;
	} else if (keyboard_state[SDL_SCANCODE_RETURN]) {
		switch(option) {
		case SetP1Up:
			setting_control = 1;
			break;
		case QuitConfig:
		default:
			Game.state = Menu;
		}
        SDL_Delay(DEBOUNCE_WAIT);
	} else if (keyboard_state[SDL_SCANCODE_ESCAPE]) {
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

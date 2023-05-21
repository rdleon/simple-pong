#ifndef MENU_H
#define MENU_H

#include "config.h"
#include "game.h"
#include "fonts.h"

#include "SDL2/SDL.h"

enum menu_option {MenuStart, MenuControls, MenuQuit};

void menu_main_loop(const Uint8* keyboard_state);

#endif

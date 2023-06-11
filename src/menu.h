#ifndef MENU_H
#define MENU_H

#include "simplepong.h"

enum menu_option {MenuSinglePlayer, MenuTwoPlayers, MenuControls, MenuQuit};

void menu_main_loop(const Uint8* keyboard_state);

#endif

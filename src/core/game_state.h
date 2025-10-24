#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game_api.h"

void init_game_state();
GameState* get_game_state();
void cleanup_game_state();

#endif // GAME_STATE_H
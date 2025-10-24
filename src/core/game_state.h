#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game_api.h"

void init_game_state();
GameState* get_game_state();
GameState* get_core_game_state(); // For UI bridge
void update_game_state(float delta_time);
void cleanup_game_state();

// Game phase control functions
void set_game_phase(int phase);
int get_game_phase();
void quit_game();

#endif // GAME_STATE_H
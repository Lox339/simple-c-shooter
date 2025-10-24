#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "game_api.h"

void init_input_manager();
void process_input();
InputState* get_input_state();
void cleanup_input_manager();

#endif // INPUT_MANAGER_H
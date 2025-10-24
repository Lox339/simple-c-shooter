#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "game_api.h"

// Input Manager functions
void init_input_manager();
void process_input();
void cleanup_input_manager();

// Input handling functions
void handle_keyboard_input(int key, int action);
void handle_mouse_movement(float x_offset, float y_offset);
void handle_mouse_click(int button, int action);
void apply_input_to_player();

// Input query functions
int is_key_pressed(int key);
int is_mouse_button_pressed(int button);
InputState* get_input_state();

// Settings functions
void set_mouse_sensitivity(float sensitivity);
float get_mouse_sensitivity();

#endif // INPUT_MANAGER_H
#include "game_api.h"
#include "core/game_state.h"
#include "core/input_manager.h"
#include <stdio.h>

void init_game() {
    printf("Initializing game API...\n");
    init_game_state();
    init_input_manager();
}

void update_game(float delta_time) {
    GameState* state = get_game_state();
    state->delta_time = delta_time;
    // Game logic updates will be implemented later
}

GameState* get_game_state() {
    return get_game_state();
}

void handle_input(int key, int action) {
    handle_keyboard_input(key, action);
}

void handle_mouse(float x_offset, float y_offset) {
    handle_mouse_movement(x_offset, y_offset);
}

void handle_mouse_click(int button, int action) {
    handle_mouse_click(button, action);
}

void cleanup_game() {
    printf("Cleaning up game API...\n");
    cleanup_game_state();
    cleanup_input_manager();
}
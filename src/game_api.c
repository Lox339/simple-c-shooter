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
    // Input handling will be implemented later
    (void)key;    // Suppress unused parameter warning
    (void)action; // Suppress unused parameter warning
}

void handle_mouse(float x_offset, float y_offset) {
    // Mouse handling will be implemented later
    (void)x_offset; // Suppress unused parameter warning
    (void)y_offset; // Suppress unused parameter warning
}

void handle_mouse_click(int button, int action) {
    // Mouse click handling will be implemented later
    (void)button; // Suppress unused parameter warning
    (void)action; // Suppress unused parameter warning
}

void cleanup_game() {
    printf("Cleaning up game API...\n");
    cleanup_game_state();
    cleanup_input_manager();
}
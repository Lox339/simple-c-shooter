#include "game_state.h"
#include "game_api.h"
#include <stdio.h>
#include <string.h>

static GameState g_game_state;

void init_game_state() {
    memset(&g_game_state, 0, sizeof(GameState));
    g_game_state.game_running = 1;
    g_game_state.current_phase = GAME_MENU;
    printf("Game State initialized\n");
}

GameState* get_game_state() {
    return &g_game_state;
}

void cleanup_game_state() {
    printf("Game State cleaned up\n");
}
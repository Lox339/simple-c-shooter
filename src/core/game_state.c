#include "game_state.h"
#include "game_api.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static GameState g_game_state;

void init_game_state() {
    memset(&g_game_state, 0, sizeof(GameState));
    
    // Initialize player state
    g_game_state.player.position = (Vector3){0.0f, 0.0f, 0.0f};
    g_game_state.player.velocity = (Vector3){0.0f, 0.0f, 0.0f};
    g_game_state.player.rotation = (Vector3){0.0f, 0.0f, 0.0f};
    g_game_state.player.speed = 0.0f;
    g_game_state.player.max_speed = 10.0f;
    g_game_state.player.on_ground = 1;
    g_game_state.player.jump_time = 0.0f;
    g_game_state.player.health = 100;
    g_game_state.player.max_health = 100;
    g_game_state.player.ammo = 30;
    g_game_state.player.max_ammo = 30;
    g_game_state.player.horizontal_speed = 0.0f;
    g_game_state.player.last_jump_time = 0.0f;
    g_game_state.player.consecutive_jumps = 0;
    
    // Initialize game state
    g_game_state.score = 0;
    g_game_state.enemy_count = 0;
    g_game_state.projectile_count = 0;
    g_game_state.delta_time = 0.0f;
    g_game_state.game_running = 1;
    g_game_state.current_phase = GAME_MENU;
    
    printf("Game State initialized - Player health: %d, ammo: %d\n", 
           g_game_state.player.health, g_game_state.player.ammo);
}

GameState* get_game_state() {
    return &g_game_state;
}

void update_game_state(float delta_time) {
    g_game_state.delta_time = delta_time;
    
    // Player physics is now handled by BunnyHopController in the physics engine
    // Just update the speed calculations here for consistency
    float horizontal_velocity = sqrtf(g_game_state.player.velocity.x * g_game_state.player.velocity.x + 
                                     g_game_state.player.velocity.z * g_game_state.player.velocity.z);
    g_game_state.player.horizontal_speed = horizontal_velocity;
    
    // Total speed calculation
    g_game_state.player.speed = sqrtf(g_game_state.player.velocity.x * g_game_state.player.velocity.x + 
                                     g_game_state.player.velocity.y * g_game_state.player.velocity.y + 
                                     g_game_state.player.velocity.z * g_game_state.player.velocity.z);
}

GameState* get_core_game_state() {
    return &g_game_state;
}

void set_game_phase(int phase) {
    if (phase >= 0 && phase <= 3) {
        g_game_state.current_phase = (GamePhase)phase;
        printf("Game phase changed to: %d\n", phase);
    }
}

int get_game_phase() {
    return (int)g_game_state.current_phase;
}

void quit_game() {
    g_game_state.game_running = 0;
    printf("Game quit requested\n");
}

void cleanup_game_state() {
    printf("Game State cleaned up\n");
}
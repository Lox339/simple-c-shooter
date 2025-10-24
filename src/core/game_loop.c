// Core Engine - Game Loop Implementation
// This file contains the main game loop with FPS control

#include "game_loop.h"
#include "game_state.h"
#include "input_manager.h"
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

static GameLoop g_game_loop;

// Cross-platform time functions
static double get_current_time() {
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
#endif
}

static void sleep_ms(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void init_core_engine() {
    printf("Initializing Core Engine...\n");
    
    // Initialize game loop structure
    g_game_loop.last_time = get_current_time();
    g_game_loop.current_time = g_game_loop.last_time;
    g_game_loop.delta_time = 0.0;
    g_game_loop.target_fps = 60;
    
    // Initialize subsystems
    init_game_state();
    init_input_manager();
    
    printf("Core Engine initialized - Target FPS: %d\n", g_game_loop.target_fps);
}

void run_game_loop() {
    printf("Starting game loop...\n");
    printf("Press Q to quit, ESC to pause/resume, WASD to move, SPACE to jump\n");
    
    GameState* game_state = get_game_state();
    
    // Start in playing mode for input testing
    game_state->current_phase = GAME_PLAYING;
    
    double target_frame_time = 1.0 / g_game_loop.target_fps;
    int frame_count = 0;
    double fps_timer = 0.0;
    double status_timer = 0.0;
    
    while (game_state->game_running) {
        // Calculate delta time
        g_game_loop.current_time = get_current_time();
        g_game_loop.delta_time = g_game_loop.current_time - g_game_loop.last_time;
        g_game_loop.last_time = g_game_loop.current_time;
        
        // Cap delta time to prevent large jumps
        if (g_game_loop.delta_time > 0.05) {
            g_game_loop.delta_time = 0.05;
        }
        
        // Update game logic
        update_game_logic((float)g_game_loop.delta_time);
        
        // FPS counter
        frame_count++;
        fps_timer += g_game_loop.delta_time;
        status_timer += g_game_loop.delta_time;
        
        if (fps_timer >= 1.0) {
            printf("FPS: %d, Delta: %.3fms, Phase: %s\n", 
                   frame_count, g_game_loop.delta_time * 1000.0,
                   game_state->current_phase == GAME_PLAYING ? "PLAYING" :
                   game_state->current_phase == GAME_PAUSED ? "PAUSED" : "OTHER");
            frame_count = 0;
            fps_timer = 0.0;
        }
        
        // Player status every 3 seconds
        if (status_timer >= 3.0) {
            PlayerState* player = &game_state->player;
            printf("Player Status - Pos(%.2f,%.2f,%.2f) Rot(%.1f,%.1f,%.1f) Speed:%.2f Ground:%s\n",
                   player->position.x, player->position.y, player->position.z,
                   player->rotation.x, player->rotation.y, player->rotation.z,
                   player->speed, player->on_ground ? "YES" : "NO");
            status_timer = 0.0;
        }
        
        // Frame rate limiting
        double frame_end_time = get_current_time();
        double frame_duration = frame_end_time - g_game_loop.current_time;
        
        if (frame_duration < target_frame_time) {
            int sleep_time = (int)((target_frame_time - frame_duration) * 1000.0);
            if (sleep_time > 0) {
                sleep_ms(sleep_time);
            }
        }
    }
    
    printf("Game loop ended\n");
}

void update_game_logic(float delta_time) {
    GameState* game_state = get_game_state();
    
    // Update game state
    update_game_state(delta_time);
    
    // Process input (placeholder - will be implemented in input manager)
    process_input();
    
    // Update game phase logic
    switch (game_state->current_phase) {
        case GAME_MENU:
            // Menu logic will be implemented later
            break;
            
        case GAME_PLAYING:
            // Game playing logic
            update_gameplay(delta_time);
            break;
            
        case GAME_PAUSED:
            // Pause logic - don't update gameplay
            break;
            
        case GAME_OVER:
            // Game over logic
            break;
    }
}

void update_gameplay(float delta_time) {
    GameState* game_state = get_game_state();
    
    // Update enemies (placeholder)
    for (int i = 0; i < game_state->enemy_count; i++) {
        // Enemy update logic will be implemented later
    }
    
    // Update projectiles (placeholder)
    for (int i = 0; i < game_state->projectile_count; i++) {
        // Projectile update logic will be implemented later
    }
    
    // Gameplay logic is now handled by input system
    // Player movement is controlled by WASD keys
}

void set_target_fps(int fps) {
    if (fps > 0 && fps <= 300) {
        g_game_loop.target_fps = fps;
        printf("Target FPS set to: %d\n", fps);
    }
}

int get_target_fps() {
    return g_game_loop.target_fps;
}

double get_delta_time() {
    return g_game_loop.delta_time;
}

void cleanup_core() {
    printf("Cleaning up Core Engine...\n");
    cleanup_input_manager();
    cleanup_game_state();
    printf("Core Engine cleaned up\n");
}
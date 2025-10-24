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
    
    GameState* game_state = get_game_state();
    double target_frame_time = 1.0 / g_game_loop.target_fps;
    int frame_count = 0;
    double fps_timer = 0.0;
    
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
        
        if (fps_timer >= 1.0) {
            printf("FPS: %d, Delta: %.3fms\n", frame_count, g_game_loop.delta_time * 1000.0);
            frame_count = 0;
            fps_timer = 0.0;
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
        
        // Simple exit condition for testing (remove when input system is complete)
        static int test_frames = 0;
        test_frames++;
        if (test_frames > 300) { // Run for ~5 seconds at 60 FPS
            game_state->game_running = 0;
            printf("Test run completed after %d frames\n", test_frames);
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
    
    // Simple test: move player forward slowly
    if (game_state->current_phase == GAME_PLAYING) {
        game_state->player.velocity.x = 1.0f; // Move forward for testing
    }
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
// Core Engine - Game Loop Implementation
// This file contains the main game loop with FPS control

#include "game_loop.h"
#include "game_state.h"
#include "input_manager.h"
#include "object_manager.h"
#include "../graphics_bridge.h"
#include "../physics_bridge.h"
#include "../ui_bridge.h"
#include "../audio_bridge.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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
    init_object_manager();
    
    // Initialize graphics engine
    if (!init_graphics_engine()) {
        printf("Warning: Graphics engine initialization failed - running in text mode\n");
    }
    
    // Initialize physics engine
    if (!init_physics_engine()) {
        printf("Warning: Physics engine initialization failed - using basic physics\n");
    } else {
        // Configure bunny hop parameters
        printf("Configuring bunny hop mechanics...\n");
        
        // Set balanced parameters for gameplay
        set_bunny_hop_max_ground_speed(12.0f);  // Slightly higher ground speed
        set_bunny_hop_max_air_speed(25.0f);     // Reasonable air speed limit
        
        printf("Ground speed limit: %.1f u/s\n", get_bunny_hop_max_ground_speed());
        printf("Air speed limit: %.1f u/s\n", get_bunny_hop_max_air_speed());
        printf("Bunny hop mechanics ready!\n");
    }
    
    // Initialize UI manager
    if (!init_ui_manager()) {
        printf("Warning: UI Manager initialization failed - using console output\n");
    }
    
    // Initialize audio system
    if (!init_audio_bridge()) {
        printf("Warning: Audio system initialization failed - running without sound\n");
    }
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    printf("Core Engine initialized - Target FPS: %d\n", g_game_loop.target_fps);
}

void run_game_loop() {
    printf("Starting game loop...\n");
    printf("Press Q to quit, ESC to pause/resume, WASD to move, SPACE to jump\n");
    
    GameState* game_state = get_game_state();
    
    // Start in menu mode
    game_state->current_phase = GAME_MENU;
    
    // Start menu music
    start_menu_music();
    
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
        
        // Update physics
        update_physics((float)g_game_loop.delta_time);
        
        // Update UI
        update_ui_manager((float)g_game_loop.delta_time);
        
        // Update audio system
        update_audio_system(game_state, (float)g_game_loop.delta_time);
        
        // Render frame
        render_game_frame(game_state);
        
        // Render UI overlay
        render_ui_manager();
        
        // Check if graphics window should close
        if (graphics_should_close()) {
            game_state->game_running = 0;
            printf("Graphics window closed\n");
        }
        
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
        
        // Game status every 3 seconds
        if (status_timer >= 3.0) {
            PlayerState* player = &game_state->player;
            printf("=== GAME STATUS ===\n");
            printf("Player: Pos(%.2f,%.2f,%.2f) Health:%d/%d\n",
                   player->position.x, player->position.y, player->position.z,
                   player->health, player->max_health);
            
            // Speedometer display (right corner simulation)
            printf("                                                    [SPEED: %.1f u/s]\n", player->speed);
            if (player->speed > get_bunny_hop_max_ground_speed()) {
                printf("                                                    [BUNNY HOP ACTIVE!]\n");
            }
            
            printf("Ground:%s Jumps:%d Enemies:%d Projectiles:%d Score:%d\n",
                   player->on_ground ? "YES" : "NO", player->consecutive_jumps,
                   game_state->enemy_count, game_state->projectile_count, game_state->score);
            printf("==================\n");
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
    static GamePhase last_phase = GAME_MENU;
    if (game_state->current_phase != last_phase) {
        // Phase changed, update music
        switch (game_state->current_phase) {
            case GAME_MENU:
                start_menu_music();
                break;
            case GAME_PLAYING:
                start_background_music();
                break;
            case GAME_PAUSED:
                // Keep current music but could lower volume
                break;
            case GAME_OVER:
                stop_current_music();
                break;
        }
        last_phase = game_state->current_phase;
    }
    
    switch (game_state->current_phase) {
        case GAME_MENU:
            // Menu logic is handled by UI system
            break;
            
        case GAME_PLAYING:
            // Game playing logic
            update_gameplay(delta_time);
            break;
            
        case GAME_PAUSED:
            // Pause logic - don't update gameplay
            break;
            
        case GAME_OVER:
            // Game over logic - stop the game after showing final score
            static float game_over_timer = 0.0f;
            game_over_timer += delta_time;
            if (game_over_timer > 3.0f) { // Show game over for 3 seconds
                game_state->current_phase = GAME_MENU;
                game_over_timer = 0.0f;
            }
            break;
    }
}

void update_gameplay(float delta_time) {
    GameState* game_state = get_game_state();
    
    // Update game objects
    update_enemies(delta_time);
    update_projectiles(delta_time);
    
    // Spawn enemies periodically
    spawn_enemies_periodically(delta_time);
    
    // Check game over condition
    if (game_state->player.health <= 0) {
        game_state->current_phase = GAME_OVER;
        printf("GAME OVER! Final Score: %d\n", game_state->score);
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
    cleanup_audio_bridge();
    cleanup_ui_manager();
    cleanup_physics_engine();
    cleanup_graphics_engine();
    cleanup_object_manager();
    cleanup_input_manager();
    cleanup_game_state();
    printf("Core Engine cleaned up\n");
}
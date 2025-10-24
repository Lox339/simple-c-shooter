#include "input_manager.h"
#include "game_state.h"
#include "object_manager.h"
#include "../physics_bridge.h"
#include "../audio_bridge.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

static InputState g_input_state;
static int g_input_initialized = 0;

// Key codes for cross-platform compatibility
#define KEY_W 'w'
#define KEY_A 'a'
#define KEY_S 's'
#define KEY_D 'd'
#define KEY_SPACE ' '
#define KEY_ESC 27
#define KEY_Q 'q'

// Cross-platform keyboard input functions
#ifdef _WIN32
static int kbhit() {
    return _kbhit();
}

static int getch_nb() {
    if (_kbhit()) {
        return _getch();
    }
    return 0;
}
#else
static struct termios old_terminal;
static int terminal_configured = 0;

static void configure_terminal() {
    if (!terminal_configured) {
        struct termios new_terminal;
        tcgetattr(STDIN_FILENO, &old_terminal);
        new_terminal = old_terminal;
        new_terminal.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        terminal_configured = 1;
    }
}

static void restore_terminal() {
    if (terminal_configured) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal);
        terminal_configured = 0;
    }
}

static int kbhit() {
    configure_terminal();
    int ch = getchar();
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

static int getch_nb() {
    configure_terminal();
    return getchar();
}
#endif

void init_input_manager() {
    memset(&g_input_state, 0, sizeof(InputState));
    g_input_state.mouse_sensitivity = 2.0f;
    g_input_state.mouse_x = 0.0f;
    g_input_state.mouse_y = 0.0f;
    g_input_initialized = 1;
    
#ifndef _WIN32
    configure_terminal();
#endif
    
    printf("Input Manager initialized - Mouse sensitivity: %.1f\n", g_input_state.mouse_sensitivity);
    printf("Controls: WASD - Move, SPACE - Jump, Q - Quit, ESC - Pause\n");
}

void process_input() {
    if (!g_input_initialized) return;
    
    GameState* game_state = get_game_state();
    if (!game_state) return;
    
    // Reset frame-specific input
    g_input_state.jump_pressed = 0;
    g_input_state.mouse_delta_x = 0.0f;
    g_input_state.mouse_delta_y = 0.0f;
    
    // Process keyboard input
    while (kbhit()) {
        int key = getch_nb();
        if (key > 0) {
            handle_keyboard_input(key, 1); // Key pressed
        }
    }
    
    // Simulate mouse movement for testing (in real implementation, this would come from OS)
    static float test_mouse_time = 0.0f;
    test_mouse_time += game_state->delta_time;
    
    // Simulate slow mouse movement for camera rotation testing
    float mouse_speed = 0.5f;
    g_input_state.mouse_delta_x = sinf(test_mouse_time * mouse_speed) * 0.1f;
    g_input_state.mouse_delta_y = cosf(test_mouse_time * mouse_speed * 0.7f) * 0.05f;
    
    // Simulate mouse clicks for shooting every 3 seconds
    static float shoot_timer = 0.0f;
    shoot_timer += game_state->delta_time;
    if (shoot_timer >= 3.0f && game_state->current_phase == GAME_PLAYING) {
        handle_mouse_click(0, 1); // Left mouse button click
        shoot_timer = 0.0f;
    }
    
    // Update mouse position
    g_input_state.mouse_x += g_input_state.mouse_delta_x;
    g_input_state.mouse_y += g_input_state.mouse_delta_y;
    
    // Apply input to player movement
    apply_input_to_player();
}

void handle_keyboard_input(int key, int action) {
    // Convert to lowercase for consistency
    if (key >= 'A' && key <= 'Z') {
        key = key - 'A' + 'a';
    }
    
    // Store key state
    if (key >= 0 && key < 512) {
        g_input_state.keys[key] = action;
    }
    
    // Handle special keys
    switch (key) {
        case KEY_SPACE:
            if (action) {
                g_input_state.jump_pressed = 1;
                printf("Jump pressed!\n");
            }
            break;
            
        case KEY_ESC:
            if (action) {
                GameState* game_state = get_game_state();
                if (game_state->current_phase == GAME_PLAYING) {
                    game_state->current_phase = GAME_PAUSED;
                    printf("Game paused\n");
                } else if (game_state->current_phase == GAME_PAUSED) {
                    game_state->current_phase = GAME_PLAYING;
                    printf("Game resumed\n");
                }
            }
            break;
            
        case KEY_Q:
            if (action) {
                GameState* game_state = get_game_state();
                game_state->game_running = 0;
                printf("Quit requested\n");
            }
            break;
            
        case 'o': // Open audio settings
            if (action) {
                printf("Audio settings toggled (press 1-6 to adjust volumes)\n");
                printf("1/2: Master Volume +/-, 3/4: SFX Volume +/-, 5/6: Music Volume +/-\n");
            }
            break;
            
        case KEY_W:
        case KEY_A:
        case KEY_S:
        case KEY_D:
            // Movement keys are handled in apply_input_to_player
            break;
    }
}

void handle_mouse_movement(float x_offset, float y_offset) {
    g_input_state.mouse_delta_x = x_offset * g_input_state.mouse_sensitivity;
    g_input_state.mouse_delta_y = y_offset * g_input_state.mouse_sensitivity;
    
    g_input_state.mouse_x += g_input_state.mouse_delta_x;
    g_input_state.mouse_y += g_input_state.mouse_delta_y;
    
    // Clamp vertical mouse movement
    if (g_input_state.mouse_y > 89.0f) g_input_state.mouse_y = 89.0f;
    if (g_input_state.mouse_y < -89.0f) g_input_state.mouse_y = -89.0f;
}

void handle_mouse_click(int button, int action) {
    if (button >= 0 && button < 8) {
        g_input_state.mouse_buttons[button] = action;
        
        if (button == 0 && action) { // Left mouse button
            fire_weapon();
        }
    }
}

void apply_input_to_player() {
    GameState* game_state = get_game_state();
    if (!game_state || game_state->current_phase != GAME_PLAYING) {
        return;
    }
    
    PlayerState* player = &game_state->player;
    float delta_time = game_state->delta_time;
    
    // Apply mouse movement to camera rotation first
    player->rotation.y += g_input_state.mouse_delta_x * 0.1f; // Yaw
    player->rotation.x += g_input_state.mouse_delta_y * 0.1f; // Pitch
    
    // Clamp pitch rotation
    if (player->rotation.x > 89.0f) player->rotation.x = 89.0f;
    if (player->rotation.x < -89.0f) player->rotation.x = -89.0f;
    
    // Wrap yaw rotation
    while (player->rotation.y > 360.0f) player->rotation.y -= 360.0f;
    while (player->rotation.y < 0.0f) player->rotation.y += 360.0f;
    
    // Use bunny hop physics for movement
    apply_bunny_hop_movement(player, &g_input_state, delta_time);
    
    // Debug output for speed tracking
    static float debug_timer = 0.0f;
    debug_timer += delta_time;
    if (debug_timer >= 1.0f) { // Every second
        if (player->speed > 12.0f) { // Only show when moving fast
            printf("Bunny Hop Status - Speed: %.1f u/s, Ground: %s, Jumps: %d\n",
                   player->speed, player->on_ground ? "YES" : "NO", player->consecutive_jumps);
        }
        debug_timer = 0.0f;
    }
}

int is_key_pressed(int key) {
    if (key >= 0 && key < 512) {
        return g_input_state.keys[key];
    }
    return 0;
}

int is_mouse_button_pressed(int button) {
    if (button >= 0 && button < 8) {
        return g_input_state.mouse_buttons[button];
    }
    return 0;
}

void set_mouse_sensitivity(float sensitivity) {
    if (sensitivity > 0.0f && sensitivity <= 10.0f) {
        g_input_state.mouse_sensitivity = sensitivity;
        printf("Mouse sensitivity set to: %.2f\n", sensitivity);
    }
}

float get_mouse_sensitivity() {
    return g_input_state.mouse_sensitivity;
}

InputState* get_input_state() {
    return &g_input_state;
}

void fire_weapon() {
    GameState* game_state = get_game_state();
    if (!game_state || game_state->current_phase != GAME_PLAYING) {
        return;
    }
    
    PlayerState* player = &game_state->player;
    
    // Check if player has ammo
    if (player->ammo <= 0) {
        printf("No ammo! Reload needed.\n");
        return;
    }
    
    // Calculate projectile spawn position (slightly in front of player at eye level)
    float yaw = player->rotation.y * M_PI / 180.0f;
    float pitch = player->rotation.x * M_PI / 180.0f;
    
    // Forward vector considering both yaw and pitch
    Vector3 forward = {
        sinf(yaw) * cosf(pitch),
        -sinf(pitch),
        cosf(yaw) * cosf(pitch)
    };
    
    // Spawn position at eye level (1.6m above ground) and 0.5m forward
    Vector3 spawn_pos = {
        player->position.x + forward.x * 0.5f,
        player->position.y + 1.6f + forward.y * 0.5f,
        player->position.z + forward.z * 0.5f
    };
    
    // Calculate projectile velocity with high speed (50 units/sec)
    float projectile_speed = 50.0f;
    Vector3 velocity = {
        forward.x * projectile_speed,
        forward.y * projectile_speed,
        forward.z * projectile_speed
    };
    
    // Create projectile
    int projectile_id = create_projectile(PROJECTILE_PLAYER_BULLET, spawn_pos, velocity, 0);
    
    if (projectile_id >= 0) {
        // Play shooting sound
        play_player_shoot_sound();
        
        // Consume ammo
        player->ammo--;
        printf("FIRE! Direction: (%.2f, %.2f, %.2f) Ammo: %d/%d\n", 
               forward.x, forward.y, forward.z, player->ammo, player->max_ammo);
        
        // Auto-reload when empty
        if (player->ammo == 0) {
            play_reload_sound();
            player->ammo = player->max_ammo;
            printf("Auto-reload! Ammo: %d/%d\n", player->ammo, player->max_ammo);
        }
    }
}

void cleanup_input_manager() {
#ifndef _WIN32
    restore_terminal();
#endif
    g_input_initialized = 0;
    printf("Input Manager cleaned up\n");
}
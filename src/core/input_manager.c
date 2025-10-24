#include "input_manager.h"
#include "game_state.h"
#include "object_manager.h"
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
    float move_speed = 5.0f;
    float delta_time = game_state->delta_time;
    
    // Calculate movement direction based on camera rotation
    float yaw = player->rotation.y * M_PI / 180.0f; // Convert to radians
    
    Vector3 forward = {sinf(yaw), 0.0f, cosf(yaw)};
    Vector3 right = {cosf(yaw), 0.0f, -sinf(yaw)};
    
    Vector3 movement = {0.0f, 0.0f, 0.0f};
    
    // WASD movement
    if (g_input_state.keys[KEY_W]) {
        movement.x += forward.x;
        movement.z += forward.z;
    }
    if (g_input_state.keys[KEY_S]) {
        movement.x -= forward.x;
        movement.z -= forward.z;
    }
    if (g_input_state.keys[KEY_A]) {
        movement.x -= right.x;
        movement.z -= right.z;
    }
    if (g_input_state.keys[KEY_D]) {
        movement.x += right.x;
        movement.z += right.z;
    }
    
    // Normalize movement vector
    float movement_length = sqrtf(movement.x * movement.x + movement.z * movement.z);
    if (movement_length > 0.0f) {
        movement.x = (movement.x / movement_length) * move_speed;
        movement.z = (movement.z / movement_length) * move_speed;
        
        // Apply movement to player velocity
        player->velocity.x = movement.x;
        player->velocity.z = movement.z;
        
        printf("Player moving: velocity(%.2f, %.2f, %.2f) speed: %.2f\n", 
               player->velocity.x, player->velocity.y, player->velocity.z, player->speed);
    } else {
        // Stop horizontal movement when no keys pressed
        player->velocity.x = 0.0f;
        player->velocity.z = 0.0f;
    }
    
    // Handle jumping
    if (g_input_state.jump_pressed && player->on_ground) {
        player->velocity.y = 8.0f; // Jump velocity
        player->on_ground = 0;
        player->last_jump_time = 0.0f; // Reset for bunny hop mechanics
        printf("Player jumped! Velocity Y: %.2f\n", player->velocity.y);
    }
    
    // Apply mouse movement to camera rotation
    player->rotation.y += g_input_state.mouse_delta_x * 0.1f; // Yaw
    player->rotation.x += g_input_state.mouse_delta_y * 0.1f; // Pitch
    
    // Clamp pitch rotation
    if (player->rotation.x > 89.0f) player->rotation.x = 89.0f;
    if (player->rotation.x < -89.0f) player->rotation.x = -89.0f;
    
    // Wrap yaw rotation
    while (player->rotation.y > 360.0f) player->rotation.y -= 360.0f;
    while (player->rotation.y < 0.0f) player->rotation.y += 360.0f;
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
    
    // Calculate projectile spawn position (slightly in front of player)
    float yaw = player->rotation.y * M_PI / 180.0f;
    Vector3 forward = {sinf(yaw), 0.0f, cosf(yaw)};
    
    Vector3 spawn_pos = {
        player->position.x + forward.x * 1.0f,
        player->position.y + 1.0f, // Shoot from chest height
        player->position.z + forward.z * 1.0f
    };
    
    // Calculate projectile velocity (forward direction with some upward angle)
    float pitch = player->rotation.x * M_PI / 180.0f;
    Vector3 velocity = {
        forward.x * 20.0f, // Fast horizontal speed
        -sinf(pitch) * 20.0f, // Vertical component based on pitch
        forward.z * 20.0f
    };
    
    // Create projectile
    int projectile_id = create_projectile(PROJECTILE_PLAYER_BULLET, spawn_pos, velocity, 0);
    
    if (projectile_id >= 0) {
        // Consume ammo
        player->ammo--;
        printf("FIRE! Ammo: %d/%d\n", player->ammo, player->max_ammo);
        
        // Auto-reload when empty
        if (player->ammo == 0) {
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
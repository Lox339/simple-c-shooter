#include "audio_bridge.h"
#include "audio/sound_generator.h"
#include <stdio.h>
#include <math.h>

// Audio bridge state
static int g_audio_bridge_initialized = 0;
static float g_footstep_timer = 0.0f;
static int g_last_on_ground = 1;
static float g_last_player_speed = 0.0f;

int init_audio_bridge() {
    printf("Initializing Audio Bridge...\n");
    
    if (!init_audio_system()) {
        printf("Failed to initialize audio system\n");
        return 0;
    }
    
    // Generate placeholder sound files
    generate_placeholder_sounds();
    
    // Load default sounds (in a real implementation, these would be actual files)
    load_sound("sounds/player_shoot.wav", SOUND_PLAYER_SHOOT);
    load_sound("sounds/enemy_shoot.wav", SOUND_ENEMY_SHOOT);
    load_sound("sounds/explosion.wav", SOUND_EXPLOSION);
    load_sound("sounds/enemy_hit.wav", SOUND_ENEMY_HIT);
    load_sound("sounds/player_hit.wav", SOUND_PLAYER_HIT);
    load_sound("sounds/enemy_death.wav", SOUND_ENEMY_DEATH);
    load_sound("sounds/footstep.wav", SOUND_FOOTSTEP);
    load_sound("sounds/jump.wav", SOUND_JUMP);
    load_sound("sounds/land.wav", SOUND_LAND);
    load_sound("sounds/bunny_hop.wav", SOUND_BUNNY_HOP);
    load_sound("sounds/reload.wav", SOUND_RELOAD);
    load_sound("sounds/pickup.wav", SOUND_PICKUP);
    load_sound("music/background.ogg", SOUND_BACKGROUND_MUSIC);
    load_sound("music/menu.ogg", SOUND_MENU_MUSIC);
    
    g_audio_bridge_initialized = 1;
    printf("Audio Bridge initialized successfully\n");
    return 1;
}

void cleanup_audio_bridge() {
    printf("Cleaning up Audio Bridge...\n");
    
    if (g_audio_bridge_initialized) {
        cleanup_audio_system();
        g_audio_bridge_initialized = 0;
    }
    
    printf("Audio Bridge cleaned up\n");
}

// Game event audio functions
void play_player_shoot_sound() {
    if (!g_audio_bridge_initialized) return;
    
    // Add slight pitch variation for variety
    float pitch = 0.9f + ((float)rand() / RAND_MAX) * 0.2f; // 0.9 to 1.1
    play_sound_2d(SOUND_PLAYER_SHOOT, 0.8f, pitch);
}

void play_enemy_shoot_sound(Vector3 position) {
    if (!g_audio_bridge_initialized) return;
    
    float pitch = 0.8f + ((float)rand() / RAND_MAX) * 0.3f; // 0.8 to 1.1
    play_sound_3d(SOUND_ENEMY_SHOOT, position, 0.7f, pitch);
}

void play_explosion_sound(Vector3 position) {
    if (!g_audio_bridge_initialized) return;
    
    play_sound_3d(SOUND_EXPLOSION, position, 1.0f, 1.0f);
}

void play_enemy_hit_sound(Vector3 position) {
    if (!g_audio_bridge_initialized) return;
    
    float pitch = 0.9f + ((float)rand() / RAND_MAX) * 0.2f;
    play_sound_3d(SOUND_ENEMY_HIT, position, 0.6f, pitch);
}

void play_player_hit_sound() {
    if (!g_audio_bridge_initialized) return;
    
    play_sound_2d(SOUND_PLAYER_HIT, 0.8f, 1.0f);
}

void play_enemy_death_sound(Vector3 position) {
    if (!g_audio_bridge_initialized) return;
    
    play_sound_3d(SOUND_ENEMY_DEATH, position, 0.7f, 1.0f);
}

// Movement audio functions
void play_footstep_sound() {
    if (!g_audio_bridge_initialized) return;
    
    // Add variation to footsteps
    float pitch = 0.8f + ((float)rand() / RAND_MAX) * 0.4f; // 0.8 to 1.2
    float volume = 0.3f + ((float)rand() / RAND_MAX) * 0.2f; // 0.3 to 0.5
    play_sound_2d(SOUND_FOOTSTEP, volume, pitch);
}

void play_jump_sound() {
    if (!g_audio_bridge_initialized) return;
    
    play_sound_2d(SOUND_JUMP, 0.5f, 1.0f);
}

void play_land_sound() {
    if (!g_audio_bridge_initialized) return;
    
    // Landing volume based on fall impact (simplified)
    float volume = 0.4f + ((float)rand() / RAND_MAX) * 0.3f; // 0.4 to 0.7
    play_sound_2d(SOUND_LAND, volume, 1.0f);
}

void play_bunny_hop_sound() {
    if (!g_audio_bridge_initialized) return;
    
    // Higher pitch for bunny hop excitement
    float pitch = 1.1f + ((float)rand() / RAND_MAX) * 0.3f; // 1.1 to 1.4
    play_sound_2d(SOUND_BUNNY_HOP, 0.6f, pitch);
}

// UI audio functions
void play_reload_sound() {
    if (!g_audio_bridge_initialized) return;
    
    play_sound_2d(SOUND_RELOAD, 0.6f, 1.0f);
}

void play_pickup_sound() {
    if (!g_audio_bridge_initialized) return;
    
    play_sound_2d(SOUND_PICKUP, 0.5f, 1.0f);
}

// Music functions
void start_background_music() {
    if (!g_audio_bridge_initialized) return;
    
    stop_current_music(); // Stop any current music
    play_music(SOUND_BACKGROUND_MUSIC, 0.6f, 1); // Loop background music
}

void start_menu_music() {
    if (!g_audio_bridge_initialized) return;
    
    stop_current_music(); // Stop any current music
    play_music(SOUND_MENU_MUSIC, 0.5f, 1); // Loop menu music
}

void stop_current_music() {
    if (!g_audio_bridge_initialized) return;
    
    stop_music();
}

// Audio system integration
void update_audio_system(const GameState* game_state, float delta_time) {
    if (!g_audio_bridge_initialized || !game_state) return;
    
    // Update listener position based on player
    set_audio_listener_from_player(&game_state->player);
    
    // Update audio sources
    update_audio_sources(delta_time);
    
    // Handle automatic footstep sounds
    if (game_state->player.on_ground && game_state->player.speed > 1.0f) {
        g_footstep_timer += delta_time;
        
        // Footstep frequency based on speed
        float footstep_interval = 0.5f / (game_state->player.speed / 5.0f);
        footstep_interval = fmaxf(0.2f, fminf(0.8f, footstep_interval));
        
        if (g_footstep_timer >= footstep_interval) {
            play_footstep_sound();
            g_footstep_timer = 0.0f;
        }
    } else {
        g_footstep_timer = 0.0f;
    }
    
    // Handle landing sounds
    if (!g_last_on_ground && game_state->player.on_ground) {
        // Player just landed
        if (g_last_player_speed > 15.0f) {
            // High speed landing - bunny hop sound
            play_bunny_hop_sound();
        } else {
            // Normal landing
            play_land_sound();
        }
    }
    
    // Handle jump sounds
    if (g_last_on_ground && !game_state->player.on_ground) {
        // Player just jumped
        play_jump_sound();
    }
    
    // Update state for next frame
    g_last_on_ground = game_state->player.on_ground;
    g_last_player_speed = game_state->player.speed;
}

void set_audio_listener_from_player(const PlayerState* player) {
    if (!g_audio_bridge_initialized || !player) return;
    
    // Set listener position
    set_listener_position(player->position);
    
    // Calculate forward direction from player rotation
    float yaw = player->rotation.y * M_PI / 180.0f; // Convert to radians
    float pitch = player->rotation.x * M_PI / 180.0f;
    
    Vector3 forward = {
        cosf(pitch) * sinf(yaw),
        -sinf(pitch),
        cosf(pitch) * cosf(yaw)
    };
    
    Vector3 up = {0.0f, 1.0f, 0.0f};
    
    set_listener_orientation(forward, up);
    set_listener_velocity(player->velocity);
}

// Volume control functions (for UI)
void audio_set_master_volume(float volume) {
    if (!g_audio_bridge_initialized) return;
    set_master_volume(volume);
}

void audio_set_sfx_volume(float volume) {
    if (!g_audio_bridge_initialized) return;
    set_sfx_volume(volume);
}

void audio_set_music_volume(float volume) {
    if (!g_audio_bridge_initialized) return;
    set_music_volume_setting(volume);
}

float audio_get_master_volume() {
    if (!g_audio_bridge_initialized) return 1.0f;
    return get_audio_settings()->master_volume;
}

float audio_get_sfx_volume() {
    if (!g_audio_bridge_initialized) return 1.0f;
    return get_audio_settings()->sfx_volume;
}

float audio_get_music_volume() {
    if (!g_audio_bridge_initialized) return 1.0f;
    return get_audio_settings()->music_volume;
}
#include "audio_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Audio system state
static AudioSettings g_audio_settings = {
    .master_volume = 1.0f,
    .sfx_volume = 1.0f,
    .music_volume = 0.7f,
    .voice_volume = 1.0f,
    .audio_enabled = 1,
    .max_sources = 32,
    .current_sources = 0
};

static Vector3 g_listener_position = {0.0f, 0.0f, 0.0f};
static Vector3 g_listener_forward = {0.0f, 0.0f, -1.0f};
static Vector3 g_listener_up = {0.0f, 1.0f, 0.0f};
static Vector3 g_listener_velocity = {0.0f, 0.0f, 0.0f};

// Sound data storage (simplified - in real implementation would load actual audio files)
typedef struct {
    char filename[256];
    SoundType type;
    AudioType audio_type;
    float default_volume;
    float default_pitch;
    int loaded;
} SoundData;

static SoundData g_sounds[MAX_SOUND_TYPES];
static AudioSource g_audio_sources[32]; // Max 32 concurrent sounds
static int g_next_source_id = 1;
static int g_current_music_source = -1;

// Internal functions
static int find_free_source();
static void cleanup_finished_sources();
static float apply_volume_settings(float base_volume, AudioType type);

int init_audio_system() {
    printf("Initializing Audio System...\n");
    
    // Initialize sound data
    memset(g_sounds, 0, sizeof(g_sounds));
    memset(g_audio_sources, 0, sizeof(g_audio_sources));
    
    // Setup default sound configurations
    // Player sounds
    strcpy(g_sounds[SOUND_PLAYER_SHOOT].filename, "sounds/player_shoot.wav");
    g_sounds[SOUND_PLAYER_SHOOT].type = SOUND_PLAYER_SHOOT;
    g_sounds[SOUND_PLAYER_SHOOT].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_PLAYER_SHOOT].default_volume = 0.8f;
    g_sounds[SOUND_PLAYER_SHOOT].default_pitch = 1.0f;
    
    // Enemy sounds
    strcpy(g_sounds[SOUND_ENEMY_SHOOT].filename, "sounds/enemy_shoot.wav");
    g_sounds[SOUND_ENEMY_SHOOT].type = SOUND_ENEMY_SHOOT;
    g_sounds[SOUND_ENEMY_SHOOT].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_ENEMY_SHOOT].default_volume = 0.7f;
    g_sounds[SOUND_ENEMY_SHOOT].default_pitch = 0.9f;
    
    // Impact sounds
    strcpy(g_sounds[SOUND_EXPLOSION].filename, "sounds/explosion.wav");
    g_sounds[SOUND_EXPLOSION].type = SOUND_EXPLOSION;
    g_sounds[SOUND_EXPLOSION].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_EXPLOSION].default_volume = 1.0f;
    g_sounds[SOUND_EXPLOSION].default_pitch = 1.0f;
    
    strcpy(g_sounds[SOUND_ENEMY_HIT].filename, "sounds/enemy_hit.wav");
    g_sounds[SOUND_ENEMY_HIT].type = SOUND_ENEMY_HIT;
    g_sounds[SOUND_ENEMY_HIT].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_ENEMY_HIT].default_volume = 0.6f;
    g_sounds[SOUND_ENEMY_HIT].default_pitch = 1.0f;
    
    strcpy(g_sounds[SOUND_PLAYER_HIT].filename, "sounds/player_hit.wav");
    g_sounds[SOUND_PLAYER_HIT].type = SOUND_PLAYER_HIT;
    g_sounds[SOUND_PLAYER_HIT].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_PLAYER_HIT].default_volume = 0.8f;
    g_sounds[SOUND_PLAYER_HIT].default_pitch = 1.0f;
    
    strcpy(g_sounds[SOUND_ENEMY_DEATH].filename, "sounds/enemy_death.wav");
    g_sounds[SOUND_ENEMY_DEATH].type = SOUND_ENEMY_DEATH;
    g_sounds[SOUND_ENEMY_DEATH].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_ENEMY_DEATH].default_volume = 0.7f;
    g_sounds[SOUND_ENEMY_DEATH].default_pitch = 1.0f;
    
    // Movement sounds
    strcpy(g_sounds[SOUND_FOOTSTEP].filename, "sounds/footstep.wav");
    g_sounds[SOUND_FOOTSTEP].type = SOUND_FOOTSTEP;
    g_sounds[SOUND_FOOTSTEP].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_FOOTSTEP].default_volume = 0.4f;
    g_sounds[SOUND_FOOTSTEP].default_pitch = 1.0f;
    
    strcpy(g_sounds[SOUND_JUMP].filename, "sounds/jump.wav");
    g_sounds[SOUND_JUMP].type = SOUND_JUMP;
    g_sounds[SOUND_JUMP].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_JUMP].default_volume = 0.5f;
    g_sounds[SOUND_JUMP].default_pitch = 1.0f;
    
    strcpy(g_sounds[SOUND_LAND].filename, "sounds/land.wav");
    g_sounds[SOUND_LAND].type = SOUND_LAND;
    g_sounds[SOUND_LAND].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_LAND].default_volume = 0.6f;
    g_sounds[SOUND_LAND].default_pitch = 1.0f;
    
    strcpy(g_sounds[SOUND_BUNNY_HOP].filename, "sounds/bunny_hop.wav");
    g_sounds[SOUND_BUNNY_HOP].type = SOUND_BUNNY_HOP;
    g_sounds[SOUND_BUNNY_HOP].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_BUNNY_HOP].default_volume = 0.7f;
    g_sounds[SOUND_BUNNY_HOP].default_pitch = 1.2f;
    
    // UI sounds
    strcpy(g_sounds[SOUND_RELOAD].filename, "sounds/reload.wav");
    g_sounds[SOUND_RELOAD].type = SOUND_RELOAD;
    g_sounds[SOUND_RELOAD].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_RELOAD].default_volume = 0.6f;
    g_sounds[SOUND_RELOAD].default_pitch = 1.0f;
    
    strcpy(g_sounds[SOUND_PICKUP].filename, "sounds/pickup.wav");
    g_sounds[SOUND_PICKUP].type = SOUND_PICKUP;
    g_sounds[SOUND_PICKUP].audio_type = AUDIO_TYPE_SFX;
    g_sounds[SOUND_PICKUP].default_volume = 0.5f;
    g_sounds[SOUND_PICKUP].default_pitch = 1.0f;
    
    // Music
    strcpy(g_sounds[SOUND_BACKGROUND_MUSIC].filename, "music/background.ogg");
    g_sounds[SOUND_BACKGROUND_MUSIC].type = SOUND_BACKGROUND_MUSIC;
    g_sounds[SOUND_BACKGROUND_MUSIC].audio_type = AUDIO_TYPE_MUSIC;
    g_sounds[SOUND_BACKGROUND_MUSIC].default_volume = 0.6f;
    g_sounds[SOUND_BACKGROUND_MUSIC].default_pitch = 1.0f;
    
    strcpy(g_sounds[SOUND_MENU_MUSIC].filename, "music/menu.ogg");
    g_sounds[SOUND_MENU_MUSIC].type = SOUND_MENU_MUSIC;
    g_sounds[SOUND_MENU_MUSIC].audio_type = AUDIO_TYPE_MUSIC;
    g_sounds[SOUND_MENU_MUSIC].default_volume = 0.5f;
    g_sounds[SOUND_MENU_MUSIC].default_pitch = 1.0f;
    
    printf("Audio System initialized successfully\n");
    printf("- Master Volume: %.1f\n", g_audio_settings.master_volume);
    printf("- SFX Volume: %.1f\n", g_audio_settings.sfx_volume);
    printf("- Music Volume: %.1f\n", g_audio_settings.music_volume);
    printf("- Max Sources: %d\n", g_audio_settings.max_sources);
    
    return 1;
}

void cleanup_audio_system() {
    printf("Cleaning up Audio System...\n");
    
    stop_all_sounds();
    stop_music();
    unload_all_sounds();
    
    memset(g_audio_sources, 0, sizeof(g_audio_sources));
    g_audio_settings.current_sources = 0;
    g_next_source_id = 1;
    g_current_music_source = -1;
    
    printf("Audio System cleaned up\n");
}

int load_sound(const char* filename, SoundType type) {
    if (type >= MAX_SOUND_TYPES) {
        printf("Error: Invalid sound type %d\n", type);
        return 0;
    }
    
    // In a real implementation, this would load the actual audio file
    // For now, just mark as loaded
    strcpy(g_sounds[type].filename, filename);
    g_sounds[type].loaded = 1;
    
    printf("Loaded sound: %s (Type: %d)\n", filename, type);
    return 1;
}

void unload_sound(SoundType type) {
    if (type >= MAX_SOUND_TYPES) return;
    
    g_sounds[type].loaded = 0;
    printf("Unloaded sound type: %d\n", type);
}

void unload_all_sounds() {
    for (int i = 0; i < MAX_SOUND_TYPES; i++) {
        g_sounds[i].loaded = 0;
    }
    printf("Unloaded all sounds\n");
}

int play_sound_2d(SoundType type, float volume, float pitch) {
    if (!g_audio_settings.audio_enabled || type >= MAX_SOUND_TYPES) {
        return -1;
    }
    
    int source_index = find_free_source();
    if (source_index == -1) {
        printf("Warning: No free audio sources available\n");
        return -1;
    }
    
    AudioSource* source = &g_audio_sources[source_index];
    source->sound_id = g_next_source_id++;
    source->position = (Vector3){0.0f, 0.0f, 0.0f}; // 2D sound has no position
    source->volume = apply_volume_settings(volume, g_sounds[type].audio_type);
    source->pitch = pitch;
    source->looping = 0;
    source->type = g_sounds[type].audio_type;
    
    g_audio_settings.current_sources++;
    
    printf("Playing 2D sound: %s (ID: %d, Volume: %.2f, Pitch: %.2f)\n", 
           g_sounds[type].filename, source->sound_id, source->volume, source->pitch);
    
    return source->sound_id;
}

int play_sound_2d_simple(SoundType type) {
    if (type >= MAX_SOUND_TYPES) return -1;
    
    return play_sound_2d(type, g_sounds[type].default_volume, g_sounds[type].default_pitch);
}

int play_sound_3d(SoundType type, Vector3 position, float volume, float pitch) {
    if (!g_audio_settings.audio_enabled || type >= MAX_SOUND_TYPES) {
        return -1;
    }
    
    int source_index = find_free_source();
    if (source_index == -1) {
        printf("Warning: No free audio sources available\n");
        return -1;
    }
    
    // Calculate 3D audio parameters
    float distance_volume = calculate_3d_volume(position, g_listener_position, 50.0f);
    float doppler_pitch = calculate_3d_pitch(position, g_listener_position, g_listener_velocity);
    
    AudioSource* source = &g_audio_sources[source_index];
    source->sound_id = g_next_source_id++;
    source->position = position;
    source->volume = apply_volume_settings(volume * distance_volume, g_sounds[type].audio_type);
    source->pitch = pitch * doppler_pitch;
    source->looping = 0;
    source->type = g_sounds[type].audio_type;
    
    g_audio_settings.current_sources++;
    
    printf("Playing 3D sound: %s at (%.1f, %.1f, %.1f) (ID: %d, Volume: %.2f)\n", 
           g_sounds[type].filename, position.x, position.y, position.z, 
           source->sound_id, source->volume);
    
    return source->sound_id;
}

int play_sound_3d_simple(SoundType type, Vector3 position) {
    if (type >= MAX_SOUND_TYPES) return -1;
    
    return play_sound_3d(type, position, g_sounds[type].default_volume, g_sounds[type].default_pitch);
}

void stop_sound_2d(int source_id) {
    for (int i = 0; i < g_audio_settings.max_sources; i++) {
        if (g_audio_sources[i].sound_id == source_id) {
            memset(&g_audio_sources[i], 0, sizeof(AudioSource));
            g_audio_settings.current_sources--;
            printf("Stopped sound ID: %d\n", source_id);
            return;
        }
    }
}

void stop_sound_3d(int source_id) {
    stop_sound_2d(source_id); // Same implementation for now
}

int play_music(SoundType type, float volume, int loop) {
    if (!g_audio_settings.audio_enabled || type >= MAX_SOUND_TYPES) {
        return -1;
    }
    
    // Stop current music if playing
    if (g_current_music_source != -1) {
        stop_music();
    }
    
    int source_index = find_free_source();
    if (source_index == -1) {
        printf("Warning: No free audio sources for music\n");
        return -1;
    }
    
    AudioSource* source = &g_audio_sources[source_index];
    source->sound_id = g_next_source_id++;
    source->position = (Vector3){0.0f, 0.0f, 0.0f};
    source->volume = apply_volume_settings(volume, AUDIO_TYPE_MUSIC);
    source->pitch = 1.0f;
    source->looping = loop;
    source->type = AUDIO_TYPE_MUSIC;
    
    g_current_music_source = source->sound_id;
    g_audio_settings.current_sources++;
    
    printf("Playing music: %s (ID: %d, Volume: %.2f, Loop: %s)\n", 
           g_sounds[type].filename, source->sound_id, source->volume, 
           loop ? "Yes" : "No");
    
    return source->sound_id;
}

void stop_music() {
    if (g_current_music_source != -1) {
        stop_sound_2d(g_current_music_source);
        g_current_music_source = -1;
        printf("Music stopped\n");
    }
}

void pause_music() {
    // In a real implementation, this would pause the audio playback
    printf("Music paused\n");
}

void resume_music() {
    // In a real implementation, this would resume the audio playback
    printf("Music resumed\n");
}

void set_music_volume(float volume) {
    g_audio_settings.music_volume = fmaxf(0.0f, fminf(1.0f, volume));
    printf("Music volume set to: %.2f\n", g_audio_settings.music_volume);
}

void set_master_volume(float volume) {
    g_audio_settings.master_volume = fmaxf(0.0f, fminf(1.0f, volume));
    printf("Master volume set to: %.2f\n", g_audio_settings.master_volume);
}

void set_sfx_volume(float volume) {
    g_audio_settings.sfx_volume = fmaxf(0.0f, fminf(1.0f, volume));
    printf("SFX volume set to: %.2f\n", g_audio_settings.sfx_volume);
}

void set_music_volume_setting(float volume) {
    set_music_volume(volume);
}

void set_voice_volume(float volume) {
    g_audio_settings.voice_volume = fmaxf(0.0f, fminf(1.0f, volume));
    printf("Voice volume set to: %.2f\n", g_audio_settings.voice_volume);
}

AudioSettings* get_audio_settings() {
    return &g_audio_settings;
}

void set_listener_position(Vector3 position) {
    g_listener_position = position;
}

void set_listener_orientation(Vector3 forward, Vector3 up) {
    g_listener_forward = forward;
    g_listener_up = up;
}

void set_listener_velocity(Vector3 velocity) {
    g_listener_velocity = velocity;
}

void update_audio_sources(float delta_time) {
    cleanup_finished_sources();
    
    // Update 3D audio sources based on listener position
    for (int i = 0; i < g_audio_settings.max_sources; i++) {
        AudioSource* source = &g_audio_sources[i];
        if (source->sound_id > 0) {
            // Update 3D audio parameters if it's a positioned sound
            if (source->position.x != 0.0f || source->position.y != 0.0f || source->position.z != 0.0f) {
                float distance_volume = calculate_3d_volume(source->position, g_listener_position, 50.0f);
                // Update volume based on distance (simplified)
                // In a real implementation, this would update the actual audio source
            }
        }
    }
}

void stop_all_sounds() {
    for (int i = 0; i < g_audio_settings.max_sources; i++) {
        if (g_audio_sources[i].sound_id > 0) {
            memset(&g_audio_sources[i], 0, sizeof(AudioSource));
        }
    }
    g_audio_settings.current_sources = 0;
    g_current_music_source = -1;
    printf("All sounds stopped\n");
}

void pause_all_sounds() {
    printf("All sounds paused\n");
}

void resume_all_sounds() {
    printf("All sounds resumed\n");
}

int is_sound_playing(int source_id) {
    for (int i = 0; i < g_audio_settings.max_sources; i++) {
        if (g_audio_sources[i].sound_id == source_id) {
            return 1;
        }
    }
    return 0;
}

float calculate_3d_volume(Vector3 sound_pos, Vector3 listener_pos, float max_distance) {
    float dx = sound_pos.x - listener_pos.x;
    float dy = sound_pos.y - listener_pos.y;
    float dz = sound_pos.z - listener_pos.z;
    float distance = sqrtf(dx*dx + dy*dy + dz*dz);
    
    if (distance >= max_distance) {
        return 0.0f;
    }
    
    // Linear falloff
    return 1.0f - (distance / max_distance);
}

float calculate_3d_pitch(Vector3 sound_pos, Vector3 listener_pos, Vector3 listener_velocity) {
    // Simplified Doppler effect calculation
    Vector3 to_sound = {sound_pos.x - listener_pos.x, 
                       sound_pos.y - listener_pos.y, 
                       sound_pos.z - listener_pos.z};
    
    float distance = sqrtf(to_sound.x*to_sound.x + to_sound.y*to_sound.y + to_sound.z*to_sound.z);
    if (distance < 0.1f) return 1.0f;
    
    // Normalize direction
    to_sound.x /= distance;
    to_sound.y /= distance;
    to_sound.z /= distance;
    
    // Calculate velocity component towards sound
    float velocity_component = listener_velocity.x * to_sound.x + 
                              listener_velocity.y * to_sound.y + 
                              listener_velocity.z * to_sound.z;
    
    // Simple Doppler shift (speed of sound = 343 m/s)
    float doppler_factor = 1.0f + (velocity_component / 343.0f);
    
    return fmaxf(0.5f, fminf(2.0f, doppler_factor));
}

// Internal helper functions
static int find_free_source() {
    for (int i = 0; i < g_audio_settings.max_sources; i++) {
        if (g_audio_sources[i].sound_id == 0) {
            return i;
        }
    }
    return -1;
}

static void cleanup_finished_sources() {
    // In a real implementation, this would check if sounds have finished playing
    // For now, we'll simulate some sounds finishing after a short time
    static float cleanup_timer = 0.0f;
    cleanup_timer += 0.016f; // Assume 60 FPS
    
    if (cleanup_timer > 2.0f) { // Clean up every 2 seconds
        int cleaned = 0;
        for (int i = 0; i < g_audio_settings.max_sources; i++) {
            if (g_audio_sources[i].sound_id > 0 && !g_audio_sources[i].looping) {
                // Randomly clean up some non-looping sounds to simulate finishing
                if ((rand() % 10) < 3) { // 30% chance
                    memset(&g_audio_sources[i], 0, sizeof(AudioSource));
                    g_audio_settings.current_sources--;
                    cleaned++;
                }
            }
        }
        if (cleaned > 0) {
            printf("Cleaned up %d finished audio sources\n", cleaned);
        }
        cleanup_timer = 0.0f;
    }
}

static float apply_volume_settings(float base_volume, AudioType type) {
    float type_volume = 1.0f;
    
    switch (type) {
        case AUDIO_TYPE_SFX:
            type_volume = g_audio_settings.sfx_volume;
            break;
        case AUDIO_TYPE_MUSIC:
            type_volume = g_audio_settings.music_volume;
            break;
        case AUDIO_TYPE_VOICE:
            type_volume = g_audio_settings.voice_volume;
            break;
    }
    
    return base_volume * type_volume * g_audio_settings.master_volume;
}
#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include "../game_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// Audio system types
typedef enum {
    AUDIO_TYPE_SFX,
    AUDIO_TYPE_MUSIC,
    AUDIO_TYPE_VOICE
} AudioType;

typedef enum {
    SOUND_PLAYER_SHOOT,
    SOUND_ENEMY_SHOOT,
    SOUND_EXPLOSION,
    SOUND_ENEMY_HIT,
    SOUND_PLAYER_HIT,
    SOUND_ENEMY_DEATH,
    SOUND_FOOTSTEP,
    SOUND_JUMP,
    SOUND_LAND,
    SOUND_BUNNY_HOP,
    SOUND_RELOAD,
    SOUND_PICKUP,
    SOUND_BACKGROUND_MUSIC,
    SOUND_MENU_MUSIC,
    MAX_SOUND_TYPES
} SoundType;

typedef struct {
    int sound_id;
    Vector3 position;
    float volume;
    float pitch;
    int looping;
    AudioType type;
} AudioSource;

typedef struct {
    float master_volume;
    float sfx_volume;
    float music_volume;
    float voice_volume;
    int audio_enabled;
    int max_sources;
    int current_sources;
} AudioSettings;

// Audio system functions
int init_audio_system();
void cleanup_audio_system();

// Sound loading and management
int load_sound(const char* filename, SoundType type);
void unload_sound(SoundType type);
void unload_all_sounds();

// 2D sound playback
int play_sound_2d(SoundType type, float volume, float pitch);
int play_sound_2d_simple(SoundType type);
void stop_sound_2d(int source_id);

// 3D positional sound playback
int play_sound_3d(SoundType type, Vector3 position, float volume, float pitch);
int play_sound_3d_simple(SoundType type, Vector3 position);
void stop_sound_3d(int source_id);

// Music playback
int play_music(SoundType type, float volume, int loop);
void stop_music();
void pause_music();
void resume_music();
void set_music_volume(float volume);

// Volume and settings
void set_master_volume(float volume);
void set_sfx_volume(float volume);
void set_music_volume_setting(float volume);
void set_voice_volume(float volume);
AudioSettings* get_audio_settings();

// Listener (player) position for 3D audio
void set_listener_position(Vector3 position);
void set_listener_orientation(Vector3 forward, Vector3 up);
void set_listener_velocity(Vector3 velocity);

// Audio source management
void update_audio_sources(float delta_time);
void stop_all_sounds();
void pause_all_sounds();
void resume_all_sounds();

// Utility functions
int is_sound_playing(int source_id);
float calculate_3d_volume(Vector3 sound_pos, Vector3 listener_pos, float max_distance);
float calculate_3d_pitch(Vector3 sound_pos, Vector3 listener_pos, Vector3 listener_velocity);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_SYSTEM_H
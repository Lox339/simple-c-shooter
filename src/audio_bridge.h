#ifndef AUDIO_BRIDGE_H
#define AUDIO_BRIDGE_H

#include "game_api.h"
#include "audio/audio_system.h"

#ifdef __cplusplus
extern "C" {
#endif

// Audio bridge initialization
int init_audio_bridge();
void cleanup_audio_bridge();

// Game event audio functions
void play_player_shoot_sound();
void play_enemy_shoot_sound(Vector3 position);
void play_explosion_sound(Vector3 position);
void play_enemy_hit_sound(Vector3 position);
void play_player_hit_sound();
void play_enemy_death_sound(Vector3 position);

// Movement audio functions
void play_footstep_sound();
void play_jump_sound();
void play_land_sound();
void play_bunny_hop_sound();

// UI audio functions
void play_reload_sound();
void play_pickup_sound();

// Music functions
void start_background_music();
void start_menu_music();
void stop_current_music();

// Audio system integration
void update_audio_system(const GameState* game_state, float delta_time);
void set_audio_listener_from_player(const PlayerState* player);

// Volume control functions (for UI)
void audio_set_master_volume(float volume);
void audio_set_sfx_volume(float volume);
void audio_set_music_volume(float volume);
float audio_get_master_volume();
float audio_get_sfx_volume();
float audio_get_music_volume();

#ifdef __cplusplus
}
#endif

#endif // AUDIO_BRIDGE_H
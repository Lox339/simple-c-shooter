#ifndef SOUND_GENERATOR_H
#define SOUND_GENERATOR_H

#ifdef __cplusplus
extern "C" {
#endif

// Sound generation functions for creating placeholder audio
// In a real game, these would be replaced with actual audio files

// Generate basic sound effects
void generate_placeholder_sounds();
void create_sound_directory();

// Individual sound generators
void generate_shoot_sound(const char* filename, float pitch);
void generate_explosion_sound(const char* filename);
void generate_hit_sound(const char* filename);
void generate_footstep_sound(const char* filename);
void generate_jump_sound(const char* filename);
void generate_music_placeholder(const char* filename, int is_menu);

// Utility functions
void write_wav_header(FILE* file, int sample_rate, int duration_ms, int channels);
void generate_sine_wave(FILE* file, float frequency, int duration_ms, int sample_rate, float amplitude);
void generate_noise(FILE* file, int duration_ms, int sample_rate, float amplitude);
void generate_sweep(FILE* file, float start_freq, float end_freq, int duration_ms, int sample_rate, float amplitude);

#ifdef __cplusplus
}
#endif

#endif // SOUND_GENERATOR_H
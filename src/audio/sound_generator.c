#include "sound_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

void generate_placeholder_sounds() {
    printf("Generating placeholder sound files...\n");
    
    create_sound_directory();
    
    // Generate weapon sounds
    generate_shoot_sound("sounds/player_shoot.wav", 1.0f);
    generate_shoot_sound("sounds/enemy_shoot.wav", 0.8f);
    generate_explosion_sound("sounds/explosion.wav");
    
    // Generate impact sounds
    generate_hit_sound("sounds/enemy_hit.wav");
    generate_hit_sound("sounds/player_hit.wav");
    generate_hit_sound("sounds/enemy_death.wav");
    
    // Generate movement sounds
    generate_footstep_sound("sounds/footstep.wav");
    generate_jump_sound("sounds/jump.wav");
    generate_jump_sound("sounds/land.wav");
    generate_jump_sound("sounds/bunny_hop.wav");
    
    // Generate UI sounds
    generate_shoot_sound("sounds/reload.wav", 0.6f);
    generate_jump_sound("sounds/pickup.wav");
    
    // Generate music placeholders
    generate_music_placeholder("music/background.ogg", 0);
    generate_music_placeholder("music/menu.ogg", 1);
    
    printf("Placeholder sounds generated successfully!\n");
}

void create_sound_directory() {
    // Create sounds directory
    mkdir("sounds", 0755);
    mkdir("music", 0755);
    printf("Created sound directories\n");
}

void generate_shoot_sound(const char* filename, float pitch) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Warning: Could not create %s\n", filename);
        return;
    }
    
    int sample_rate = 22050;
    int duration_ms = 200;
    
    write_wav_header(file, sample_rate, duration_ms, 1);
    
    // Generate a quick "pop" sound with frequency sweep
    generate_sweep(file, 800.0f * pitch, 200.0f * pitch, duration_ms, sample_rate, 0.3f);
    
    fclose(file);
    printf("Generated: %s\n", filename);
}

void generate_explosion_sound(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Warning: Could not create %s\n", filename);
        return;
    }
    
    int sample_rate = 22050;
    int duration_ms = 800;
    
    write_wav_header(file, sample_rate, duration_ms, 1);
    
    // Generate explosion as noise burst with decay
    generate_noise(file, duration_ms, sample_rate, 0.5f);
    
    fclose(file);
    printf("Generated: %s\n", filename);
}

void generate_hit_sound(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Warning: Could not create %s\n", filename);
        return;
    }
    
    int sample_rate = 22050;
    int duration_ms = 150;
    
    write_wav_header(file, sample_rate, duration_ms, 1);
    
    // Generate a sharp "thwack" sound
    generate_sweep(file, 1200.0f, 400.0f, duration_ms, sample_rate, 0.4f);
    
    fclose(file);
    printf("Generated: %s\n", filename);
}

void generate_footstep_sound(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Warning: Could not create %s\n", filename);
        return;
    }
    
    int sample_rate = 22050;
    int duration_ms = 100;
    
    write_wav_header(file, sample_rate, duration_ms, 1);
    
    // Generate a soft "thud" sound
    generate_sine_wave(file, 80.0f, duration_ms, sample_rate, 0.2f);
    
    fclose(file);
    printf("Generated: %s\n", filename);
}

void generate_jump_sound(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Warning: Could not create %s\n", filename);
        return;
    }
    
    int sample_rate = 22050;
    int duration_ms = 300;
    
    write_wav_header(file, sample_rate, duration_ms, 1);
    
    // Generate an upward sweep for jump
    generate_sweep(file, 200.0f, 600.0f, duration_ms, sample_rate, 0.3f);
    
    fclose(file);
    printf("Generated: %s\n", filename);
}

void generate_music_placeholder(const char* filename, int is_menu) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Warning: Could not create %s\n", filename);
        return;
    }
    
    int sample_rate = 22050;
    int duration_ms = 10000; // 10 seconds of music
    
    write_wav_header(file, sample_rate, duration_ms, 1);
    
    // Generate simple melody based on type
    if (is_menu) {
        // Calm menu music - lower frequency
        generate_sine_wave(file, 220.0f, duration_ms, sample_rate, 0.1f);
    } else {
        // Action background music - higher frequency
        generate_sine_wave(file, 440.0f, duration_ms, sample_rate, 0.15f);
    }
    
    fclose(file);
    printf("Generated: %s\n", filename);
}

void write_wav_header(FILE* file, int sample_rate, int duration_ms, int channels) {
    int samples = (sample_rate * duration_ms) / 1000;
    int data_size = samples * channels * 2; // 16-bit samples
    int file_size = data_size + 36;
    
    // WAV header
    fwrite("RIFF", 1, 4, file);
    fwrite(&file_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);
    
    // Format chunk
    fwrite("fmt ", 1, 4, file);
    int fmt_size = 16;
    fwrite(&fmt_size, 4, 1, file);
    short format = 1; // PCM
    fwrite(&format, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&sample_rate, 4, 1, file);
    int byte_rate = sample_rate * channels * 2;
    fwrite(&byte_rate, 4, 1, file);
    short block_align = channels * 2;
    fwrite(&block_align, 2, 1, file);
    short bits_per_sample = 16;
    fwrite(&bits_per_sample, 2, 1, file);
    
    // Data chunk
    fwrite("data", 1, 4, file);
    fwrite(&data_size, 4, 1, file);
}

void generate_sine_wave(FILE* file, float frequency, int duration_ms, int sample_rate, float amplitude) {
    int samples = (sample_rate * duration_ms) / 1000;
    
    for (int i = 0; i < samples; i++) {
        float t = (float)i / sample_rate;
        float decay = 1.0f - (float)i / samples; // Linear decay
        float sample = amplitude * decay * sinf(2.0f * M_PI * frequency * t);
        short sample_16 = (short)(sample * 32767);
        fwrite(&sample_16, 2, 1, file);
    }
}

void generate_noise(FILE* file, int duration_ms, int sample_rate, float amplitude) {
    int samples = (sample_rate * duration_ms) / 1000;
    
    for (int i = 0; i < samples; i++) {
        float decay = 1.0f - (float)i / samples; // Linear decay
        float sample = amplitude * decay * ((float)rand() / RAND_MAX * 2.0f - 1.0f);
        short sample_16 = (short)(sample * 32767);
        fwrite(&sample_16, 2, 1, file);
    }
}

void generate_sweep(FILE* file, float start_freq, float end_freq, int duration_ms, int sample_rate, float amplitude) {
    int samples = (sample_rate * duration_ms) / 1000;
    
    for (int i = 0; i < samples; i++) {
        float t = (float)i / sample_rate;
        float progress = (float)i / samples;
        float decay = 1.0f - progress; // Linear decay
        float frequency = start_freq + (end_freq - start_freq) * progress;
        float sample = amplitude * decay * sinf(2.0f * M_PI * frequency * t);
        short sample_16 = (short)(sample * 32767);
        fwrite(&sample_16, 2, 1, file);
    }
}
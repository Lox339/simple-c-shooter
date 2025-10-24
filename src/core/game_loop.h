#ifndef GAME_LOOP_H
#define GAME_LOOP_H

// Game loop structure
typedef struct {
    double last_time;
    double current_time;
    double delta_time;
    int target_fps;
} GameLoop;

// Core Engine function declarations
void init_core_engine();
void run_game_loop();
void update_game_logic(float delta_time);
void update_gameplay(float delta_time);
void cleanup_core();

// FPS control functions
void set_target_fps(int fps);
int get_target_fps();
double get_delta_time();

#endif // GAME_LOOP_H
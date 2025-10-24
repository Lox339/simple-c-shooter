#ifndef UI_BRIDGE_H
#define UI_BRIDGE_H

#include "game_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// UI Manager bridge functions
bool init_ui_manager();
void update_ui_manager(float delta_time);
void render_ui_manager();
void handle_ui_input(int key, int action);
void cleanup_ui_manager();

// UI rendering functions (called by C#)
void render_text(const char* text, float x, float y, float r, float g, float b);
void render_ui_background(float x, float y, float width, float height, float r, float g, float b, float a);
void render_crosshair(float x, float y, float size, float r, float g, float b);

// Data access functions (called by C#)
GameState* get_game_state();
int get_window_width();
int get_window_height();

#ifdef __cplusplus
}
#endif

#endif // UI_BRIDGE_H
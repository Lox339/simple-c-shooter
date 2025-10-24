#ifndef GRAPHICS_BRIDGE_H
#define GRAPHICS_BRIDGE_H

#include "game_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// Graphics engine bridge functions
bool init_graphics_engine();
void render_game_frame(const GameState* game_state);
bool graphics_should_close();
void cleanup_graphics_engine();

#ifdef __cplusplus
}
#endif

#endif // GRAPHICS_BRIDGE_H
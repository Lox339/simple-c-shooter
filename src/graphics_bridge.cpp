// Bridge between C Core Engine and C++ Graphics Engine
#include "graphics/renderer.hpp"
#include "game_api.h"
#include <iostream>

// Global renderer instance
static Renderer* g_renderer = nullptr;

extern "C" {
    
bool init_graphics_engine() {
    std::cout << "Initializing Graphics Bridge..." << std::endl;
    
    if (g_renderer) {
        std::cerr << "Graphics engine already initialized!" << std::endl;
        return false;
    }
    
    g_renderer = new Renderer();
    
    if (!g_renderer->initialize()) {
        std::cerr << "Failed to initialize graphics engine" << std::endl;
        delete g_renderer;
        g_renderer = nullptr;
        return false;
    }
    
    std::cout << "Graphics Bridge initialized successfully" << std::endl;
    return true;
}

void render_game_frame(const GameState* game_state) {
    if (!g_renderer || !game_state) {
        return;
    }
    
    g_renderer->render_frame(*game_state);
}

bool graphics_should_close() {
    if (!g_renderer) {
        return true;
    }
    
    return g_renderer->should_close();
}

void cleanup_graphics_engine() {
    std::cout << "Cleaning up Graphics Bridge..." << std::endl;
    
    if (g_renderer) {
        g_renderer->cleanup();
        delete g_renderer;
        g_renderer = nullptr;
    }
    
    std::cout << "Graphics Bridge cleaned up" << std::endl;
}

} // extern "C"
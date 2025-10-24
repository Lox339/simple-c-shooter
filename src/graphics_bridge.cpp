// Bridge between C Core Engine and C++ Graphics Engine
#include "graphics/renderer.hpp"
#include "game_api.h"
#include <iostream>

// Global renderer instance
static Renderer* g_renderer = nullptr;

// Forward declaration
void render_speedometer_overlay(const GameState* game_state);

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

// External UI rendering functions
extern "C" void render_text_opengl(const char* text, float x, float y, float r, float g, float b);
extern "C" void render_ui_background_opengl(float x, float y, float width, float height, 
                                           float r, float g, float b, float a);

void render_game_frame(const GameState* game_state) {
    if (!g_renderer || !game_state) {
        return;
    }
    
    g_renderer->render_frame(*game_state);
    
    // Render speedometer overlay
    render_speedometer_overlay(game_state);
}

void render_speedometer_overlay(const GameState* game_state) {
    if (!game_state) return;
    
    // Get window dimensions
    int width = g_renderer->get_window_width();
    int height = g_renderer->get_window_height();
    
    // Speedometer position (right upper corner)
    float speedometer_x = width - 220.0f;
    float speedometer_y = 20.0f;
    float speedometer_width = 200.0f;
    float speedometer_height = 80.0f;
    
    // Render background
    render_ui_background_opengl(speedometer_x, speedometer_y, speedometer_width, speedometer_height,
                               0.0f, 0.0f, 0.0f, 0.7f);
    
    // Format speed text
    char speed_text[64];
    snprintf(speed_text, sizeof(speed_text), "Speed: %.1f u/s", game_state->player.speed);
    
    // Color based on speed
    float r = 1.0f, g = 1.0f, b = 1.0f; // Default white
    
    if (game_state->player.speed > 20.0f) {
        r = 1.0f; g = 0.0f; b = 0.0f; // Red for bunny hop
    } else if (game_state->player.speed > 15.0f) {
        r = 1.0f; g = 1.0f; b = 0.0f; // Yellow for fast
    }
    
    // Render speed text
    render_text_opengl(speed_text, speedometer_x + 10, speedometer_y + 20, r, g, b);
    
    // Render ground/air status
    const char* status_text = game_state->player.on_ground ? "Ground" : "Air";
    render_text_opengl(status_text, speedometer_x + 10, speedometer_y + 40, 0.8f, 0.8f, 0.8f);
    
    // Render bunny hop indicator
    if (game_state->player.speed > 12.0f && !game_state->player.on_ground) {
        render_text_opengl("BUNNY HOP!", speedometer_x + 10, speedometer_y + 60, 1.0f, 0.0f, 0.0f);
    }
}

bool graphics_should_close() {
    if (!g_renderer) {
        return true;
    }
    
    return g_renderer->should_close();
}

int get_graphics_window_width() {
    if (g_renderer) {
        return g_renderer->get_window_width();
    }
    return 0;
}

int get_graphics_window_height() {
    if (g_renderer) {
        return g_renderer->get_window_height();
    }
    return 0;
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
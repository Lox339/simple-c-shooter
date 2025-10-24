// Bridge between C Core Engine and C++ Graphics Engine
#include "graphics/renderer.hpp"
#include "game_api.h"
#include <iostream>

// Global renderer instance
static Renderer* g_renderer = nullptr;

// Forward declarations
void render_speedometer_overlay(const GameState* game_state);
void render_game_hud(const GameState* game_state);
void render_health_bar(const GameState* game_state, float x, float y);
void render_ammo_counter(const GameState* game_state, float x, float y);
void render_score_display(const GameState* game_state, float x, float y);
void render_game_over_overlay(const GameState* game_state, int width, int height);

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

// Hit effects function
void create_hit_effect_at_position(float x, float y, float z, int effect_type, float damage) {
    if (!g_renderer) return;
    
    Vector3 position = {x, y, z};
    
    // Access the hit effects system from renderer (we'll need to add a getter)
    // For now, just print debug info
    std::cout << "Creating hit effect at (" << x << ", " << y << ", " << z 
              << ") - Type: " << effect_type << ", Damage: " << damage << std::endl;
}

void render_game_frame(const GameState* game_state) {
    if (!g_renderer || !game_state) {
        return;
    }
    
    g_renderer->render_frame(*game_state);
    
    // Render UI overlays
    render_speedometer_overlay(game_state);
    render_game_hud(game_state);
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

void render_game_hud(const GameState* game_state) {
    if (!game_state) return;
    
    int width = g_renderer->get_window_width();
    int height = g_renderer->get_window_height();
    
    // Render health bar (bottom left)
    render_health_bar(game_state, 20, height - 60);
    
    // Render ammo counter (bottom right)
    render_ammo_counter(game_state, width - 150, height - 60);
    
    // Render score (top left)
    render_score_display(game_state, 20, 20);
    
    // Render crosshair (center)
    render_crosshair_opengl(width / 2.0f, height / 2.0f, 20.0f, 1.0f, 1.0f, 1.0f);
    
    // Render game over overlay if needed
    if (game_state->current_phase == 3) { // GAME_OVER
        render_game_over_overlay(game_state, width, height);
    }
}

void render_health_bar(const GameState* game_state, float x, float y) {
    const float bar_width = 200.0f;
    const float bar_height = 20.0f;
    
    // Background
    render_ui_background_opengl(x, y, bar_width, bar_height, 0.2f, 0.2f, 0.2f, 0.8f);
    
    // Health bar fill
    float health_percent = (float)game_state->player.health / game_state->player.max_health;
    float fill_width = (bar_width - 4) * health_percent;
    
    // Color based on health
    float r = health_percent > 0.3f ? 0.0f : 1.0f; // Red when low
    float g = health_percent > 0.3f ? 1.0f : 0.0f; // Green when healthy
    float b = 0.0f;
    
    render_ui_background_opengl(x + 2, y + 2, fill_width, bar_height - 4, r, g, b, 0.9f);
    
    // Health text
    char health_text[64];
    snprintf(health_text, sizeof(health_text), "Health: %d/%d", 
             game_state->player.health, game_state->player.max_health);
    render_text_opengl(health_text, x, y - 20, 1.0f, 1.0f, 1.0f);
}

void render_ammo_counter(const GameState* game_state, float x, float y) {
    // Ammo text
    char ammo_text[64];
    snprintf(ammo_text, sizeof(ammo_text), "Ammo: %d/%d", 
             game_state->player.ammo, game_state->player.max_ammo);
    render_text_opengl(ammo_text, x, y, 0.0f, 1.0f, 1.0f); // Cyan
    
    // Low ammo warning
    if (game_state->player.ammo <= game_state->player.max_ammo * 0.2f) {
        render_text_opengl("LOW AMMO!", x, y - 20, 1.0f, 0.0f, 0.0f); // Red
    }
}

void render_score_display(const GameState* game_state, float x, float y) {
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Score: %d", game_state->score);
    render_text_opengl(score_text, x, y, 1.0f, 1.0f, 0.0f); // Yellow
    
    // Additional game info
    char info_text[128];
    snprintf(info_text, sizeof(info_text), "Enemies: %d  Projectiles: %d", 
             game_state->enemy_count, game_state->projectile_count);
    render_text_opengl(info_text, x, y + 20, 0.8f, 0.8f, 0.8f); // Gray
}

void render_game_over_overlay(const GameState* game_state, int width, int height) {
    // Semi-transparent overlay
    render_ui_background_opengl(0, 0, width, height, 0.0f, 0.0f, 0.0f, 0.7f);
    
    // Game over text
    float center_x = width / 2.0f - 100;
    float center_y = height / 2.0f - 50;
    
    render_text_opengl("GAME OVER", center_x, center_y, 1.0f, 0.0f, 0.0f);
    
    char final_score_text[64];
    snprintf(final_score_text, sizeof(final_score_text), "Final Score: %d", game_state->score);
    render_text_opengl(final_score_text, center_x - 20, center_y + 30, 1.0f, 1.0f, 0.0f);
    
    render_text_opengl("Press Q to quit", center_x - 30, center_y + 60, 0.8f, 0.8f, 0.8f);
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
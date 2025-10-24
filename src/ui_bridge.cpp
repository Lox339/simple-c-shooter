// Bridge between C Core Engine and C# UI Manager
#include "game_api.h"
#include <iostream>

// Global UI state
static bool g_ui_initialized = false;

extern "C" {

// UI rendering functions that C# will call
void render_text(const char* text, float x, float y, float r, float g, float b) {
    // Placeholder - in real implementation, this would render text using OpenGL
    // For now, just output to console occasionally
    static int call_count = 0;
    if (++call_count % 100 == 0) { // Only print every 100th call to avoid spam
        printf("[UI] Text at (%.0f,%.0f): %s\n", x, y, text);
    }
}

void render_ui_background(float x, float y, float width, float height, float r, float g, float b, float a) {
    // Placeholder - in real implementation, this would render a colored rectangle
    static int call_count = 0;
    if (++call_count % 50 == 0) { // Only print every 50th call
        printf("[UI] Background at (%.0f,%.0f) size %.0fx%.0f\n", x, y, width, height);
    }
}

void render_crosshair(float x, float y, float size, float r, float g, float b) {
    // Placeholder - in real implementation, this would render crosshair lines
    static int call_count = 0;
    if (++call_count % 200 == 0) { // Only print every 200th call
        printf("[UI] Crosshair at (%.0f,%.0f) size %.0f\n", x, y, size);
    }
}

// Functions that C# can call to get game data
GameState* get_game_state() {
    // This should return the actual game state
    // For now, we'll need to implement this properly
    static GameState dummy_state = {0};
    return &dummy_state;
}

int get_window_width() {
    return 1024; // Default width
}

int get_window_height() {
    return 768; // Default height
}

// UI management functions
bool init_ui_manager() {
    std::cout << "Initializing UI Bridge..." << std::endl;
    
    // In a real implementation, we would:
    // 1. Load the C# assembly
    // 2. Get the UIManager class
    // 3. Create an instance
    // 4. Call Initialize method
    
    g_ui_initialized = true;
    std::cout << "UI Bridge initialized (placeholder)" << std::endl;
    return true;
}

void update_ui_manager(float delta_time) {
    if (!g_ui_initialized) return;
    
    // In a real implementation, we would call the C# Update method
    static float update_timer = 0.0f;
    update_timer += delta_time;
    
    if (update_timer >= 1.0f) { // Update UI info every second
        printf("[UI] UI Update - Delta: %.3f\n", delta_time);
        update_timer = 0.0f;
    }
}

void render_ui_manager() {
    if (!g_ui_initialized) return;
    
    // In a real implementation, we would call the C# Render method
    static int render_count = 0;
    if (++render_count % 300 == 0) { // Print every 300 frames (~5 seconds at 60fps)
        printf("[UI] UI Render call #%d\n", render_count);
    }
}

void handle_ui_input(int key, int action) {
    if (!g_ui_initialized) return;
    
    // In a real implementation, we would call the C# HandleInput method
    if (action == 1) { // Key pressed
        printf("[UI] UI Input: key %d pressed\n", key);
    }
}

void cleanup_ui_manager() {
    if (!g_ui_initialized) return;
    
    std::cout << "Cleaning up UI Bridge..." << std::endl;
    
    // In a real implementation, we would:
    // 1. Call C# Cleanup method
    // 2. Release the C# assembly
    
    g_ui_initialized = false;
    std::cout << "UI Bridge cleaned up" << std::endl;
}

} // extern "C"
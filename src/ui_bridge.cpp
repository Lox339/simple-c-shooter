// Bridge between C Core Engine and C# UI Manager
#include "game_api.h"
#include <iostream>

// Global UI state
static bool g_ui_initialized = false;

extern "C" {

// External UI rendering functions from graphics engine
extern "C" void render_text_opengl(const char* text, float x, float y, float r, float g, float b);
extern "C" void render_ui_background_opengl(float x, float y, float width, float height, 
                                           float r, float g, float b, float a);
extern "C" void render_crosshair_opengl(float x, float y, float size, float r, float g, float b);

// UI rendering functions that C# will call
void render_text(const char* text, float x, float y, float r, float g, float b) {
    render_text_opengl(text, x, y, r, g, b);
    
    // Debug output occasionally
    static int call_count = 0;
    if (++call_count % 100 == 0) {
        printf("[UI] Rendered text: %s at (%.0f,%.0f)\n", text, x, y);
    }
}

void render_ui_background(float x, float y, float width, float height, float r, float g, float b, float a) {
    render_ui_background_opengl(x, y, width, height, r, g, b, a);
    
    // Debug output occasionally
    static int call_count = 0;
    if (++call_count % 50 == 0) {
        printf("[UI] Rendered background at (%.0f,%.0f) size %.0fx%.0f\n", x, y, width, height);
    }
}

void render_crosshair(float x, float y, float size, float r, float g, float b) {
    render_crosshair_opengl(x, y, size, r, g, b);
    
    // Debug output occasionally
    static int call_count = 0;
    if (++call_count % 200 == 0) {
        printf("[UI] Rendered crosshair at (%.0f,%.0f) size %.0f\n", x, y, size);
    }
}

// External function to get actual game state from Core Engine
extern "C" GameState* get_core_game_state();

// Functions that C# can call to get game data
GameState* get_game_state() {
    // Return the actual game state from Core Engine
    return get_core_game_state();
}

// External functions to get window size from graphics engine
extern "C" int get_graphics_window_width();
extern "C" int get_graphics_window_height();

int get_window_width() {
    int width = get_graphics_window_width();
    return width > 0 ? width : 1024; // Fallback to default
}

int get_window_height() {
    int height = get_graphics_window_height();
    return height > 0 ? height : 768; // Fallback to default
}

// External UI renderer functions
extern "C" bool init_ui_renderer();
extern "C" void cleanup_ui_renderer();

// UI management functions
bool init_ui_manager() {
    std::cout << "Initializing UI Bridge..." << std::endl;
    
    // Initialize OpenGL UI renderer
    if (!init_ui_renderer()) {
        std::cerr << "Failed to initialize UI renderer" << std::endl;
        return false;
    }
    
    // In a real implementation, we would also:
    // 1. Load the C# assembly
    // 2. Get the UIManager class
    // 3. Create an instance
    // 4. Call Initialize method
    
    g_ui_initialized = true;
    std::cout << "UI Bridge initialized with OpenGL renderer" << std::endl;
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
    
    // Cleanup UI renderer
    cleanup_ui_renderer();
    
    // In a real implementation, we would also:
    // 1. Call C# Cleanup method
    // 2. Release the C# assembly
    
    g_ui_initialized = false;
    std::cout << "UI Bridge cleaned up" << std::endl;
}

} // extern "C"
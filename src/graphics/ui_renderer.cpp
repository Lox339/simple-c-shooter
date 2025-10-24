// UI Renderer for OpenGL text and UI elements
#include "ui_renderer.hpp"
#include <iostream>
#include <cstring>

// OpenGL headers
#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#else
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

UIRenderer::UIRenderer() : initialized(false) {
}

UIRenderer::~UIRenderer() {
    cleanup();
}

bool UIRenderer::initialize() {
    std::cout << "Initializing UI Renderer..." << std::endl;
    
    // Initialize OpenGL state for UI rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    initialized = true;
    std::cout << "UI Renderer initialized" << std::endl;
    return true;
}

void UIRenderer::render_text(const char* text, float x, float y, float r, float g, float b) {
    if (!initialized || !text) return;
    
    // Save current OpenGL state
    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Set up orthographic projection for UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    // Get viewport dimensions
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Disable depth testing for UI
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Set text color
    glColor3f(r, g, b);
    
    // Position text
    glRasterPos2f(x, y);
    
    // Render each character (simple bitmap font)
    render_bitmap_string(text);
    
    // Restore OpenGL state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
    glPopMatrix();
}

void UIRenderer::render_ui_background(float x, float y, float width, float height, 
                                     float r, float g, float b, float a) {
    if (!initialized) return;
    
    // Save current OpenGL state
    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Set up orthographic projection for UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Disable depth testing for UI
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Set background color
    glColor4f(r, g, b, a);
    
    // Render rectangle
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
    
    // Restore OpenGL state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
    glPopMatrix();
}

void UIRenderer::render_crosshair(float x, float y, float size, float r, float g, float b) {
    if (!initialized) return;
    
    // Save current OpenGL state
    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Set up orthographic projection for UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Disable depth testing for UI
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Set crosshair color
    glColor3f(r, g, b);
    glLineWidth(2.0f);
    
    // Render crosshair lines
    float half_size = size * 0.5f;
    
    glBegin(GL_LINES);
    // Horizontal line
    glVertex2f(x - half_size, y);
    glVertex2f(x + half_size, y);
    // Vertical line
    glVertex2f(x, y - half_size);
    glVertex2f(x, y + half_size);
    glEnd();
    
    // Restore OpenGL state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
    glPopMatrix();
}

void UIRenderer::render_bitmap_string(const char* text) {
    // Simple bitmap font rendering (placeholder)
    // In a real implementation, this would use a proper font system
    
    // For now, just render simple rectangles for each character
    float char_width = 8.0f;
    float char_height = 12.0f;
    
    GLfloat current_pos[4];
    glGetFloatv(GL_CURRENT_RASTER_POSITION, current_pos);
    
    float x = current_pos[0];
    float y = current_pos[1];
    
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        char c = text[i];
        
        if (c == ' ') {
            x += char_width;
            continue;
        }
        
        // Render a simple rectangle for each character
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + char_width, y);
        glVertex2f(x + char_width, y + char_height);
        glVertex2f(x, y + char_height);
        glEnd();
        
        x += char_width;
    }
}

void UIRenderer::cleanup() {
    if (!initialized) return;
    
    initialized = false;
    std::cout << "UI Renderer cleaned up" << std::endl;
}

// Global UI renderer instance
static UIRenderer g_ui_renderer;

extern "C" {

bool init_ui_renderer() {
    return g_ui_renderer.initialize();
}

void render_text_opengl(const char* text, float x, float y, float r, float g, float b) {
    g_ui_renderer.render_text(text, x, y, r, g, b);
}

void render_ui_background_opengl(float x, float y, float width, float height, 
                                float r, float g, float b, float a) {
    g_ui_renderer.render_ui_background(x, y, width, height, r, g, b, a);
}

void render_crosshair_opengl(float x, float y, float size, float r, float g, float b) {
    g_ui_renderer.render_crosshair(x, y, size, r, g, b);
}

void cleanup_ui_renderer() {
    g_ui_renderer.cleanup();
}

} // extern "C"
#ifndef UI_RENDERER_HPP
#define UI_RENDERER_HPP

class UIRenderer {
private:
    bool initialized;
    
    void render_bitmap_string(const char* text);

public:
    UIRenderer();
    ~UIRenderer();
    
    bool initialize();
    void cleanup();
    
    // UI rendering functions
    void render_text(const char* text, float x, float y, float r, float g, float b);
    void render_ui_background(float x, float y, float width, float height, 
                             float r, float g, float b, float a);
    void render_crosshair(float x, float y, float size, float r, float g, float b);
    
    bool is_initialized() const { return initialized; }
};

// C interface functions
extern "C" {
    bool init_ui_renderer();
    void render_text_opengl(const char* text, float x, float y, float r, float g, float b);
    void render_ui_background_opengl(float x, float y, float width, float height, 
                                    float r, float g, float b, float a);
    void render_crosshair_opengl(float x, float y, float size, float r, float g, float b);
    void cleanup_ui_renderer();
}

#endif // UI_RENDERER_HPP
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "../game_api.h"
#include "camera.hpp"

#ifdef GLFW_AVAILABLE
struct GLFWwindow;
#endif

// Simple 4x4 matrix structure
struct Matrix4 {
    float data[16];
    
    Matrix4();
    Matrix4(float diagonal);
};

// Matrix operations
Matrix4 create_identity_matrix();
Matrix4 create_translation_matrix(float x, float y, float z);
Matrix4 create_scale_matrix(float x, float y, float z);
Matrix4 create_rotation_matrix_x(float angle);
Matrix4 create_rotation_matrix_y(float angle);
Matrix4 create_rotation_matrix_z(float angle);
Matrix4 multiply_matrices(const Matrix4& a, const Matrix4& b);
Matrix4 create_perspective_matrix(float fov, float aspect, float near, float far);
Matrix4 create_look_at_matrix(float eye_x, float eye_y, float eye_z,
                             float center_x, float center_y, float center_z,
                             float up_x, float up_y, float up_z);

// Graphics Engine class
class Renderer {
private:
#ifdef GLFW_AVAILABLE
    GLFWwindow* window;
#else
    void* window;
#endif
    
    unsigned int shader_program;
    unsigned int vao, vbo, ebo;
    int window_width, window_height;
    bool initialized;
    
    Camera camera;
    
    // Private methods
    bool create_shader_program();
    void setup_basic_geometry();
    void render_cube();
    void set_matrix_uniform(const char* name, const Matrix4& matrix);
    
#ifdef GLFW_AVAILABLE
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
#endif

public:
    Renderer();
    ~Renderer();
    
    bool initialize();
    void render_frame(const GameState& game_state);
    bool should_close();
    void cleanup();
    
    // Getters
    int get_window_width() const { return window_width; }
    int get_window_height() const { return window_height; }
};

#endif // RENDERER_HPP
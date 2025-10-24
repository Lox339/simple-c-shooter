// Graphics & Physics Engine - Renderer Implementation
// OpenGL 3D rendering system

#include "renderer.hpp"
#include "camera.hpp"
#include "model.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// OpenGL headers
#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

// GLFW for window management
#ifdef GLFW_AVAILABLE
#include <GLFW/glfw3.h>
#endif

// Enhanced vertex shader with lighting support
const char* vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

out vec3 vertexColor;
out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;
out vec3 lightDir;
out vec3 viewDir;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    
    vertexColor = aColor;
    normal = mat3(transpose(inverse(model))) * aNormal;
    fragPos = vec3(worldPos);
    texCoord = aTexCoord;
    
    lightDir = normalize(lightPos - fragPos);
    viewDir = normalize(viewPos - fragPos);
}
)";

// Enhanced fragment shader with basic lighting
const char* fragment_shader_source = R"(
#version 330 core
in vec3 vertexColor;
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in vec3 lightDir;
in vec3 viewDir;

uniform vec3 lightColor;
uniform float ambientStrength;
uniform float specularStrength;

out vec4 FragColor;

void main() {
    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * vertexColor;
    FragColor = vec4(result, 1.0);
}
)";

Renderer::Renderer() : 
    window(nullptr),
    shader_program(0),
    vao(0),
    vbo(0),
    ebo(0),
    window_width(1024),
    window_height(768),
    initialized(false) {
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize() {
    std::cout << "Initializing Graphics Engine..." << std::endl;
    
#ifdef GLFW_AVAILABLE
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create window
    window = glfwCreateWindow(window_width, window_height, "Simple 3D Shooter", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Set viewport
    glViewport(0, 0, window_width, window_height);
    
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#else
    std::cout << "GLFW not available - using software rendering fallback" << std::endl;
#endif
    
    // Initialize shaders
    if (!create_shader_program()) {
        std::cerr << "Failed to create shader program" << std::endl;
        return false;
    }
    
    // Setup lighting
    setup_lighting();
    
    // Initialize models
    if (!initialize_models()) {
        std::cerr << "Failed to initialize 3D models" << std::endl;
        return false;
    }
    
    // Initialize camera
    camera.initialize();
    
    // Initialize projectile trail system
    projectile_trail.initialize();
    
    // Initialize hit effects system
    if (!hit_effects.initialize()) {
        std::cerr << "Failed to initialize hit effects system" << std::endl;
        return false;
    }
    
    initialized = true;
    std::cout << "Graphics Engine initialized successfully" << std::endl;
    return true;
}

bool Renderer::create_shader_program() {
    // Compile vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    
    // Check vertex shader compilation
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cerr << "Vertex shader compilation failed: " << info_log << std::endl;
        return false;
    }
    
    // Compile fragment shader
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    
    // Check fragment shader compilation
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cerr << "Fragment shader compilation failed: " << info_log << std::endl;
        return false;
    }
    
    // Create shader program
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    
    // Check program linking
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cerr << "Shader program linking failed: " << info_log << std::endl;
        return false;
    }
    
    // Clean up shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    std::cout << "Shaders compiled and linked successfully" << std::endl;
    return true;
}

void Renderer::setup_lighting() {
    // Set lighting uniforms
    glUseProgram(shader_program);
    
    // Light position (above and to the side)
    int light_pos_loc = glGetUniformLocation(shader_program, "lightPos");
    glUniform3f(light_pos_loc, 10.0f, 10.0f, 10.0f);
    
    // Light color (white)
    int light_color_loc = glGetUniformLocation(shader_program, "lightColor");
    glUniform3f(light_color_loc, 1.0f, 1.0f, 1.0f);
    
    // Ambient strength
    int ambient_loc = glGetUniformLocation(shader_program, "ambientStrength");
    glUniform1f(ambient_loc, 0.3f);
    
    // Specular strength
    int specular_loc = glGetUniformLocation(shader_program, "specularStrength");
    glUniform1f(specular_loc, 0.5f);
    
    std::cout << "Lighting setup complete" << std::endl;
}

void Renderer::render_frame(const GameState& game_state) {
    if (!initialized) return;
    
#ifdef GLFW_AVAILABLE
    if (glfwWindowShouldClose(window)) {
        return;
    }
#endif
    
    // Clear screen
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);  // Dark blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(shader_program);
    
    // Update camera based on player state
    camera.set_position(game_state.player.position.x, 
                       game_state.player.position.y + 1.8f,  // Eye height
                       game_state.player.position.z);
    camera.set_rotation(game_state.player.rotation.x, game_state.player.rotation.y);
    
    // Get matrices
    Matrix4 view_matrix = camera.get_view_matrix();
    Matrix4 projection_matrix = camera.get_projection_matrix(window_width, window_height);
    
    // Set uniforms
    set_matrix_uniform("view", view_matrix);
    set_matrix_uniform("projection", projection_matrix);
    
    // Set view position for lighting
    int view_pos_loc = glGetUniformLocation(shader_program, "viewPos");
    glUniform3f(view_pos_loc, game_state.player.position.x, 
                game_state.player.position.y + 1.8f, game_state.player.position.z);
    
    // Get model pointers
    const Model* cube_model = get_cube_model();
    const Model* sphere_model = get_sphere_model();
    const Model* plane_model = get_plane_model();
    
    // Render player (as a small cube at player position for debugging)
    if (cube_model) {
        Matrix4 player_model = create_translation_matrix(game_state.player.position.x,
                                                        game_state.player.position.y + 0.5f,
                                                        game_state.player.position.z);
        player_model = multiply_matrices(player_model, create_scale_matrix(0.2f, 0.2f, 0.2f));
        set_matrix_uniform("model", player_model);
        cube_model->render();
    }
    
    // Render enemies with different models and colors based on type and AI state
    for (int i = 0; i < game_state.enemy_count; i++) {
        const Enemy& enemy = game_state.enemies[i];
        if (enemy.ai_state == AI_DEAD || !enemy.is_active) continue;
        
        Matrix4 enemy_model = create_translation_matrix(enemy.position.x,
                                                       enemy.position.y + 0.5f,
                                                       enemy.position.z);
        
        // Choose model, scale, and color based on enemy type
        const Model* enemy_model_ptr = nullptr;
        float scale = 1.0f;
        Vector3 enemy_color = {1.0f, 0.0f, 0.0f}; // Default red
        
        switch (enemy.type) {
            case ENEMY_BASIC:
                enemy_model_ptr = cube_model;
                scale = 1.0f;
                enemy_color = {0.8f, 0.2f, 0.2f}; // Dark red
                break;
            case ENEMY_FAST:
                enemy_model_ptr = sphere_model;
                scale = 0.8f;
                enemy_color = {0.2f, 0.8f, 0.2f}; // Green
                break;
            case ENEMY_HEAVY:
                enemy_model_ptr = cube_model;
                scale = 1.4f;
                enemy_color = {0.2f, 0.2f, 0.8f}; // Blue
                break;
        }
        
        // Modify color based on AI state
        switch (enemy.ai_state) {
            case AI_PATROL:
                // Dim the color for patrolling enemies
                enemy_color.x *= 0.6f;
                enemy_color.y *= 0.6f;
                enemy_color.z *= 0.6f;
                break;
            case AI_CHASE:
                // Brighten color for chasing enemies
                enemy_color.x = std::min(1.0f, enemy_color.x * 1.3f);
                enemy_color.y = std::min(1.0f, enemy_color.y * 1.3f);
                enemy_color.z = std::min(1.0f, enemy_color.z * 1.3f);
                break;
            case AI_ATTACK:
                // Flash red for attacking enemies
                static float attack_flash = 0.0f;
                attack_flash += game_state.delta_time * 10.0f;
                float flash_intensity = (sin(attack_flash) + 1.0f) * 0.5f;
                enemy_color = {1.0f, flash_intensity * 0.3f, flash_intensity * 0.3f};
                break;
            default:
                break;
        }
        
        // Apply health-based color modification
        if (enemy.health > 0) {
            float health_ratio = enemy.health / 100.0f; // Assuming max health around 100
            if (health_ratio < 0.3f) {
                // Flash when low health
                static float low_health_flash = 0.0f;
                low_health_flash += game_state.delta_time * 8.0f;
                float flash = (sin(low_health_flash) + 1.0f) * 0.5f;
                enemy_color.x = std::max(enemy_color.x, flash);
            }
        }
        
        if (enemy_model_ptr) {
            enemy_model = multiply_matrices(enemy_model, create_scale_matrix(scale, scale, scale));
            set_matrix_uniform("model", enemy_model);
            
            // Set enemy color uniform
            int color_loc = glGetUniformLocation(shader_program, "objectColor");
            glUniform3f(color_loc, enemy_color.x, enemy_color.y, enemy_color.z);
            
            enemy_model_ptr->render();
        }
    }
    
    // Update and render projectile trails
    projectile_trail.update(game_state, game_state.delta_time);
    projectile_trail.render(shader_program);
    
    // Update and render hit effects
    hit_effects.update(game_state.delta_time);
    hit_effects.render(shader_program);
    
    // Render projectiles as small spheres with glow effect
    if (sphere_model) {
        for (int i = 0; i < game_state.projectile_count; i++) {
            const Projectile& projectile = game_state.projectiles[i];
            
            Matrix4 projectile_model = create_translation_matrix(projectile.position.x,
                                                               projectile.position.y,
                                                               projectile.position.z);
            projectile_model = multiply_matrices(projectile_model, create_scale_matrix(0.15f, 0.15f, 0.15f));
            
            set_matrix_uniform("model", projectile_model);
            sphere_model->render();
        }
    }
    
    // Render ground plane
    if (plane_model) {
        Matrix4 ground_model = create_translation_matrix(0.0f, -0.5f, 0.0f);
        set_matrix_uniform("model", ground_model);
        plane_model->render();
    }
    
#ifdef GLFW_AVAILABLE
    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();
#endif
}

// Remove old render_cube function - now using Model system

void Renderer::set_matrix_uniform(const char* name, const Matrix4& matrix) {
    int location = glGetUniformLocation(shader_program, name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix.data);
    }
}

bool Renderer::should_close() {
#ifdef GLFW_AVAILABLE
    return window ? glfwWindowShouldClose(window) : true;
#else
    return false;
#endif
}

void Renderer::cleanup() {
    if (!initialized) return;
    
    std::cout << "Cleaning up Graphics Engine..." << std::endl;
    
    // Clean up models
    cleanup_models();
    
    // Clean up projectile trails
    projectile_trail.cleanup();
    
    // Clean up OpenGL objects
    if (shader_program) glDeleteProgram(shader_program);
    
    camera.cleanup();
    
#ifdef GLFW_AVAILABLE
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
#endif
    
    initialized = false;
    std::cout << "Graphics Engine cleaned up" << std::endl;
}

#ifdef GLFW_AVAILABLE
void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
#endif
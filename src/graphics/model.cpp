#include "model.hpp"
#include "renderer.hpp"
#include <iostream>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

Model::Model() : vao(0), vbo(0), ebo(0), vertex_count(0), index_count(0), initialized(false) {
}

Model::~Model() {
    cleanup();
}

bool Model::initialize() {
    std::cout << "Model system initialized" << std::endl;
    return true;
}

bool Model::load_cube() {
    if (initialized) {
        cleanup();
    }
    
    // Enhanced cube with normals and texture coordinates
    float vertices[] = {
        // Positions          // Colors           // Normals          // Tex Coords
        // Front face
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        
        // Back face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        
        // Left face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 1.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        
        // Right face
         0.5f, -0.5f, -0.5f,  0.5f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        
        // Top face
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.5f, 1.0f, 0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
    };
    
    vertex_count = 24;
    index_count = 36;
    
    return load_geometry(vertices, sizeof(vertices), indices, sizeof(indices));
}

bool Model::load_sphere(int segments) {
    if (initialized) {
        cleanup();
    }
    
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Generate sphere vertices
    for (int lat = 0; lat <= segments; lat++) {
        float theta = lat * M_PI / segments;
        float sin_theta = sinf(theta);
        float cos_theta = cosf(theta);
        
        for (int lon = 0; lon <= segments; lon++) {
            float phi = lon * 2 * M_PI / segments;
            float sin_phi = sinf(phi);
            float cos_phi = cosf(phi);
            
            float x = cos_phi * sin_theta;
            float y = cos_theta;
            float z = sin_phi * sin_theta;
            
            // Position
            vertices.push_back(x * 0.5f);
            vertices.push_back(y * 0.5f);
            vertices.push_back(z * 0.5f);
            
            // Color (based on position)
            vertices.push_back((x + 1.0f) * 0.5f);
            vertices.push_back((y + 1.0f) * 0.5f);
            vertices.push_back((z + 1.0f) * 0.5f);
            
            // Normal
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Texture coordinates
            vertices.push_back((float)lon / segments);
            vertices.push_back((float)lat / segments);
        }
    }
    
    // Generate sphere indices
    for (int lat = 0; lat < segments; lat++) {
        for (int lon = 0; lon < segments; lon++) {
            int first = lat * (segments + 1) + lon;
            int second = first + segments + 1;
            
            // First triangle
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            // Second triangle
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    vertex_count = (segments + 1) * (segments + 1);
    index_count = indices.size();
    
    return load_geometry(vertices.data(), vertices.size() * sizeof(float), 
                        indices.data(), indices.size() * sizeof(unsigned int));
}

bool Model::load_plane(float width, float height) {
    if (initialized) {
        cleanup();
    }
    
    float half_width = width * 0.5f;
    float half_height = height * 0.5f;
    
    float vertices[] = {
        // Positions                    // Colors           // Normals          // Tex Coords
        -half_width, 0.0f, -half_height, 0.2f, 0.8f, 0.2f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         half_width, 0.0f, -half_height, 0.2f, 0.8f, 0.2f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         half_width, 0.0f,  half_height, 0.2f, 0.8f, 0.2f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -half_width, 0.0f,  half_height, 0.2f, 0.8f, 0.2f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0
    };
    
    vertex_count = 4;
    index_count = 6;
    
    return load_geometry(vertices, sizeof(vertices), indices, sizeof(indices));
}

bool Model::load_geometry(const float* vertices, size_t vertices_size, 
                         const unsigned int* indices, size_t indices_size) {
    // Generate and bind VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Generate and bind VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    
    // Generate and bind EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
    
    // Vertex attributes
    int stride = 11 * sizeof(float); // pos(3) + color(3) + normal(3) + texcoord(2)
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Normal attribute (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Texture coordinate attribute (location 3)
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);
    
    // Unbind VAO
    glBindVertexArray(0);
    
    initialized = true;
    std::cout << "Model loaded - Vertices: " << vertex_count << ", Indices: " << index_count << std::endl;
    return true;
}

void Model::render() const {
    if (!initialized) {
        return;
    }
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Model::cleanup() {
    if (!initialized) {
        return;
    }
    
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    
    vao = vbo = ebo = 0;
    vertex_count = index_count = 0;
    initialized = false;
}

// Static model manager
static Model g_cube_model;
static Model g_sphere_model;
static Model g_plane_model;
static bool g_models_initialized = false;

bool initialize_models() {
    if (g_models_initialized) {
        return true;
    }
    
    std::cout << "Initializing 3D models..." << std::endl;
    
    // Load basic models
    if (!g_cube_model.load_cube()) {
        std::cerr << "Failed to load cube model" << std::endl;
        return false;
    }
    
    if (!g_sphere_model.load_sphere(16)) {
        std::cerr << "Failed to load sphere model" << std::endl;
        return false;
    }
    
    if (!g_plane_model.load_plane(100.0f, 100.0f)) {
        std::cerr << "Failed to load plane model" << std::endl;
        return false;
    }
    
    g_models_initialized = true;
    std::cout << "3D models initialized successfully" << std::endl;
    return true;
}

const Model* get_cube_model() {
    return g_models_initialized ? &g_cube_model : nullptr;
}

const Model* get_sphere_model() {
    return g_models_initialized ? &g_sphere_model : nullptr;
}

const Model* get_plane_model() {
    return g_models_initialized ? &g_plane_model : nullptr;
}

void cleanup_models() {
    if (!g_models_initialized) {
        return;
    }
    
    std::cout << "Cleaning up 3D models..." << std::endl;
    
    g_cube_model.cleanup();
    g_sphere_model.cleanup();
    g_plane_model.cleanup();
    
    g_models_initialized = false;
    std::cout << "3D models cleaned up" << std::endl;
}
#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstddef>

class Model {
private:
    unsigned int vao, vbo, ebo;
    int vertex_count, index_count;
    bool initialized;
    
    bool load_geometry(const float* vertices, size_t vertices_size, 
                      const unsigned int* indices, size_t indices_size);

public:
    Model();
    ~Model();
    
    bool initialize();
    void cleanup();
    
    // Model loading functions
    bool load_cube();
    bool load_sphere(int segments = 16);
    bool load_plane(float width, float height);
    
    // Rendering
    void render() const;
    
    // Getters
    bool is_initialized() const { return initialized; }
    int get_vertex_count() const { return vertex_count; }
    int get_index_count() const { return index_count; }
};

// Global model management functions
bool initialize_models();
void cleanup_models();

// Get predefined models
const Model* get_cube_model();
const Model* get_sphere_model();
const Model* get_plane_model();

#endif // MODEL_HPP
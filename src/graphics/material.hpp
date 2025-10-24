#ifndef MATERIAL_HPP
#define MATERIAL_HPP

struct Material {
    float ambient[3];    // Ambient color
    float diffuse[3];    // Diffuse color
    float specular[3];   // Specular color
    float shininess;     // Specular shininess
    
    Material() : shininess(32.0f) {
        // Default material (white)
        ambient[0] = ambient[1] = ambient[2] = 0.2f;
        diffuse[0] = diffuse[1] = diffuse[2] = 0.8f;
        specular[0] = specular[1] = specular[2] = 1.0f;
    }
    
    Material(float r, float g, float b, float shine = 32.0f) : shininess(shine) {
        ambient[0] = r * 0.2f; ambient[1] = g * 0.2f; ambient[2] = b * 0.2f;
        diffuse[0] = r * 0.8f; diffuse[1] = g * 0.8f; diffuse[2] = b * 0.8f;
        specular[0] = specular[1] = specular[2] = 1.0f;
    }
};

// Predefined materials
namespace Materials {
    extern const Material Red;
    extern const Material Green;
    extern const Material Blue;
    extern const Material Yellow;
    extern const Material Cyan;
    extern const Material Magenta;
    extern const Material White;
    extern const Material Gray;
    extern const Material Gold;
    extern const Material Silver;
}

#endif // MATERIAL_HPP
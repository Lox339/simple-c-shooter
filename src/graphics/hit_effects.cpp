#include "hit_effects.hpp"
#include <GL/glew.h>
#include <iostream>
#include <cmath>
#include <algorithm>

HitEffectsSystem::HitEffectsSystem() : particle_vao(0), particle_vbo(0) {
}

HitEffectsSystem::~HitEffectsSystem() {
    cleanup();
}

bool HitEffectsSystem::initialize() {
    // Create particle rendering buffers
    float particle_vertices[] = {
        // positions    // texture coords
        -0.5f, -0.5f,   0.0f, 0.0f,
         0.5f, -0.5f,   1.0f, 0.0f,
         0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f,   0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &particle_vao);
    glGenBuffers(1, &particle_vbo);
    
    glBindVertexArray(particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);
    
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    std::cout << "Hit effects system initialized" << std::endl;
    return true;
}

void HitEffectsSystem::cleanup() {
    if (particle_vao) {
        glDeleteVertexArrays(1, &particle_vao);
        particle_vao = 0;
    }
    if (particle_vbo) {
        glDeleteBuffers(1, &particle_vbo);
        particle_vbo = 0;
    }
    
    effects.clear();
}

void HitEffectsSystem::create_explosion_effect(Vector3 position, float size) {
    // Create multiple particles for explosion effect
    for (int i = 0; i < 8; i++) {
        HitEffect effect;
        effect.position = position;
        effect.position.x += ((float)rand() / RAND_MAX - 0.5f) * size;
        effect.position.y += ((float)rand() / RAND_MAX - 0.5f) * size;
        effect.position.z += ((float)rand() / RAND_MAX - 0.5f) * size;
        
        effect.color = {1.0f, 0.5f + (float)rand() / RAND_MAX * 0.5f, 0.0f}; // Orange-red
        effect.lifetime = 0.5f + (float)rand() / RAND_MAX * 0.3f;
        effect.max_lifetime = effect.lifetime;
        effect.size = size * (0.5f + (float)rand() / RAND_MAX * 0.5f);
        effect.type = 0; // explosion
        
        effects.push_back(effect);
    }
    
    std::cout << "Created explosion effect at (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void HitEffectsSystem::create_blood_effect(Vector3 position, float size) {
    // Create blood splatter particles
    for (int i = 0; i < 5; i++) {
        HitEffect effect;
        effect.position = position;
        effect.position.x += ((float)rand() / RAND_MAX - 0.5f) * size * 0.5f;
        effect.position.y += ((float)rand() / RAND_MAX - 0.5f) * size * 0.5f;
        effect.position.z += ((float)rand() / RAND_MAX - 0.5f) * size * 0.5f;
        
        effect.color = {0.8f, 0.1f, 0.1f}; // Dark red
        effect.lifetime = 1.0f + (float)rand() / RAND_MAX * 0.5f;
        effect.max_lifetime = effect.lifetime;
        effect.size = size * (0.3f + (float)rand() / RAND_MAX * 0.4f);
        effect.type = 1; // blood
        
        effects.push_back(effect);
    }
}

void HitEffectsSystem::create_spark_effect(Vector3 position, float size) {
    // Create spark particles
    for (int i = 0; i < 6; i++) {
        HitEffect effect;
        effect.position = position;
        effect.position.x += ((float)rand() / RAND_MAX - 0.5f) * size * 0.3f;
        effect.position.y += ((float)rand() / RAND_MAX - 0.5f) * size * 0.3f;
        effect.position.z += ((float)rand() / RAND_MAX - 0.5f) * size * 0.3f;
        
        effect.color = {1.0f, 1.0f, 0.5f + (float)rand() / RAND_MAX * 0.5f}; // Yellow-white
        effect.lifetime = 0.3f + (float)rand() / RAND_MAX * 0.2f;
        effect.max_lifetime = effect.lifetime;
        effect.size = size * (0.2f + (float)rand() / RAND_MAX * 0.3f);
        effect.type = 2; // spark
        
        effects.push_back(effect);
    }
}

void HitEffectsSystem::create_damage_number(Vector3 position, float damage) {
    HitEffect effect;
    effect.position = position;
    effect.position.y += 1.0f; // Float above hit point
    
    // Color based on damage amount
    if (damage >= 50.0f) {
        effect.color = {1.0f, 0.0f, 0.0f}; // Red for high damage
    } else if (damage >= 25.0f) {
        effect.color = {1.0f, 0.5f, 0.0f}; // Orange for medium damage
    } else {
        effect.color = {1.0f, 1.0f, 0.0f}; // Yellow for low damage
    }
    
    effect.lifetime = 1.5f;
    effect.max_lifetime = effect.lifetime;
    effect.size = 0.5f + damage * 0.01f; // Size based on damage
    effect.type = 3; // damage number
    
    effects.push_back(effect);
}

void HitEffectsSystem::update(float delta_time) {
    // Update all effects
    for (auto it = effects.begin(); it != effects.end();) {
        it->lifetime -= delta_time;
        
        // Move particles based on type
        switch (it->type) {
            case 0: // explosion
                it->position.y += delta_time * 2.0f; // Rise up
                break;
            case 1: // blood
                it->position.y -= delta_time * 1.0f; // Fall down
                break;
            case 2: // spark
                it->position.y += delta_time * 3.0f; // Rise quickly
                break;
            case 3: // damage number
                it->position.y += delta_time * 1.5f; // Float up
                break;
        }
        
        // Fade out over time
        float fade_ratio = it->lifetime / it->max_lifetime;
        it->color.x *= fade_ratio;
        it->color.y *= fade_ratio;
        it->color.z *= fade_ratio;
        
        // Remove expired effects
        if (it->lifetime <= 0.0f) {
            it = effects.erase(it);
        } else {
            ++it;
        }
    }
}

void HitEffectsSystem::render(unsigned int shader_program) {
    if (effects.empty() || !particle_vao) {
        return;
    }
    
    // Enable blending for particle effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(particle_vao);
    
    for (const auto& effect : effects) {
        // Set particle color
        int color_loc = glGetUniformLocation(shader_program, "objectColor");
        glUniform3f(color_loc, effect.color.x, effect.color.y, effect.color.z);
        
        // Create model matrix for particle
        // This is a simplified version - in a real implementation you'd use proper matrix math
        glUniform3f(glGetUniformLocation(shader_program, "particlePos"), 
                   effect.position.x, effect.position.y, effect.position.z);
        glUniform1f(glGetUniformLocation(shader_program, "particleSize"), effect.size);
        
        // Render particle
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

void HitEffectsSystem::clear_all_effects() {
    effects.clear();
}
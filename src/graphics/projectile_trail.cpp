#include "projectile_trail.hpp"
#include "math_utils.cpp"
#include <iostream>
#include <algorithm>

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

ProjectileTrail::ProjectileTrail() : 
    trail_duration(0.5f),
    trail_spacing(0.05f) {
}

ProjectileTrail::~ProjectileTrail() {
    cleanup();
}

void ProjectileTrail::initialize() {
    trails.resize(MAX_PROJECTILES);
    std::cout << "Projectile Trail system initialized" << std::endl;
}

void ProjectileTrail::update(const GameState& game_state, float delta_time) {
    // Update existing trails
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        auto& trail = trails[i];
        
        // Update trail points
        for (auto it = trail.begin(); it != trail.end();) {
            it->lifetime -= delta_time;
            it->alpha = it->lifetime / trail_duration;
            
            if (it->lifetime <= 0.0f) {
                it = trail.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // Add new trail points for active projectiles
    for (int i = 0; i < game_state.projectile_count; i++) {
        const Projectile& projectile = game_state.projectiles[i];
        
        // Add trail point if enough distance traveled
        bool should_add = true;
        if (!trails[i].empty()) {
            const TrailPoint& last_point = trails[i].back();
            float dx = projectile.position.x - last_point.position.x;
            float dy = projectile.position.y - last_point.position.y;
            float dz = projectile.position.z - last_point.position.z;
            float distance = sqrtf(dx*dx + dy*dy + dz*dz);
            
            should_add = distance >= trail_spacing;
        }
        
        if (should_add) {
            add_trail_point(i, projectile.position);
        }
    }
    
    // Clear trails for removed projectiles
    for (int i = game_state.projectile_count; i < MAX_PROJECTILES; i++) {
        if (!trails[i].empty()) {
            trails[i].clear();
        }
    }
}

void ProjectileTrail::render(unsigned int shader_program) {
    // Disable depth writing for transparent trails
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Render each trail
    for (const auto& trail : trails) {
        if (trail.size() < 2) continue;
        
        // Render trail as line strip
        glBegin(GL_LINE_STRIP);
        
        for (const auto& point : trail) {
            // Color based on alpha (fade out)
            float r = 1.0f;
            float g = 0.5f;
            float b = 0.0f;
            float a = point.alpha;
            
            glColor4f(r, g, b, a);
            glVertex3f(point.position.x, point.position.y, point.position.z);
        }
        
        glEnd();
    }
    
    // Re-enable depth writing
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void ProjectileTrail::add_trail_point(int projectile_id, const Vector3& position) {
    if (projectile_id < 0 || projectile_id >= MAX_PROJECTILES) {
        return;
    }
    
    TrailPoint point;
    point.position = position;
    point.lifetime = trail_duration;
    point.alpha = 1.0f;
    
    trails[projectile_id].push_back(point);
    
    // Limit trail length
    const size_t max_trail_points = 20;
    if (trails[projectile_id].size() > max_trail_points) {
        trails[projectile_id].erase(trails[projectile_id].begin());
    }
}

void ProjectileTrail::clear_trail(int projectile_id) {
    if (projectile_id >= 0 && projectile_id < MAX_PROJECTILES) {
        trails[projectile_id].clear();
    }
}

void ProjectileTrail::clear_all_trails() {
    for (auto& trail : trails) {
        trail.clear();
    }
}

void ProjectileTrail::cleanup() {
    clear_all_trails();
    std::cout << "Projectile Trail system cleaned up" << std::endl;
}

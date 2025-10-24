#ifndef PROJECTILE_TRAIL_HPP
#define PROJECTILE_TRAIL_HPP

#include "../game_api.h"
#include <vector>

// Trail point structure
struct TrailPoint {
    Vector3 position;
    float lifetime;
    float alpha;
};

// Projectile trail system
class ProjectileTrail {
private:
    std::vector<std::vector<TrailPoint>> trails; // One trail per projectile
    float trail_duration;
    float trail_spacing;
    
public:
    ProjectileTrail();
    ~ProjectileTrail();
    
    void initialize();
    void update(const GameState& game_state, float delta_time);
    void render(unsigned int shader_program);
    void cleanup();
    
    // Trail management
    void add_trail_point(int projectile_id, const Vector3& position);
    void clear_trail(int projectile_id);
    void clear_all_trails();
};

#endif // PROJECTILE_TRAIL_HPP

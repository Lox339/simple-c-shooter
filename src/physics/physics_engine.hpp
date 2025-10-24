#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

#include "../game_api.h"
#include "collision_detector.hpp"
#include <vector>

// Bounding box structure
struct BoundingBox {
    Vector3 min;
    Vector3 max;
};

// Rigid body structure
struct RigidBody {
    int id;
    Vector3 position;
    Vector3 velocity;
    Vector3 size;           // Width, height, depth
    BoundingBox bounding_box;
    
    bool active;
    bool kinematic;         // If true, not affected by physics
    bool use_gravity;
    bool use_air_resistance;
    bool on_ground;
    
    // Constructor
    RigidBody() : 
        id(-1),
        position({0.0f, 0.0f, 0.0f}),
        velocity({0.0f, 0.0f, 0.0f}),
        size({1.0f, 1.0f, 1.0f}),
        active(true),
        kinematic(false),
        use_gravity(true),
        use_air_resistance(true),
        on_ground(false) {}
};

// Raycast hit information
struct RaycastHit {
    Vector3 point;
    Vector3 normal;
    float distance;
    int body_id;
};

class PhysicsEngine {
private:
    std::vector<RigidBody> rigid_bodies;
    CollisionDetector collision_detector;
    
    float gravity;
    float air_resistance;
    float ground_friction;
    bool initialized;
    
    void update_rigid_body(RigidBody& body, float delta_time);
    void update_bounding_box(RigidBody& body);

public:
    PhysicsEngine();
    ~PhysicsEngine();
    
    bool initialize();
    void update(float delta_time);
    void cleanup();
    
    // Rigid body management
    int add_rigid_body(const RigidBody& body);
    RigidBody* get_rigid_body(int id);
    void remove_rigid_body(int id);
    
    // Physics operations
    void apply_force(int body_id, const Vector3& force);
    void apply_impulse(int body_id, const Vector3& impulse);
    void set_velocity(int body_id, const Vector3& velocity);
    void set_position(int body_id, const Vector3& position);
    
    // Raycasting
    bool raycast(const Vector3& origin, const Vector3& direction, float max_distance, RaycastHit& hit);
    
    // Utility functions
    float calculate_player_speed(const PlayerState& player);
    
    // Settings
    void set_gravity(float gravity);
    void set_air_resistance(float resistance);
    void set_ground_friction(float friction);
    
    // Getters
    float get_gravity() const { return gravity; }
    float get_air_resistance() const { return air_resistance; }
    float get_ground_friction() const { return ground_friction; }
    int get_rigid_body_count() const;
};

#endif // PHYSICS_ENGINE_HPP
#ifndef COLLISION_DETECTOR_HPP
#define COLLISION_DETECTOR_HPP

#include "../game_api.h"
#include <vector>

// Forward declaration
struct RigidBody;

// Collision information structure
struct CollisionInfo {
    int body_a_id;
    int body_b_id;
    Vector3 contact_point;
    Vector3 normal;
    float penetration;
    
    CollisionInfo() : body_a_id(-1), body_b_id(-1), penetration(0.0f) {
        contact_point = {0.0f, 0.0f, 0.0f};
        normal = {0.0f, 1.0f, 0.0f};
    }
};

class CollisionDetector {
private:
    std::vector<CollisionInfo> collision_pairs;
    bool initialized;
    
    void resolve_collision(RigidBody& a, RigidBody& b, const CollisionInfo& collision);

public:
    CollisionDetector();
    ~CollisionDetector();
    
    bool initialize();
    void cleanup();
    
    // Collision detection
    void detect_collisions(std::vector<RigidBody>& bodies);
    bool check_aabb_collision(const RigidBody& a, const RigidBody& b, CollisionInfo& collision);
    bool check_sphere_collision(const RigidBody& a, const RigidBody& b, CollisionInfo& collision);
    
    // Utility functions
    bool point_in_aabb(const Vector3& point, const RigidBody& body);
    float distance_between_bodies(const RigidBody& a, const RigidBody& b);
    
    // Getters
    const std::vector<CollisionInfo>& get_collision_pairs() const;
};

#endif // COLLISION_DETECTOR_HPP
#include "collision_detector.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

CollisionDetector::CollisionDetector() : initialized(false) {
}

CollisionDetector::~CollisionDetector() {
    cleanup();
}

bool CollisionDetector::initialize() {
    std::cout << "Collision Detector initialized" << std::endl;
    initialized = true;
    return true;
}

void CollisionDetector::detect_collisions(std::vector<RigidBody>& bodies) {
    if (!initialized || bodies.empty()) {
        return;
    }
    
    // Clear previous collision data
    collision_pairs.clear();
    
    // Check all pairs of bodies
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            if (!bodies[i].active || !bodies[j].active) {
                continue;
            }
            
            // Skip if both are kinematic
            if (bodies[i].kinematic && bodies[j].kinematic) {
                continue;
            }
            
            CollisionInfo collision;
            if (check_aabb_collision(bodies[i], bodies[j], collision)) {
                collision_pairs.push_back(collision);
                resolve_collision(bodies[i], bodies[j], collision);
            }
        }
    }
}

bool CollisionDetector::check_aabb_collision(const RigidBody& a, const RigidBody& b, CollisionInfo& collision) {
    // AABB (Axis-Aligned Bounding Box) collision detection
    bool x_overlap = a.bounding_box.max.x >= b.bounding_box.min.x && 
                     b.bounding_box.max.x >= a.bounding_box.min.x;
    bool y_overlap = a.bounding_box.max.y >= b.bounding_box.min.y && 
                     b.bounding_box.max.y >= a.bounding_box.min.y;
    bool z_overlap = a.bounding_box.max.z >= b.bounding_box.min.z && 
                     b.bounding_box.max.z >= a.bounding_box.min.z;
    
    if (x_overlap && y_overlap && z_overlap) {
        // Calculate collision details
        collision.body_a_id = a.id;
        collision.body_b_id = b.id;
        
        // Calculate overlap distances
        float x_overlap_dist = std::min(a.bounding_box.max.x - b.bounding_box.min.x,
                                       b.bounding_box.max.x - a.bounding_box.min.x);
        float y_overlap_dist = std::min(a.bounding_box.max.y - b.bounding_box.min.y,
                                       b.bounding_box.max.y - a.bounding_box.min.y);
        float z_overlap_dist = std::min(a.bounding_box.max.z - b.bounding_box.min.z,
                                       b.bounding_box.max.z - a.bounding_box.min.z);
        
        // Find the axis with minimum overlap (separation axis)
        if (x_overlap_dist <= y_overlap_dist && x_overlap_dist <= z_overlap_dist) {
            collision.normal.x = (a.position.x < b.position.x) ? -1.0f : 1.0f;
            collision.normal.y = 0.0f;
            collision.normal.z = 0.0f;
            collision.penetration = x_overlap_dist;
        } else if (y_overlap_dist <= z_overlap_dist) {
            collision.normal.x = 0.0f;
            collision.normal.y = (a.position.y < b.position.y) ? -1.0f : 1.0f;
            collision.normal.z = 0.0f;
            collision.penetration = y_overlap_dist;
        } else {
            collision.normal.x = 0.0f;
            collision.normal.y = 0.0f;
            collision.normal.z = (a.position.z < b.position.z) ? -1.0f : 1.0f;
            collision.penetration = z_overlap_dist;
        }
        
        // Calculate contact point (midpoint of overlap)
        collision.contact_point.x = (std::max(a.bounding_box.min.x, b.bounding_box.min.x) + 
                                    std::min(a.bounding_box.max.x, b.bounding_box.max.x)) * 0.5f;
        collision.contact_point.y = (std::max(a.bounding_box.min.y, b.bounding_box.min.y) + 
                                    std::min(a.bounding_box.max.y, b.bounding_box.max.y)) * 0.5f;
        collision.contact_point.z = (std::max(a.bounding_box.min.z, b.bounding_box.min.z) + 
                                    std::min(a.bounding_box.max.z, b.bounding_box.max.z)) * 0.5f;
        
        return true;
    }
    
    return false;
}

bool CollisionDetector::check_sphere_collision(const RigidBody& a, const RigidBody& b, CollisionInfo& collision) {
    // Calculate distance between centers
    float dx = a.position.x - b.position.x;
    float dy = a.position.y - b.position.y;
    float dz = a.position.z - b.position.z;
    float distance_squared = dx * dx + dy * dy + dz * dz;
    
    // Calculate radii (assume spheres, use average of size components)
    float radius_a = (a.size.x + a.size.y + a.size.z) / 6.0f; // Divide by 6 for radius
    float radius_b = (b.size.x + b.size.y + b.size.z) / 6.0f;
    float combined_radius = radius_a + radius_b;
    
    if (distance_squared <= combined_radius * combined_radius) {
        collision.body_a_id = a.id;
        collision.body_b_id = b.id;
        
        float distance = sqrtf(distance_squared);
        collision.penetration = combined_radius - distance;
        
        if (distance > 0.0f) {
            collision.normal.x = dx / distance;
            collision.normal.y = dy / distance;
            collision.normal.z = dz / distance;
        } else {
            // Objects are at the same position, use arbitrary normal
            collision.normal.x = 1.0f;
            collision.normal.y = 0.0f;
            collision.normal.z = 0.0f;
        }
        
        // Contact point is on the line between centers
        collision.contact_point.x = a.position.x - collision.normal.x * radius_a;
        collision.contact_point.y = a.position.y - collision.normal.y * radius_a;
        collision.contact_point.z = a.position.z - collision.normal.z * radius_a;
        
        return true;
    }
    
    return false;
}

void CollisionDetector::resolve_collision(RigidBody& a, RigidBody& b, const CollisionInfo& collision) {
    // Separate objects to prevent overlap
    float separation_factor = 0.5f;
    
    if (!a.kinematic) {
        a.position.x += collision.normal.x * collision.penetration * separation_factor;
        a.position.y += collision.normal.y * collision.penetration * separation_factor;
        a.position.z += collision.normal.z * collision.penetration * separation_factor;
    }
    
    if (!b.kinematic) {
        b.position.x -= collision.normal.x * collision.penetration * separation_factor;
        b.position.y -= collision.normal.y * collision.penetration * separation_factor;
        b.position.z -= collision.normal.z * collision.penetration * separation_factor;
    }
    
    // Apply collision response (simplified elastic collision)
    float restitution = 0.3f; // Bounciness factor
    
    // Calculate relative velocity
    float rel_vel_x = a.velocity.x - b.velocity.x;
    float rel_vel_y = a.velocity.y - b.velocity.y;
    float rel_vel_z = a.velocity.z - b.velocity.z;
    
    // Calculate relative velocity along collision normal
    float vel_along_normal = rel_vel_x * collision.normal.x + 
                            rel_vel_y * collision.normal.y + 
                            rel_vel_z * collision.normal.z;
    
    // Don't resolve if velocities are separating
    if (vel_along_normal > 0) {
        return;
    }
    
    // Calculate impulse scalar
    float impulse_scalar = -(1 + restitution) * vel_along_normal;
    
    // Assume equal mass for simplicity
    impulse_scalar /= 2.0f;
    
    // Apply impulse
    if (!a.kinematic) {
        a.velocity.x += impulse_scalar * collision.normal.x;
        a.velocity.y += impulse_scalar * collision.normal.y;
        a.velocity.z += impulse_scalar * collision.normal.z;
    }
    
    if (!b.kinematic) {
        b.velocity.x -= impulse_scalar * collision.normal.x;
        b.velocity.y -= impulse_scalar * collision.normal.y;
        b.velocity.z -= impulse_scalar * collision.normal.z;
    }
}

bool CollisionDetector::point_in_aabb(const Vector3& point, const RigidBody& body) {
    return point.x >= body.bounding_box.min.x && point.x <= body.bounding_box.max.x &&
           point.y >= body.bounding_box.min.y && point.y <= body.bounding_box.max.y &&
           point.z >= body.bounding_box.min.z && point.z <= body.bounding_box.max.z;
}

float CollisionDetector::distance_between_bodies(const RigidBody& a, const RigidBody& b) {
    float dx = a.position.x - b.position.x;
    float dy = a.position.y - b.position.y;
    float dz = a.position.z - b.position.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

const std::vector<CollisionInfo>& CollisionDetector::get_collision_pairs() const {
    return collision_pairs;
}

void CollisionDetector::cleanup() {
    if (!initialized) {
        return;
    }
    
    collision_pairs.clear();
    initialized = false;
    std::cout << "Collision Detector cleaned up" << std::endl;
}
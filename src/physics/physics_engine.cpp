#include "physics_engine.hpp"
#include "collision_detector.hpp"
#include "../game_api.h"
#include <iostream>
#include <cmath>
#include <algorithm>

PhysicsEngine::PhysicsEngine() : 
    gravity(-9.81f),
    air_resistance(0.98f),
    ground_friction(0.9f),
    initialized(false) {
}

PhysicsEngine::~PhysicsEngine() {
    cleanup();
}

bool PhysicsEngine::initialize() {
    std::cout << "Initializing Physics Engine..." << std::endl;
    
    // Initialize collision detector
    if (!collision_detector.initialize()) {
        std::cerr << "Failed to initialize collision detector" << std::endl;
        return false;
    }
    
    // Clear rigid bodies
    rigid_bodies.clear();
    
    initialized = true;
    std::cout << "Physics Engine initialized successfully" << std::endl;
    return true;
}

void PhysicsEngine::update(float delta_time) {
    if (!initialized || delta_time <= 0.0f) {
        return;
    }
    
    // Cap delta time to prevent instability
    delta_time = std::min(delta_time, 0.033f); // Max 30 FPS equivalent
    
    // Update all rigid bodies
    for (auto& body : rigid_bodies) {
        update_rigid_body(body, delta_time);
    }
    
    // Process collisions
    collision_detector.detect_collisions(rigid_bodies);
}

void PhysicsEngine::update_rigid_body(RigidBody& body, float delta_time) {
    if (!body.active) {
        return;
    }
    
    // Apply gravity
    if (body.use_gravity && !body.kinematic) {
        body.velocity.y += gravity * delta_time;
    }
    
    // Apply air resistance
    if (body.use_air_resistance) {
        body.velocity.x *= air_resistance;
        body.velocity.z *= air_resistance;
    }
    
    // Update position based on velocity
    body.position.x += body.velocity.x * delta_time;
    body.position.y += body.velocity.y * delta_time;
    body.position.z += body.velocity.z * delta_time;
    
    // Ground collision check
    if (body.position.y - body.size.y * 0.5f <= 0.0f) {
        body.position.y = body.size.y * 0.5f;
        
        if (body.velocity.y < 0.0f) {
            body.velocity.y = 0.0f;
            body.on_ground = true;
            
            // Apply ground friction
            body.velocity.x *= ground_friction;
            body.velocity.z *= ground_friction;
        }
    } else {
        body.on_ground = false;
    }
    
    // Update bounding box
    update_bounding_box(body);
}

void PhysicsEngine::update_bounding_box(RigidBody& body) {
    body.bounding_box.min.x = body.position.x - body.size.x * 0.5f;
    body.bounding_box.min.y = body.position.y - body.size.y * 0.5f;
    body.bounding_box.min.z = body.position.z - body.size.z * 0.5f;
    
    body.bounding_box.max.x = body.position.x + body.size.x * 0.5f;
    body.bounding_box.max.y = body.position.y + body.size.y * 0.5f;
    body.bounding_box.max.z = body.position.z + body.size.z * 0.5f;
}

int PhysicsEngine::add_rigid_body(const RigidBody& body) {
    if (!initialized) {
        return -1;
    }
    
    rigid_bodies.push_back(body);
    int id = static_cast<int>(rigid_bodies.size() - 1);
    rigid_bodies[id].id = id;
    
    // Update bounding box
    update_bounding_box(rigid_bodies[id]);
    
    std::cout << "Added rigid body ID: " << id << " at position (" 
              << body.position.x << ", " << body.position.y << ", " << body.position.z << ")" << std::endl;
    
    return id;
}

RigidBody* PhysicsEngine::get_rigid_body(int id) {
    if (id >= 0 && id < static_cast<int>(rigid_bodies.size())) {
        return &rigid_bodies[id];
    }
    return nullptr;
}

void PhysicsEngine::remove_rigid_body(int id) {
    if (id >= 0 && id < static_cast<int>(rigid_bodies.size())) {
        rigid_bodies[id].active = false;
        std::cout << "Removed rigid body ID: " << id << std::endl;
    }
}

void PhysicsEngine::apply_force(int body_id, const Vector3& force) {
    RigidBody* body = get_rigid_body(body_id);
    if (body && !body->kinematic) {
        // F = ma, so a = F/m, and v += a * dt
        // For simplicity, assume mass = 1, so acceleration = force
        body->velocity.x += force.x;
        body->velocity.y += force.y;
        body->velocity.z += force.z;
    }
}

void PhysicsEngine::apply_impulse(int body_id, const Vector3& impulse) {
    RigidBody* body = get_rigid_body(body_id);
    if (body && !body->kinematic) {
        // Impulse directly changes velocity
        body->velocity.x += impulse.x;
        body->velocity.y += impulse.y;
        body->velocity.z += impulse.z;
    }
}

void PhysicsEngine::set_velocity(int body_id, const Vector3& velocity) {
    RigidBody* body = get_rigid_body(body_id);
    if (body) {
        body->velocity = velocity;
    }
}

void PhysicsEngine::set_position(int body_id, const Vector3& position) {
    RigidBody* body = get_rigid_body(body_id);
    if (body) {
        body->position = position;
        update_bounding_box(*body);
    }
}

bool PhysicsEngine::raycast(const Vector3& origin, const Vector3& direction, float max_distance, RaycastHit& hit) {
    if (!initialized) {
        return false;
    }
    
    float closest_distance = max_distance;
    bool hit_found = false;
    
    for (const auto& body : rigid_bodies) {
        if (!body.active) continue;
        
        // Simple AABB raycast
        float t_min = 0.0f;
        float t_max = max_distance;
        
        // Check intersection with each axis
        for (int i = 0; i < 3; i++) {
            float origin_component = (&origin.x)[i];
            float dir_component = (&direction.x)[i];
            float box_min = (&body.bounding_box.min.x)[i];
            float box_max = (&body.bounding_box.max.x)[i];
            
            if (std::abs(dir_component) < 1e-6f) {
                // Ray is parallel to the slab
                if (origin_component < box_min || origin_component > box_max) {
                    continue; // No intersection
                }
            } else {
                float t1 = (box_min - origin_component) / dir_component;
                float t2 = (box_max - origin_component) / dir_component;
                
                if (t1 > t2) std::swap(t1, t2);
                
                t_min = std::max(t_min, t1);
                t_max = std::min(t_max, t2);
                
                if (t_min > t_max) {
                    continue; // No intersection
                }
            }
        }
        
        if (t_min >= 0.0f && t_min < closest_distance) {
            closest_distance = t_min;
            hit_found = true;
            
            hit.distance = t_min;
            hit.point.x = origin.x + direction.x * t_min;
            hit.point.y = origin.y + direction.y * t_min;
            hit.point.z = origin.z + direction.z * t_min;
            hit.body_id = body.id;
            
            // Calculate normal (simplified - just use the closest face)
            Vector3 center = body.position;
            Vector3 hit_to_center = {center.x - hit.point.x, center.y - hit.point.y, center.z - hit.point.z};
            
            // Find the axis with the largest component
            if (std::abs(hit_to_center.x) > std::abs(hit_to_center.y) && std::abs(hit_to_center.x) > std::abs(hit_to_center.z)) {
                hit.normal = {hit_to_center.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f};
            } else if (std::abs(hit_to_center.y) > std::abs(hit_to_center.z)) {
                hit.normal = {0.0f, hit_to_center.y > 0 ? 1.0f : -1.0f, 0.0f};
            } else {
                hit.normal = {0.0f, 0.0f, hit_to_center.z > 0 ? 1.0f : -1.0f};
            }
        }
    }
    
    return hit_found;
}

float PhysicsEngine::calculate_player_speed(const PlayerState& player) {
    return sqrtf(player.velocity.x * player.velocity.x + 
                player.velocity.y * player.velocity.y + 
                player.velocity.z * player.velocity.z);
}

void PhysicsEngine::set_gravity(float new_gravity) {
    gravity = new_gravity;
    std::cout << "Gravity set to: " << gravity << std::endl;
}

void PhysicsEngine::set_air_resistance(float resistance) {
    if (resistance >= 0.0f && resistance <= 1.0f) {
        air_resistance = resistance;
        std::cout << "Air resistance set to: " << air_resistance << std::endl;
    }
}

void PhysicsEngine::set_ground_friction(float friction) {
    if (friction >= 0.0f && friction <= 1.0f) {
        ground_friction = friction;
        std::cout << "Ground friction set to: " << ground_friction << std::endl;
    }
}

int PhysicsEngine::get_rigid_body_count() const {
    int count = 0;
    for (const auto& body : rigid_bodies) {
        if (body.active) count++;
    }
    return count;
}

void PhysicsEngine::cleanup() {
    if (!initialized) {
        return;
    }
    
    std::cout << "Cleaning up Physics Engine..." << std::endl;
    
    collision_detector.cleanup();
    rigid_bodies.clear();
    
    initialized = false;
    std::cout << "Physics Engine cleaned up" << std::endl;
}
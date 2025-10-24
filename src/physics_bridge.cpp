// Bridge between C Core Engine and C++ Physics Engine
#include "physics/physics_engine.hpp"
#include "game_api.h"
#include <iostream>

// Global physics engine instance
static PhysicsEngine* g_physics_engine = nullptr;

extern "C" {

bool init_physics_engine() {
    std::cout << "Initializing Physics Bridge..." << std::endl;
    
    if (g_physics_engine) {
        std::cerr << "Physics engine already initialized!" << std::endl;
        return false;
    }
    
    g_physics_engine = new PhysicsEngine();
    
    if (!g_physics_engine->initialize()) {
        std::cerr << "Failed to initialize physics engine" << std::endl;
        delete g_physics_engine;
        g_physics_engine = nullptr;
        return false;
    }
    
    std::cout << "Physics Bridge initialized successfully" << std::endl;
    return true;
}

void update_physics(float delta_time) {
    if (g_physics_engine) {
        g_physics_engine->update(delta_time);
    }
}

int create_physics_body(Vector3 position, Vector3 size, int kinematic) {
    if (!g_physics_engine) {
        return -1;
    }
    
    RigidBody body;
    body.position = position;
    body.size = size;
    body.kinematic = (kinematic != 0);
    body.use_gravity = !body.kinematic;
    body.use_air_resistance = !body.kinematic;
    
    return g_physics_engine->add_rigid_body(body);
}

void remove_physics_body(int body_id) {
    if (g_physics_engine) {
        g_physics_engine->remove_rigid_body(body_id);
    }
}

void set_physics_position(int body_id, Vector3 position) {
    if (g_physics_engine) {
        g_physics_engine->set_position(body_id, position);
    }
}

void set_physics_velocity(int body_id, Vector3 velocity) {
    if (g_physics_engine) {
        g_physics_engine->set_velocity(body_id, velocity);
    }
}

void apply_physics_force(int body_id, Vector3 force) {
    if (g_physics_engine) {
        g_physics_engine->apply_force(body_id, force);
    }
}

void apply_physics_impulse(int body_id, Vector3 impulse) {
    if (g_physics_engine) {
        g_physics_engine->apply_impulse(body_id, impulse);
    }
}

int physics_raycast(Vector3 origin, Vector3 direction, float max_distance, 
                   Vector3* hit_point, Vector3* hit_normal, float* hit_distance) {
    if (!g_physics_engine) {
        return 0;
    }
    
    RaycastHit hit;
    if (g_physics_engine->raycast(origin, direction, max_distance, hit)) {
        if (hit_point) *hit_point = hit.point;
        if (hit_normal) *hit_normal = hit.normal;
        if (hit_distance) *hit_distance = hit.distance;
        return hit.body_id;
    }
    
    return -1;
}

float calculate_physics_speed(const PlayerState* player) {
    if (g_physics_engine && player) {
        return g_physics_engine->calculate_player_speed(*player);
    }
    return 0.0f;
}

void set_physics_gravity(float gravity) {
    if (g_physics_engine) {
        g_physics_engine->set_gravity(gravity);
    }
}

int get_physics_body_count() {
    if (g_physics_engine) {
        return g_physics_engine->get_rigid_body_count();
    }
    return 0;
}

void apply_bunny_hop_movement(PlayerState* player, const InputState* input, float delta_time) {
    if (g_physics_engine && player && input) {
        g_physics_engine->apply_bunny_hop(*player, *input, delta_time);
    }
}

void set_bunny_hop_max_ground_speed(float speed) {
    if (g_physics_engine) {
        BunnyHopController* controller = g_physics_engine->get_bunny_hop_controller();
        if (controller) {
            controller->set_max_ground_speed(speed);
        }
    }
}

void set_bunny_hop_max_air_speed(float speed) {
    if (g_physics_engine) {
        BunnyHopController* controller = g_physics_engine->get_bunny_hop_controller();
        if (controller) {
            controller->set_max_air_speed(speed);
        }
    }
}

float get_bunny_hop_max_ground_speed() {
    if (g_physics_engine) {
        BunnyHopController* controller = g_physics_engine->get_bunny_hop_controller();
        if (controller) {
            return controller->get_max_ground_speed();
        }
    }
    return 0.0f;
}

float get_bunny_hop_max_air_speed() {
    if (g_physics_engine) {
        BunnyHopController* controller = g_physics_engine->get_bunny_hop_controller();
        if (controller) {
            return controller->get_max_air_speed();
        }
    }
    return 0.0f;
}

void cleanup_physics_engine() {
    std::cout << "Cleaning up Physics Bridge..." << std::endl;
    
    if (g_physics_engine) {
        g_physics_engine->cleanup();
        delete g_physics_engine;
        g_physics_engine = nullptr;
    }
    
    std::cout << "Physics Bridge cleaned up" << std::endl;
}

} // extern "C"
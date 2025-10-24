#ifndef PHYSICS_BRIDGE_H
#define PHYSICS_BRIDGE_H

#include "game_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// Physics engine bridge functions
bool init_physics_engine();
void update_physics(float delta_time);
void cleanup_physics_engine();

// Rigid body management
int create_physics_body(Vector3 position, Vector3 size, int kinematic);
void remove_physics_body(int body_id);

// Physics operations
void set_physics_position(int body_id, Vector3 position);
void set_physics_velocity(int body_id, Vector3 velocity);
void apply_physics_force(int body_id, Vector3 force);
void apply_physics_impulse(int body_id, Vector3 impulse);

// Raycasting
int physics_raycast(Vector3 origin, Vector3 direction, float max_distance, 
                   Vector3* hit_point, Vector3* hit_normal, float* hit_distance);

// Utility functions
float calculate_physics_speed(const PlayerState* player);

// Settings
void set_physics_gravity(float gravity);

// Query functions
int get_physics_body_count();

#ifdef __cplusplus
}
#endif

#endif // PHYSICS_BRIDGE_H
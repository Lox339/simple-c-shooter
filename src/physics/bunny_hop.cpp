#include "bunny_hop.hpp"
#include "../game_api.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

BunnyHopController::BunnyHopController() :
    max_ground_speed(10.0f),
    max_air_speed(30.0f),
    ground_acceleration(8.0f),
    air_acceleration(2.0f),
    ground_friction(6.0f),
    air_friction(0.1f),
    jump_velocity(8.0f),
    gravity(-9.81f),
    ground_threshold(0.1f),
    strafe_angle_threshold(45.0f),
    speed_gain_factor(1.2f),
    initialized(false) {
}

BunnyHopController::~BunnyHopController() {
    cleanup();
}

bool BunnyHopController::initialize() {
    std::cout << "Initializing Bunny Hop Controller..." << std::endl;
    std::cout << "Max ground speed: " << max_ground_speed << " u/s" << std::endl;
    std::cout << "Max air speed: " << max_air_speed << " u/s" << std::endl;
    std::cout << "Jump velocity: " << jump_velocity << " u/s" << std::endl;
    
    initialized = true;
    std::cout << "Bunny Hop Controller initialized successfully" << std::endl;
    return true;
}

void BunnyHopController::update_movement(PlayerState& player, const InputState& input, float delta_time) {
    if (!initialized || delta_time <= 0.0f) {
        return;
    }
    
    // Cap delta time for stability
    delta_time = std::min(delta_time, 0.033f);
    
    // Check if player is on ground
    bool was_on_ground = player.on_ground;
    player.on_ground = is_on_ground(player);
    
    // Handle jumping
    if (input.jump_pressed && player.on_ground) {
        perform_jump(player);
    }
    
    // Update movement based on ground state
    if (player.on_ground) {
        update_ground_movement(player, input, delta_time);
    } else {
        update_air_movement(player, input, delta_time);
    }
    
    // Apply gravity
    if (!player.on_ground) {
        player.velocity.y += gravity * delta_time;
    }
    
    // Update position
    player.position.x += player.velocity.x * delta_time;
    player.position.y += player.velocity.y * delta_time;
    player.position.z += player.velocity.z * delta_time;
    
    // Ground collision
    if (player.position.y <= 0.0f) {
        player.position.y = 0.0f;
        if (player.velocity.y < 0.0f) {
            player.velocity.y = 0.0f;
            
            // Landing detection for bunny hop
            if (!was_on_ground) {
                on_landing(player);
            }
        }
    }
    
    // Update speed calculations
    update_speed_calculations(player);
}

bool BunnyHopController::is_on_ground(const PlayerState& player) {
    return player.position.y <= ground_threshold && std::abs(player.velocity.y) < 0.1f;
}

void BunnyHopController::perform_jump(PlayerState& player) {
    player.velocity.y = jump_velocity;
    player.on_ground = false;
    player.last_jump_time = 0.0f;
    player.consecutive_jumps++;
    
    std::cout << "Jump! Consecutive: " << player.consecutive_jumps 
              << ", Speed: " << player.speed << " u/s" << std::endl;
}

void BunnyHopController::update_ground_movement(PlayerState& player, const InputState& input, float delta_time) {
    // Calculate input direction
    Vector3 input_dir = calculate_input_direction(input, player.rotation.y);
    
    // Calculate target velocity
    Vector3 target_velocity = {
        input_dir.x * max_ground_speed,
        player.velocity.y,
        input_dir.z * max_ground_speed
    };
    
    // Apply acceleration towards target velocity
    Vector3 velocity_diff = {
        target_velocity.x - player.velocity.x,
        0.0f,
        target_velocity.z - player.velocity.z
    };
    
    float acceleration = ground_acceleration * delta_time;
    
    // Limit acceleration
    float diff_length = sqrtf(velocity_diff.x * velocity_diff.x + velocity_diff.z * velocity_diff.z);
    if (diff_length > acceleration) {
        velocity_diff.x = (velocity_diff.x / diff_length) * acceleration;
        velocity_diff.z = (velocity_diff.z / diff_length) * acceleration;
    }
    
    player.velocity.x += velocity_diff.x;
    player.velocity.z += velocity_diff.z;
    
    // Apply ground friction when no input
    if (input_dir.x == 0.0f && input_dir.z == 0.0f) {
        float friction = ground_friction * delta_time;
        
        float current_speed = sqrtf(player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z);
        if (current_speed > friction) {
            float friction_factor = 1.0f - (friction / current_speed);
            player.velocity.x *= friction_factor;
            player.velocity.z *= friction_factor;
        } else {
            player.velocity.x = 0.0f;
            player.velocity.z = 0.0f;
        }
    }
}

void BunnyHopController::update_air_movement(PlayerState& player, const InputState& input, float delta_time) {
    // Calculate input direction
    Vector3 input_dir = calculate_input_direction(input, player.rotation.y);
    
    if (input_dir.x == 0.0f && input_dir.z == 0.0f) {
        // No input - apply minimal air friction
        player.velocity.x *= (1.0f - air_friction * delta_time);
        player.velocity.z *= (1.0f - air_friction * delta_time);
        return;
    }
    
    // Current horizontal velocity
    Vector3 current_vel = {player.velocity.x, 0.0f, player.velocity.z};
    float current_speed = sqrtf(current_vel.x * current_vel.x + current_vel.z * current_vel.z);
    
    // Calculate strafe angle for bunny hopping
    float strafe_angle = calculate_strafe_angle(current_vel, input_dir);
    
    // Determine if this is a good bunny hop strafe
    bool is_good_strafe = std::abs(strafe_angle) > strafe_angle_threshold && 
                         std::abs(strafe_angle) < (90.0f - strafe_angle_threshold);
    
    // Calculate acceleration
    float acceleration = air_acceleration * delta_time;
    
    // Bunny hop speed gain
    if (is_good_strafe && current_speed > max_ground_speed) {
        acceleration *= speed_gain_factor;
        
        // Additional speed gain based on strafe quality
        float strafe_quality = 1.0f - (std::abs(strafe_angle - 45.0f) / 45.0f);
        acceleration *= (1.0f + strafe_quality * 0.5f);
    }
    
    // Apply acceleration in input direction
    Vector3 accel_vector = {
        input_dir.x * acceleration,
        0.0f,
        input_dir.z * acceleration
    };
    
    // Add acceleration to velocity
    player.velocity.x += accel_vector.x;
    player.velocity.z += accel_vector.z;
    
    // Cap maximum air speed
    float new_speed = sqrtf(player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z);
    if (new_speed > max_air_speed) {
        float speed_ratio = max_air_speed / new_speed;
        player.velocity.x *= speed_ratio;
        player.velocity.z *= speed_ratio;
    }
    
    // Debug output for good strafes
    if (is_good_strafe && current_speed > max_ground_speed) {
        std::cout << "Good strafe! Angle: " << strafe_angle 
                  << "°, Speed: " << new_speed << " u/s" << std::endl;
    }
}

Vector3 BunnyHopController::calculate_input_direction(const InputState& input, float yaw_degrees) {
    Vector3 direction = {0.0f, 0.0f, 0.0f};
    
    // Convert yaw to radians
    float yaw_rad = yaw_degrees * M_PI / 180.0f;
    
    // Calculate forward and right vectors
    Vector3 forward = {sinf(yaw_rad), 0.0f, cosf(yaw_rad)};
    Vector3 right = {cosf(yaw_rad), 0.0f, -sinf(yaw_rad)};
    
    // WASD input
    if (input.keys['w']) {
        direction.x += forward.x;
        direction.z += forward.z;
    }
    if (input.keys['s']) {
        direction.x -= forward.x;
        direction.z -= forward.z;
    }
    if (input.keys['a']) {
        direction.x -= right.x;
        direction.z -= right.z;
    }
    if (input.keys['d']) {
        direction.x += right.x;
        direction.z += right.z;
    }
    
    // Normalize direction
    float length = sqrtf(direction.x * direction.x + direction.z * direction.z);
    if (length > 0.0f) {
        direction.x /= length;
        direction.z /= length;
    }
    
    return direction;
}

float BunnyHopController::calculate_strafe_angle(const Vector3& velocity, const Vector3& input_dir) {
    if (velocity.x == 0.0f && velocity.z == 0.0f) {
        return 0.0f;
    }
    
    if (input_dir.x == 0.0f && input_dir.z == 0.0f) {
        return 0.0f;
    }
    
    // Calculate angle between velocity and input direction
    float dot_product = velocity.x * input_dir.x + velocity.z * input_dir.z;
    float vel_length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    float input_length = sqrtf(input_dir.x * input_dir.x + input_dir.z * input_dir.z);
    
    float cos_angle = dot_product / (vel_length * input_length);
    cos_angle = std::max(-1.0f, std::min(1.0f, cos_angle)); // Clamp to valid range
    
    float angle_rad = acosf(cos_angle);
    return angle_rad * 180.0f / M_PI;
}

float BunnyHopController::calculate_speed_gain(float current_speed, float input_angle) {
    if (current_speed < max_ground_speed) {
        return 0.0f;
    }
    
    // Optimal strafe angle is around 45 degrees
    float optimal_angle = 45.0f;
    float angle_diff = std::abs(input_angle - optimal_angle);
    
    if (angle_diff > optimal_angle) {
        return 0.0f;
    }
    
    // Calculate speed gain based on angle quality
    float angle_quality = 1.0f - (angle_diff / optimal_angle);
    float base_gain = 0.1f; // Base speed gain per frame
    
    return base_gain * angle_quality * speed_gain_factor;
}

void BunnyHopController::on_landing(PlayerState& player) {
    // Preserve horizontal speed on landing for bunny hopping
    float horizontal_speed = sqrtf(player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z);
    
    if (horizontal_speed > max_ground_speed) {
        std::cout << "Bunny hop landing! Speed preserved: " << horizontal_speed << " u/s" << std::endl;
        
        // Slightly reduce speed on landing but preserve most of it
        float preservation_factor = 0.95f;
        player.velocity.x *= preservation_factor;
        player.velocity.z *= preservation_factor;
    }
}

void BunnyHopController::update_speed_calculations(PlayerState& player) {
    // Calculate horizontal speed
    player.horizontal_speed = sqrtf(player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z);
    
    // Calculate total speed
    player.speed = sqrtf(player.velocity.x * player.velocity.x + 
                        player.velocity.y * player.velocity.y + 
                        player.velocity.z * player.velocity.z);
    
    // Reset consecutive jumps if on ground for too long
    if (player.on_ground) {
        player.last_jump_time += 0.016f; // Assume ~60 FPS
        if (player.last_jump_time > 1.0f) {
            player.consecutive_jumps = 0;
        }
    }
}

// Configuration setters
void BunnyHopController::set_max_ground_speed(float speed) {
    if (speed > 0.0f) {
        max_ground_speed = speed;
        std::cout << "Max ground speed set to: " << speed << std::endl;
    }
}

void BunnyHopController::set_max_air_speed(float speed) {
    if (speed > 0.0f) {
        max_air_speed = speed;
        std::cout << "Max air speed set to: " << speed << std::endl;
    }
}

void BunnyHopController::set_jump_velocity(float velocity) {
    if (velocity > 0.0f) {
        jump_velocity = velocity;
        std::cout << "Jump velocity set to: " << velocity << std::endl;
    }
}

void BunnyHopController::set_speed_gain_factor(float factor) {
    if (factor > 0.0f) {
        speed_gain_factor = factor;
        std::cout << "Speed gain factor set to: " << factor << std::endl;
    }
}

// Getters
float BunnyHopController::get_max_ground_speed() const { return max_ground_speed; }
float BunnyHopController::get_max_air_speed() const { return max_air_speed; }
float BunnyHopController::get_jump_velocity() const { return jump_velocity; }
float BunnyHopController::get_speed_gain_factor() const { return speed_gain_factor; }

void BunnyHopController::cleanup() {
    if (!initialized) {
        return;
    }
    
    initialized = false;
    std::cout << "Bunny Hop Controller cleaned up" << std::endl;
}
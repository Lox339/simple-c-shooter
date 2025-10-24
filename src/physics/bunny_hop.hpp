#ifndef BUNNY_HOP_HPP
#define BUNNY_HOP_HPP

#include "../game_api.h"

class BunnyHopController {
private:
    // Movement parameters
    float max_ground_speed;      // Maximum speed on ground
    float max_air_speed;         // Maximum speed in air (for bunny hopping)
    float ground_acceleration;   // Acceleration on ground
    float air_acceleration;      // Acceleration in air
    float ground_friction;       // Friction when on ground
    float air_friction;          // Friction when in air
    float jump_velocity;         // Initial jump velocity
    float gravity;               // Gravity acceleration
    
    // Bunny hop parameters
    float ground_threshold;      // Distance from ground to be considered "on ground"
    float strafe_angle_threshold; // Minimum angle for effective strafing
    float speed_gain_factor;     // Multiplier for speed gain during bunny hopping
    
    bool initialized;
    
    // Helper methods
    Vector3 calculate_input_direction(const InputState& input, float yaw_degrees);
    float calculate_strafe_angle(const Vector3& velocity, const Vector3& input_dir);
    void update_ground_movement(PlayerState& player, const InputState& input, float delta_time);
    void update_air_movement(PlayerState& player, const InputState& input, float delta_time);
    void on_landing(PlayerState& player);
    void update_speed_calculations(PlayerState& player);

public:
    BunnyHopController();
    ~BunnyHopController();
    
    bool initialize();
    void cleanup();
    
    // Main update function
    void update_movement(PlayerState& player, const InputState& input, float delta_time);
    
    // Movement checks
    bool is_on_ground(const PlayerState& player);
    void perform_jump(PlayerState& player);
    
    // Speed calculations
    float calculate_speed_gain(float current_speed, float input_angle);
    
    // Configuration
    void set_max_ground_speed(float speed);
    void set_max_air_speed(float speed);
    void set_jump_velocity(float velocity);
    void set_speed_gain_factor(float factor);
    
    // Getters
    float get_max_ground_speed() const;
    float get_max_air_speed() const;
    float get_jump_velocity() const;
    float get_speed_gain_factor() const;
};

#endif // BUNNY_HOP_HPP
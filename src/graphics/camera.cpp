#include "camera.hpp"
#include "renderer.hpp"
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera() :
    position_x(0.0f), position_y(0.0f), position_z(0.0f),
    pitch(0.0f), yaw(0.0f), roll(0.0f),
    fov(75.0f), near_plane(0.1f), far_plane(100.0f) {
}

void Camera::initialize() {
    std::cout << "Camera initialized - FOV: " << fov << " degrees" << std::endl;
}

void Camera::set_position(float x, float y, float z) {
    position_x = x;
    position_y = y;
    position_z = z;
}

void Camera::set_rotation(float pitch_deg, float yaw_deg, float roll_deg) {
    pitch = pitch_deg;
    yaw = yaw_deg;
    roll = roll_deg;
    
    // Clamp pitch to prevent gimbal lock
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void Camera::move_forward(float distance) {
    float yaw_rad = yaw * M_PI / 180.0f;
    position_x += sinf(yaw_rad) * distance;
    position_z += cosf(yaw_rad) * distance;
}

void Camera::move_right(float distance) {
    float yaw_rad = yaw * M_PI / 180.0f;
    position_x += cosf(yaw_rad) * distance;
    position_z -= sinf(yaw_rad) * distance;
}

void Camera::move_up(float distance) {
    position_y += distance;
}

Matrix4 Camera::get_view_matrix() const {
    // Calculate forward vector from pitch and yaw
    float pitch_rad = pitch * M_PI / 180.0f;
    float yaw_rad = yaw * M_PI / 180.0f;
    
    float forward_x = sinf(yaw_rad) * cosf(pitch_rad);
    float forward_y = -sinf(pitch_rad);
    float forward_z = cosf(yaw_rad) * cosf(pitch_rad);
    
    // Calculate target position
    float target_x = position_x + forward_x;
    float target_y = position_y + forward_y;
    float target_z = position_z + forward_z;
    
    // Up vector (with roll support)
    float roll_rad = roll * M_PI / 180.0f;
    float up_x = -sinf(roll_rad);
    float up_y = cosf(roll_rad);
    float up_z = 0.0f;
    
    return create_look_at_matrix(position_x, position_y, position_z,
                                target_x, target_y, target_z,
                                up_x, up_y, up_z);
}

Matrix4 Camera::get_projection_matrix(int screen_width, int screen_height) const {
    float aspect_ratio = (float)screen_width / (float)screen_height;
    return create_perspective_matrix(fov, aspect_ratio, near_plane, far_plane);
}

void Camera::set_fov(float fov_degrees) {
    if (fov_degrees > 10.0f && fov_degrees < 120.0f) {
        fov = fov_degrees;
    }
}

void Camera::set_clipping_planes(float near, float far) {
    if (near > 0.0f && far > near) {
        near_plane = near;
        far_plane = far;
    }
}

void Camera::cleanup() {
    std::cout << "Camera cleaned up" << std::endl;
}
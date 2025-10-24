#ifndef CAMERA_HPP
#define CAMERA_HPP

// Forward declaration
struct Matrix4;

class Camera {
private:
    float position_x, position_y, position_z;
    float pitch, yaw, roll;  // Rotation in degrees
    float fov;               // Field of view in degrees
    float near_plane, far_plane;

public:
    Camera();
    
    void initialize();
    void cleanup();
    
    // Position and rotation
    void set_position(float x, float y, float z);
    void set_rotation(float pitch, float yaw, float roll = 0.0f);
    
    // Movement
    void move_forward(float distance);
    void move_right(float distance);
    void move_up(float distance);
    
    // Matrix generation
    Matrix4 get_view_matrix() const;
    Matrix4 get_projection_matrix(int screen_width, int screen_height) const;
    
    // Settings
    void set_fov(float fov_degrees);
    void set_clipping_planes(float near, float far);
    
    // Getters
    float get_position_x() const { return position_x; }
    float get_position_y() const { return position_y; }
    float get_position_z() const { return position_z; }
    float get_pitch() const { return pitch; }
    float get_yaw() const { return yaw; }
    float get_roll() const { return roll; }
    float get_fov() const { return fov; }
};

#endif // CAMERA_HPP
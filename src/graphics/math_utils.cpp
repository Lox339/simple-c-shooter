#include "renderer.hpp"
#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Matrix4 constructors
Matrix4::Matrix4() {
    memset(data, 0, sizeof(data));
}

Matrix4::Matrix4(float diagonal) {
    memset(data, 0, sizeof(data));
    data[0] = diagonal;   // [0][0]
    data[5] = diagonal;   // [1][1]
    data[10] = diagonal;  // [2][2]
    data[15] = diagonal;  // [3][3]
}

// Matrix operations
Matrix4 create_identity_matrix() {
    return Matrix4(1.0f);
}

Matrix4 create_translation_matrix(float x, float y, float z) {
    Matrix4 result(1.0f);
    result.data[12] = x;  // [3][0]
    result.data[13] = y;  // [3][1]
    result.data[14] = z;  // [3][2]
    return result;
}

Matrix4 create_scale_matrix(float x, float y, float z) {
    Matrix4 result;
    result.data[0] = x;   // [0][0]
    result.data[5] = y;   // [1][1]
    result.data[10] = z;  // [2][2]
    result.data[15] = 1.0f; // [3][3]
    return result;
}

Matrix4 create_rotation_matrix_x(float angle) {
    Matrix4 result(1.0f);
    float rad = angle * M_PI / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);
    
    result.data[5] = cos_a;   // [1][1]
    result.data[6] = -sin_a;  // [1][2]
    result.data[9] = sin_a;   // [2][1]
    result.data[10] = cos_a;  // [2][2]
    
    return result;
}

Matrix4 create_rotation_matrix_y(float angle) {
    Matrix4 result(1.0f);
    float rad = angle * M_PI / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);
    
    result.data[0] = cos_a;   // [0][0]
    result.data[2] = sin_a;   // [0][2]
    result.data[8] = -sin_a;  // [2][0]
    result.data[10] = cos_a;  // [2][2]
    
    return result;
}

Matrix4 create_rotation_matrix_z(float angle) {
    Matrix4 result(1.0f);
    float rad = angle * M_PI / 180.0f;
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);
    
    result.data[0] = cos_a;   // [0][0]
    result.data[1] = -sin_a;  // [0][1]
    result.data[4] = sin_a;   // [1][0]
    result.data[5] = cos_a;   // [1][1]
    
    return result;
}

Matrix4 multiply_matrices(const Matrix4& a, const Matrix4& b) {
    Matrix4 result;
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += a.data[row * 4 + k] * b.data[k * 4 + col];
            }
            result.data[row * 4 + col] = sum;
        }
    }
    
    return result;
}

Matrix4 create_perspective_matrix(float fov, float aspect, float near, float far) {
    Matrix4 result;
    
    float fov_rad = fov * M_PI / 180.0f;
    float tan_half_fov = tanf(fov_rad / 2.0f);
    
    result.data[0] = 1.0f / (aspect * tan_half_fov);  // [0][0]
    result.data[5] = 1.0f / tan_half_fov;             // [1][1]
    result.data[10] = -(far + near) / (far - near);   // [2][2]
    result.data[11] = -1.0f;                          // [2][3]
    result.data[14] = -(2.0f * far * near) / (far - near); // [3][2]
    
    return result;
}

Matrix4 create_look_at_matrix(float eye_x, float eye_y, float eye_z,
                             float center_x, float center_y, float center_z,
                             float up_x, float up_y, float up_z) {
    // Calculate forward vector (from eye to center)
    float forward_x = center_x - eye_x;
    float forward_y = center_y - eye_y;
    float forward_z = center_z - eye_z;
    
    // Normalize forward vector
    float forward_length = sqrtf(forward_x * forward_x + forward_y * forward_y + forward_z * forward_z);
    if (forward_length > 0.0f) {
        forward_x /= forward_length;
        forward_y /= forward_length;
        forward_z /= forward_length;
    }
    
    // Calculate right vector (cross product of forward and up)
    float right_x = forward_y * up_z - forward_z * up_y;
    float right_y = forward_z * up_x - forward_x * up_z;
    float right_z = forward_x * up_y - forward_y * up_x;
    
    // Normalize right vector
    float right_length = sqrtf(right_x * right_x + right_y * right_y + right_z * right_z);
    if (right_length > 0.0f) {
        right_x /= right_length;
        right_y /= right_length;
        right_z /= right_length;
    }
    
    // Calculate actual up vector (cross product of right and forward)
    float actual_up_x = right_y * forward_z - right_z * forward_y;
    float actual_up_y = right_z * forward_x - right_x * forward_z;
    float actual_up_z = right_x * forward_y - right_y * forward_x;
    
    Matrix4 result(1.0f);
    
    // Set rotation part
    result.data[0] = right_x;     // [0][0]
    result.data[4] = right_y;     // [1][0]
    result.data[8] = right_z;     // [2][0]
    
    result.data[1] = actual_up_x; // [0][1]
    result.data[5] = actual_up_y; // [1][1]
    result.data[9] = actual_up_z; // [2][1]
    
    result.data[2] = -forward_x;  // [0][2]
    result.data[6] = -forward_y;  // [1][2]
    result.data[10] = -forward_z; // [2][2]
    
    // Set translation part
    result.data[12] = -(right_x * eye_x + right_y * eye_y + right_z * eye_z);       // [3][0]
    result.data[13] = -(actual_up_x * eye_x + actual_up_y * eye_y + actual_up_z * eye_z); // [3][1]
    result.data[14] = -(-forward_x * eye_x + -forward_y * eye_y + -forward_z * eye_z);    // [3][2]
    
    return result;
}
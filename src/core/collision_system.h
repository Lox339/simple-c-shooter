#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "game_api.h"

// Collision shapes
typedef enum {
    COLLISION_SPHERE,
    COLLISION_BOX,
    COLLISION_CAPSULE
} CollisionShape;

// Collision data
typedef struct {
    Vector3 center;
    float radius;        // For sphere and capsule
    Vector3 half_extents; // For box
    float height;        // For capsule
    CollisionShape shape;
} CollisionVolume;

// Collision result
typedef struct {
    int hit;
    Vector3 hit_point;
    Vector3 hit_normal;
    float distance;
    float penetration_depth;
} CollisionResult;

// Damage information
typedef struct {
    float amount;
    Vector3 hit_point;
    Vector3 hit_direction;
    int damage_type; // 0 = bullet, 1 = explosion, 2 = melee
} DamageInfo;

// Collision system functions
void init_collision_system();
void cleanup_collision_system();

// Collision detection
CollisionResult check_sphere_sphere_collision(Vector3 pos1, float radius1, Vector3 pos2, float radius2);
CollisionResult check_sphere_box_collision(Vector3 sphere_pos, float radius, Vector3 box_pos, Vector3 box_half_extents);
CollisionResult check_ray_sphere_collision(Vector3 ray_origin, Vector3 ray_direction, Vector3 sphere_pos, float radius);
CollisionResult check_ray_box_collision(Vector3 ray_origin, Vector3 ray_direction, Vector3 box_pos, Vector3 box_half_extents);

// Game-specific collision checks
int check_projectile_enemy_collision(const Projectile* projectile, const Enemy* enemy, CollisionResult* result);
int check_projectile_player_collision(const Projectile* projectile, const PlayerState* player, CollisionResult* result);
int check_enemy_player_collision(const Enemy* enemy, const PlayerState* player, CollisionResult* result);

// Damage system
void apply_damage_to_enemy(Enemy* enemy, const DamageInfo* damage);
void apply_damage_to_player(PlayerState* player, const DamageInfo* damage);
void create_damage_effects(const DamageInfo* damage);

// Collision volumes for game objects
CollisionVolume get_player_collision_volume(const PlayerState* player);
CollisionVolume get_enemy_collision_volume(const Enemy* enemy);
CollisionVolume get_projectile_collision_volume(const Projectile* projectile);

// Utility functions
float vector3_distance(Vector3 a, Vector3 b);
float vector3_distance_squared(Vector3 a, Vector3 b);
Vector3 vector3_normalize(Vector3 v);
Vector3 vector3_subtract(Vector3 a, Vector3 b);
Vector3 vector3_add(Vector3 a, Vector3 b);
float vector3_dot(Vector3 a, Vector3 b);
float vector3_length(Vector3 v);

#endif // COLLISION_SYSTEM_H
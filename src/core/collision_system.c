#include "collision_system.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void init_collision_system() {
    printf("Collision system initialized\n");
}

void cleanup_collision_system() {
    printf("Collision system cleaned up\n");
}

// Utility functions
float vector3_distance(Vector3 a, Vector3 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

float vector3_distance_squared(Vector3 a, Vector3 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return dx*dx + dy*dy + dz*dz;
}

Vector3 vector3_normalize(Vector3 v) {
    float length = vector3_length(v);
    if (length > 0.0f) {
        return (Vector3){v.x / length, v.y / length, v.z / length};
    }
    return (Vector3){0.0f, 0.0f, 0.0f};
}

Vector3 vector3_subtract(Vector3 a, Vector3 b) {
    return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 vector3_add(Vector3 a, Vector3 b) {
    return (Vector3){a.x + b.x, a.y + b.y, a.z + b.z};
}

float vector3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vector3_length(Vector3 v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

// Collision detection functions
CollisionResult check_sphere_sphere_collision(Vector3 pos1, float radius1, Vector3 pos2, float radius2) {
    CollisionResult result = {0};
    
    float distance = vector3_distance(pos1, pos2);
    float combined_radius = radius1 + radius2;
    
    if (distance <= combined_radius) {
        result.hit = 1;
        result.distance = distance;
        result.penetration_depth = combined_radius - distance;
        
        // Calculate hit point (on the surface of first sphere)
        Vector3 direction = vector3_normalize(vector3_subtract(pos2, pos1));
        result.hit_point = vector3_add(pos1, (Vector3){direction.x * radius1, direction.y * radius1, direction.z * radius1});
        result.hit_normal = direction;
    }
    
    return result;
}

CollisionResult check_sphere_box_collision(Vector3 sphere_pos, float radius, Vector3 box_pos, Vector3 box_half_extents) {
    CollisionResult result = {0};
    
    // Find closest point on box to sphere center
    Vector3 closest_point;
    closest_point.x = fmaxf(box_pos.x - box_half_extents.x, fminf(sphere_pos.x, box_pos.x + box_half_extents.x));
    closest_point.y = fmaxf(box_pos.y - box_half_extents.y, fminf(sphere_pos.y, box_pos.y + box_half_extents.y));
    closest_point.z = fmaxf(box_pos.z - box_half_extents.z, fminf(sphere_pos.z, box_pos.z + box_half_extents.z));
    
    float distance = vector3_distance(sphere_pos, closest_point);
    
    if (distance <= radius) {
        result.hit = 1;
        result.distance = distance;
        result.penetration_depth = radius - distance;
        result.hit_point = closest_point;
        
        if (distance > 0.0f) {
            result.hit_normal = vector3_normalize(vector3_subtract(sphere_pos, closest_point));
        } else {
            // Sphere center is inside box, use closest axis
            Vector3 to_center = vector3_subtract(sphere_pos, box_pos);
            if (fabsf(to_center.x) > fabsf(to_center.y) && fabsf(to_center.x) > fabsf(to_center.z)) {
                result.hit_normal = (Vector3){to_center.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f};
            } else if (fabsf(to_center.y) > fabsf(to_center.z)) {
                result.hit_normal = (Vector3){0.0f, to_center.y > 0 ? 1.0f : -1.0f, 0.0f};
            } else {
                result.hit_normal = (Vector3){0.0f, 0.0f, to_center.z > 0 ? 1.0f : -1.0f};
            }
        }
    }
    
    return result;
}

CollisionResult check_ray_sphere_collision(Vector3 ray_origin, Vector3 ray_direction, Vector3 sphere_pos, float radius) {
    CollisionResult result = {0};
    
    Vector3 oc = vector3_subtract(ray_origin, sphere_pos);
    float a = vector3_dot(ray_direction, ray_direction);
    float b = 2.0f * vector3_dot(oc, ray_direction);
    float c = vector3_dot(oc, oc) - radius * radius;
    
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant >= 0) {
        float sqrt_discriminant = sqrtf(discriminant);
        float t1 = (-b - sqrt_discriminant) / (2.0f * a);
        float t2 = (-b + sqrt_discriminant) / (2.0f * a);
        
        float t = (t1 >= 0) ? t1 : t2;
        
        if (t >= 0) {
            result.hit = 1;
            result.distance = t;
            result.hit_point = vector3_add(ray_origin, (Vector3){ray_direction.x * t, ray_direction.y * t, ray_direction.z * t});
            result.hit_normal = vector3_normalize(vector3_subtract(result.hit_point, sphere_pos));
        }
    }
    
    return result;
}

CollisionResult check_ray_box_collision(Vector3 ray_origin, Vector3 ray_direction, Vector3 box_pos, Vector3 box_half_extents) {
    CollisionResult result = {0};
    
    Vector3 box_min = vector3_subtract(box_pos, box_half_extents);
    Vector3 box_max = vector3_add(box_pos, box_half_extents);
    
    float t_min = 0.0f;
    float t_max = INFINITY;
    
    // Check intersection with each axis
    for (int i = 0; i < 3; i++) {
        float origin_component = (i == 0) ? ray_origin.x : (i == 1) ? ray_origin.y : ray_origin.z;
        float direction_component = (i == 0) ? ray_direction.x : (i == 1) ? ray_direction.y : ray_direction.z;
        float min_component = (i == 0) ? box_min.x : (i == 1) ? box_min.y : box_min.z;
        float max_component = (i == 0) ? box_max.x : (i == 1) ? box_max.y : box_max.z;
        
        if (fabsf(direction_component) < 1e-6f) {
            // Ray is parallel to this axis
            if (origin_component < min_component || origin_component > max_component) {
                return result; // No intersection
            }
        } else {
            float t1 = (min_component - origin_component) / direction_component;
            float t2 = (max_component - origin_component) / direction_component;
            
            if (t1 > t2) {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }
            
            t_min = fmaxf(t_min, t1);
            t_max = fminf(t_max, t2);
            
            if (t_min > t_max) {
                return result; // No intersection
            }
        }
    }
    
    if (t_min >= 0) {
        result.hit = 1;
        result.distance = t_min;
        result.hit_point = vector3_add(ray_origin, (Vector3){ray_direction.x * t_min, ray_direction.y * t_min, ray_direction.z * t_min});
        
        // Calculate normal based on which face was hit
        Vector3 local_hit = vector3_subtract(result.hit_point, box_pos);
        float epsilon = 1e-4f;
        
        if (fabsf(local_hit.x - box_half_extents.x) < epsilon) result.hit_normal = (Vector3){1.0f, 0.0f, 0.0f};
        else if (fabsf(local_hit.x + box_half_extents.x) < epsilon) result.hit_normal = (Vector3){-1.0f, 0.0f, 0.0f};
        else if (fabsf(local_hit.y - box_half_extents.y) < epsilon) result.hit_normal = (Vector3){0.0f, 1.0f, 0.0f};
        else if (fabsf(local_hit.y + box_half_extents.y) < epsilon) result.hit_normal = (Vector3){0.0f, -1.0f, 0.0f};
        else if (fabsf(local_hit.z - box_half_extents.z) < epsilon) result.hit_normal = (Vector3){0.0f, 0.0f, 1.0f};
        else result.hit_normal = (Vector3){0.0f, 0.0f, -1.0f};
    }
    
    return result;
}

// Game-specific collision checks
int check_projectile_enemy_collision(const Projectile* projectile, const Enemy* enemy, CollisionResult* result) {
    if (!projectile || !enemy || !result) return 0;
    
    CollisionVolume proj_vol = get_projectile_collision_volume(projectile);
    CollisionVolume enemy_vol = get_enemy_collision_volume(enemy);
    
    *result = check_sphere_sphere_collision(proj_vol.center, proj_vol.radius, 
                                          enemy_vol.center, enemy_vol.radius);
    
    return result->hit;
}

int check_projectile_player_collision(const Projectile* projectile, const PlayerState* player, CollisionResult* result) {
    if (!projectile || !player || !result) return 0;
    
    CollisionVolume proj_vol = get_projectile_collision_volume(projectile);
    CollisionVolume player_vol = get_player_collision_volume(player);
    
    *result = check_sphere_sphere_collision(proj_vol.center, proj_vol.radius, 
                                          player_vol.center, player_vol.radius);
    
    return result->hit;
}

int check_enemy_player_collision(const Enemy* enemy, const PlayerState* player, CollisionResult* result) {
    if (!enemy || !player || !result) return 0;
    
    CollisionVolume enemy_vol = get_enemy_collision_volume(enemy);
    CollisionVolume player_vol = get_player_collision_volume(player);
    
    *result = check_sphere_sphere_collision(enemy_vol.center, enemy_vol.radius, 
                                          player_vol.center, player_vol.radius);
    
    return result->hit;
}

// Damage system
void apply_damage_to_enemy(Enemy* enemy, const DamageInfo* damage) {
    if (!enemy || !damage || enemy->ai_state == AI_DEAD) return;
    
    enemy->health -= damage->amount;
    
    printf("Enemy took %.1f damage! Health: %.1f\n", damage->amount, enemy->health);
    
    // Play hit sound
    play_enemy_hit_sound(damage->hit_point);
    
    if (enemy->health <= 0.0f) {
        enemy->health = 0.0f;
        enemy->ai_state = AI_DEAD;
        enemy->is_active = 0;
        
        // Play death sound
        play_enemy_death_sound(enemy->position);
        printf("Enemy killed!\n");
    }
    
    create_damage_effects(damage);
}

void apply_damage_to_player(PlayerState* player, const DamageInfo* damage) {
    if (!player || !damage) return;
    
    player->health -= (int)damage->amount;
    if (player->health < 0) player->health = 0;
    
    printf("Player took %.1f damage! Health: %d/%d\n", 
           damage->amount, player->health, player->max_health);
    
    // Play player hit sound
    play_player_hit_sound();
    
    create_damage_effects(damage);
}

// Forward declarations for bridge functions
extern void create_hit_effect_at_position(float x, float y, float z, int effect_type, float damage);
extern void play_enemy_hit_sound(Vector3 position);
extern void play_player_hit_sound();
extern void play_enemy_death_sound(Vector3 position);
extern void play_explosion_sound(Vector3 position);

void create_damage_effects(const DamageInfo* damage) {
    if (!damage) return;
    
    // Create visual effects based on damage type
    int effect_type = 0; // Default explosion
    switch (damage->damage_type) {
        case 0: // bullet
            effect_type = (damage->amount > 30.0f) ? 1 : 2; // blood for high damage, sparks for low
            break;
        case 1: // explosion
            effect_type = 0;
            break;
        case 2: // melee
            effect_type = 1; // blood
            break;
    }
    
    // Call graphics bridge to create effect
    create_hit_effect_at_position(damage->hit_point.x, damage->hit_point.y, damage->hit_point.z, 
                                 effect_type, damage->amount);
    
    printf("Creating damage effects at (%.2f, %.2f, %.2f) - Damage: %.1f, Type: %d\n",
           damage->hit_point.x, damage->hit_point.y, damage->hit_point.z, damage->amount, effect_type);
}

// Collision volumes for game objects
CollisionVolume get_player_collision_volume(const PlayerState* player) {
    CollisionVolume volume;
    volume.center = player->position;
    volume.center.y += 0.9f; // Player height offset
    volume.radius = 0.5f;
    volume.shape = COLLISION_SPHERE;
    return volume;
}

CollisionVolume get_enemy_collision_volume(const Enemy* enemy) {
    CollisionVolume volume;
    volume.center = enemy->position;
    volume.center.y += 0.5f; // Enemy height offset
    
    // Different collision sizes based on enemy type
    switch (enemy->type) {
        case ENEMY_BASIC:
            volume.radius = 0.5f;
            break;
        case ENEMY_FAST:
            volume.radius = 0.4f;
            break;
        case ENEMY_HEAVY:
            volume.radius = 0.7f;
            break;
        default:
            volume.radius = 0.5f;
            break;
    }
    
    volume.shape = COLLISION_SPHERE;
    return volume;
}

CollisionVolume get_projectile_collision_volume(const Projectile* projectile) {
    CollisionVolume volume;
    volume.center = projectile->position;
    volume.radius = 0.1f; // Small projectile radius
    volume.shape = COLLISION_SPHERE;
    return volume;
}
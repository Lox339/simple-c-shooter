#include "enemy_ai.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void enemy_ai_init(EnemyAI* ai, EnemyType type) {
    memset(ai, 0, sizeof(EnemyAI));
    ai->type = type;
    ai->state = AI_STATE_IDLE;
    ai->last_attack_time = 0.0f;
    
    // Configure based on enemy type
    switch (type) {
        case ENEMY_TYPE_BASIC:
            enemy_ai_configure_basic(ai);
            break;
        case ENEMY_TYPE_FAST:
            enemy_ai_configure_fast(ai);
            break;
        case ENEMY_TYPE_HEAVY:
            enemy_ai_configure_heavy(ai);
            break;
    }
    
    ai->health = ai->max_health;
}

void enemy_ai_update(EnemyAI* ai, Enemy* enemy, const PlayerState* player, float delta_time) {
    if (ai->state == AI_STATE_DEAD) {
        return;
    }
    
    // Check if enemy is dead
    if (ai->health <= 0.0f) {
        enemy_ai_set_state(ai, AI_STATE_DEAD);
        enemy->is_active = 0;
        return;
    }
    
    // Calculate distance to player
    float dx = player->position.x - enemy->position.x;
    float dy = player->position.y - enemy->position.y;
    float dz = player->position.z - enemy->position.z;
    float distance_to_player = sqrtf(dx*dx + dy*dy + dz*dz);
    
    // Check if player is visible
    ai->is_player_visible = enemy_ai_can_see_player(enemy, player);
    
    if (ai->is_player_visible) {
        ai->last_known_player_pos = player->position;
    }
    
    // State machine
    switch (ai->state) {
        case AI_STATE_IDLE:
            if (distance_to_player <= ai->detection_range && ai->is_player_visible) {
                enemy_ai_set_state(ai, AI_STATE_CHASING);
            }
            break;
            
        case AI_STATE_CHASING:
            if (distance_to_player <= ai->attack_range) {
                enemy_ai_set_state(ai, AI_STATE_ATTACKING);
            } else if (distance_to_player > ai->detection_range * 1.5f && !ai->is_player_visible) {
                enemy_ai_set_state(ai, AI_STATE_IDLE);
            } else {
                // Move towards player or last known position
                Vector3 target = ai->is_player_visible ? player->position : ai->last_known_player_pos;
                enemy_ai_move_towards_target(ai, enemy, target, delta_time);
            }
            break;
            
        case AI_STATE_ATTACKING:
            if (distance_to_player > ai->attack_range * 1.2f) {
                enemy_ai_set_state(ai, AI_STATE_CHASING);
            } else {
                // Attack player
                enemy_ai_attack_player(ai, enemy, player, delta_time);
                // Still move slightly towards player while attacking
                enemy_ai_move_towards_target(ai, enemy, player->position, delta_time * 0.3f);
            }
            break;
            
        case AI_STATE_DEAD:
            // Do nothing
            break;
    }
}

void enemy_ai_set_state(EnemyAI* ai, AIState new_state) {
    if (ai->state != new_state) {
        ai->state = new_state;
        
        // State entry actions
        switch (new_state) {
            case AI_STATE_ATTACKING:
                ai->last_attack_time = 0.0f; // Reset attack timer
                break;
            default:
                break;
        }
    }
}

int enemy_ai_can_see_player(const Enemy* enemy, const PlayerState* player) {
    // Simple line-of-sight check (in a real game, you'd do raycasting)
    float dx = player->position.x - enemy->position.x;
    float dy = player->position.y - enemy->position.y;
    float dz = player->position.z - enemy->position.z;
    float distance = sqrtf(dx*dx + dy*dy + dz*dz);
    
    // For now, assume player is always visible if within range
    // In a real implementation, you'd check for obstacles
    return distance <= 50.0f; // Max visibility range
}

void enemy_ai_move_towards_target(EnemyAI* ai, Enemy* enemy, Vector3 target, float delta_time) {
    float dx = target.x - enemy->position.x;
    float dy = target.y - enemy->position.y;
    float dz = target.z - enemy->position.z;
    float distance = sqrtf(dx*dx + dy*dy + dz*dz);
    
    if (distance > 0.1f) {
        // Normalize direction
        dx /= distance;
        dy /= distance;
        dz /= distance;
        
        // Move towards target
        float move_distance = ai->move_speed * delta_time;
        enemy->position.x += dx * move_distance;
        enemy->position.y += dy * move_distance;
        enemy->position.z += dz * move_distance;
        
        // Update velocity for physics
        enemy->velocity.x = dx * ai->move_speed;
        enemy->velocity.y = dy * ai->move_speed;
        enemy->velocity.z = dz * ai->move_speed;
    }
}

void enemy_ai_attack_player(EnemyAI* ai, Enemy* enemy, const PlayerState* player, float current_time) {
    if (current_time - ai->last_attack_time >= ai->attack_cooldown) {
        // Create enemy projectile
        Vector3 direction;
        direction.x = player->position.x - enemy->position.x;
        direction.y = player->position.y - enemy->position.y;
        direction.z = player->position.z - enemy->position.z;
        
        // Normalize direction
        float length = sqrtf(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
        if (length > 0.0f) {
            direction.x /= length;
            direction.y /= length;
            direction.z /= length;
            
            // Create projectile (this would be handled by the object manager)
            // For now, we'll just mark that an attack happened
            ai->last_attack_time = current_time;
        }
    }
}

void enemy_ai_configure_basic(EnemyAI* ai) {
    ai->detection_range = 15.0f;
    ai->attack_range = 8.0f;
    ai->move_speed = 3.0f;
    ai->attack_cooldown = 2.0f;
    ai->max_health = 100.0f;
}

void enemy_ai_configure_fast(EnemyAI* ai) {
    ai->detection_range = 12.0f;
    ai->attack_range = 6.0f;
    ai->move_speed = 6.0f;
    ai->attack_cooldown = 1.0f;
    ai->max_health = 60.0f;
}

void enemy_ai_configure_heavy(EnemyAI* ai) {
    ai->detection_range = 20.0f;
    ai->attack_range = 10.0f;
    ai->move_speed = 1.5f;
    ai->attack_cooldown = 3.0f;
    ai->max_health = 200.0f;
}
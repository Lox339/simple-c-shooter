#include "object_manager.h"
#include "game_state.h"
#include "enemy_ai.h"
#include "collision_system.h"
#include "../audio_bridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Enemy spawn points around the player
static Vector3 enemy_spawn_points[] = {
    {10.0f, 0.0f, 10.0f},
    {-10.0f, 0.0f, 10.0f},
    {10.0f, 0.0f, -10.0f},
    {-10.0f, 0.0f, -10.0f},
    {15.0f, 0.0f, 0.0f},
    {-15.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 15.0f},
    {0.0f, 0.0f, -15.0f}
};

static int num_spawn_points = sizeof(enemy_spawn_points) / sizeof(Vector3);

void init_object_manager() {
    printf("Object Manager initialized\n");
}

// Enemy management functions
int create_enemy(EnemyType type, Vector3 position) {
    GameState* game_state = get_game_state();
    
    if (game_state->enemy_count >= MAX_ENEMIES) {
        printf("Cannot create enemy: maximum limit reached (%d)\n", MAX_ENEMIES);
        return -1;
    }
    
    int enemy_id = game_state->enemy_count;
    Enemy* enemy = &game_state->enemies[enemy_id];
    
    // Initialize enemy
    enemy->position = position;
    enemy->velocity = (Vector3){0.0f, 0.0f, 0.0f};
    enemy->target_position = game_state->player.position;
    enemy->type = type;
    enemy->ai_state = AI_PATROL;
    enemy->last_attack_time = 0.0f;
    enemy->is_active = 1;
    
    // Allocate and initialize AI component
    enemy->ai = malloc(sizeof(EnemyAI));
    if (enemy->ai) {
        EnemyType ai_type = (type == ENEMY_BASIC) ? ENEMY_TYPE_BASIC :
                           (type == ENEMY_FAST) ? ENEMY_TYPE_FAST : ENEMY_TYPE_HEAVY;
        enemy_ai_init(enemy->ai, ai_type);
    }
    
    // Set enemy properties based on type (keep for compatibility)
    switch (type) {
        case ENEMY_BASIC:
            enemy->health = 50.0f;
            enemy->speed = 2.0f;
            enemy->attack_range = 5.0f;
            break;
            
        case ENEMY_FAST:
            enemy->health = 30.0f;
            enemy->speed = 4.0f;
            enemy->attack_range = 3.0f;
            break;
            
        case ENEMY_HEAVY:
            enemy->health = 100.0f;
            enemy->speed = 1.0f;
            enemy->attack_range = 8.0f;
            break;
    }
    
    game_state->enemy_count++;
    
    printf("Created %s enemy with AI at (%.2f, %.2f, %.2f) - ID: %d\n",
           type == ENEMY_BASIC ? "BASIC" : 
           type == ENEMY_FAST ? "FAST" : "HEAVY",
           position.x, position.y, position.z, enemy_id);
    
    return enemy_id;
}

void update_enemies(float delta_time) {
    GameState* game_state = get_game_state();
    
    for (int i = 0; i < game_state->enemy_count; i++) {
        Enemy* enemy = &game_state->enemies[i];
        
        if (!enemy->is_active || enemy->ai_state == AI_DEAD) {
            continue;
        }
        
        // Use new AI system if available
        if (enemy->ai) {
            enemy_ai_update(enemy->ai, enemy, &game_state->player, delta_time);
            
            // Sync AI health with enemy health
            enemy->health = enemy->ai->health;
            
            // Update legacy ai_state for compatibility
            switch (enemy->ai->state) {
                case AI_STATE_IDLE: enemy->ai_state = AI_PATROL; break;
                case AI_STATE_CHASING: enemy->ai_state = AI_CHASE; break;
                case AI_STATE_ATTACKING: enemy->ai_state = AI_ATTACK; break;
                case AI_STATE_DEAD: enemy->ai_state = AI_DEAD; break;
            }
        } else {
            // Fallback to old AI system
            update_enemy_ai(enemy, &game_state->player, delta_time);
            update_enemy_movement(enemy, delta_time);
        }
    }
}

void update_enemy_ai(Enemy* enemy, PlayerState* player, float delta_time) {
    // Calculate distance to player
    float dx = player->position.x - enemy->position.x;
    float dy = player->position.y - enemy->position.y;
    float dz = player->position.z - enemy->position.z;
    float distance_to_player = sqrtf(dx*dx + dy*dy + dz*dz);
    
    // Update target position to player's current position
    enemy->target_position = player->position;
    
    // AI state machine
    switch (enemy->ai_state) {
        case AI_PATROL:
            // Switch to chase if player is close
            if (distance_to_player < 12.0f) {
                enemy->ai_state = AI_CHASE;
                printf("Enemy %p started chasing player (distance: %.2f)\n", 
                       (void*)enemy, distance_to_player);
            }
            break;
            
        case AI_CHASE:
            // Switch to attack if in range
            if (distance_to_player <= enemy->attack_range) {
                enemy->ai_state = AI_ATTACK;
                enemy->last_attack_time = 0.0f;
                printf("Enemy %p entered attack range (distance: %.2f)\n", 
                       (void*)enemy, distance_to_player);
            }
            // Return to patrol if player is too far
            else if (distance_to_player > 15.0f) {
                enemy->ai_state = AI_PATROL;
                printf("Enemy %p lost player, returning to patrol\n", (void*)enemy);
            }
            break;
            
        case AI_ATTACK:
            enemy->last_attack_time += delta_time;
            
            // Attack every 2 seconds
            if (enemy->last_attack_time >= 2.0f) {
                attack_player(enemy, player);
                enemy->last_attack_time = 0.0f;
            }
            
            // Return to chase if player moves away
            if (distance_to_player > enemy->attack_range + 1.0f) {
                enemy->ai_state = AI_CHASE;
                printf("Enemy %p player moved away, chasing again\n", (void*)enemy);
            }
            break;
            
        case AI_DEAD:
            // Dead enemies don't update
            break;
    }
}

void update_enemy_movement(Enemy* enemy, float delta_time) {
    if (enemy->ai_state == AI_DEAD || enemy->ai_state == AI_ATTACK) {
        // Stop moving when dead or attacking
        enemy->velocity = (Vector3){0.0f, 0.0f, 0.0f};
        return;
    }
    
    // Calculate direction to target
    float dx = enemy->target_position.x - enemy->position.x;
    float dz = enemy->target_position.z - enemy->position.z;
    float distance = sqrtf(dx*dx + dz*dz);
    
    if (distance > 0.1f) {
        // Normalize direction and apply speed
        enemy->velocity.x = (dx / distance) * enemy->speed;
        enemy->velocity.z = (dz / distance) * enemy->speed;
        
        // Update position
        enemy->position.x += enemy->velocity.x * delta_time;
        enemy->position.z += enemy->velocity.z * delta_time;
        
        // Keep enemy on ground
        enemy->position.y = 0.0f;
    } else {
        enemy->velocity = (Vector3){0.0f, 0.0f, 0.0f};
    }
}

void attack_player(Enemy* enemy, PlayerState* player) {
    printf("Enemy %p attacks player! ", (void*)enemy);
    
    // Create projectile towards player
    Vector3 direction;
    direction.x = player->position.x - enemy->position.x;
    direction.y = player->position.y - enemy->position.y + 1.0f; // Aim slightly higher
    direction.z = player->position.z - enemy->position.z;
    
    // Normalize direction
    float length = sqrtf(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
    if (length > 0.0f) {
        direction.x /= length;
        direction.y /= length;
        direction.z /= length;
        
        // Set projectile speed based on enemy type
        float projectile_speed = 15.0f;
        switch (enemy->type) {
            case ENEMY_BASIC: projectile_speed = 12.0f; break;
            case ENEMY_FAST: projectile_speed = 18.0f; break;
            case ENEMY_HEAVY: projectile_speed = 10.0f; break;
        }
        
        Vector3 projectile_velocity = {
            direction.x * projectile_speed,
            direction.y * projectile_speed,
            direction.z * projectile_speed
        };
        
        // Create enemy projectile
        Vector3 spawn_pos = enemy->position;
        spawn_pos.y += 1.0f; // Spawn slightly above enemy
        
        create_projectile(PROJECTILE_ENEMY_BULLET, spawn_pos, projectile_velocity, -1);
        
        // Play enemy shoot sound
        play_enemy_shoot_sound(enemy->position);
        
        printf("Enemy fired projectile at player!\n");
    }
}

void remove_enemy(int enemy_id) {
    GameState* game_state = get_game_state();
    
    if (enemy_id < 0 || enemy_id >= game_state->enemy_count) {
        return;
    }
    
    printf("Removing enemy ID: %d\n", enemy_id);
    
    // Free AI memory if allocated
    if (game_state->enemies[enemy_id].ai) {
        free(game_state->enemies[enemy_id].ai);
        game_state->enemies[enemy_id].ai = NULL;
    }
    
    // Shift remaining enemies down
    for (int i = enemy_id; i < game_state->enemy_count - 1; i++) {
        game_state->enemies[i] = game_state->enemies[i + 1];
    }
    
    game_state->enemy_count--;
}

// Projectile management functions
int create_projectile(ProjectileType type, Vector3 position, Vector3 velocity, int owner_id) {
    GameState* game_state = get_game_state();
    
    if (game_state->projectile_count >= MAX_PROJECTILES) {
        printf("Cannot create projectile: maximum limit reached (%d)\n", MAX_PROJECTILES);
        return -1;
    }
    
    int projectile_id = game_state->projectile_count;
    Projectile* projectile = &game_state->projectiles[projectile_id];
    
    // Initialize projectile
    projectile->position = position;
    projectile->velocity = velocity;
    projectile->type = type;
    projectile->owner_id = owner_id;
    projectile->lifetime = 5.0f; // 5 seconds lifetime
    
    // Set damage based on type
    switch (type) {
        case PROJECTILE_PLAYER_BULLET:
            projectile->damage = 25.0f;
            break;
        case PROJECTILE_ENEMY_BULLET:
            projectile->damage = 15.0f;
            break;
    }
    
    game_state->projectile_count++;
    
    printf("Created %s projectile at (%.2f, %.2f, %.2f) - ID: %d\n",
           type == PROJECTILE_PLAYER_BULLET ? "PLAYER" : "ENEMY",
           position.x, position.y, position.z, projectile_id);
    
    return projectile_id;
}

void update_projectiles(float delta_time) {
    GameState* game_state = get_game_state();
    
    for (int i = game_state->projectile_count - 1; i >= 0; i--) {
        Projectile* projectile = &game_state->projectiles[i];
        
        // Update position
        projectile->position.x += projectile->velocity.x * delta_time;
        projectile->position.y += projectile->velocity.y * delta_time;
        projectile->position.z += projectile->velocity.z * delta_time;
        
        // Apply gravity (reduced for faster projectiles)
        projectile->velocity.y -= 5.0f * delta_time;
        
        // Update lifetime
        projectile->lifetime -= delta_time;
        
        // Check for removal conditions
        int should_remove = 0;
        
        // Remove if lifetime expired
        if (projectile->lifetime <= 0.0f) {
            should_remove = 1;
        }
        
        // Remove if hit ground
        if (projectile->position.y <= 0.0f) {
            should_remove = 1;
        }
        
        // Check collisions with enemies (for player bullets)
        if (projectile->type == PROJECTILE_PLAYER_BULLET) {
            for (int j = 0; j < game_state->enemy_count; j++) {
                Enemy* enemy = &game_state->enemies[j];
                if (enemy->ai_state == AI_DEAD || !enemy->is_active) continue;
                
                CollisionResult collision;
                if (check_projectile_enemy_collision(projectile, enemy, &collision)) {
                    // Create damage info
                    DamageInfo damage;
                    damage.amount = projectile->damage;
                    damage.hit_point = collision.hit_point;
                    damage.hit_direction = vector3_normalize(projectile->velocity);
                    damage.damage_type = 0; // bullet damage
                    
                    // Apply damage using new system
                    apply_damage_to_enemy(enemy, &damage);
                    
                    // Award points if enemy was killed
                    if (enemy->ai_state == AI_DEAD) {
                        int points = 0;
                        switch (enemy->type) {
                            case ENEMY_BASIC: points = 100; break;
                            case ENEMY_FAST: points = 150; break;
                            case ENEMY_HEAVY: points = 200; break;
                        }
                        game_state->score += points;
                        printf("Enemy killed! +%d points, Score: %d\n", points, game_state->score);
                    }
                    
                    should_remove = 1;
                    break;
                }
            }
        }
        
        // Check collision with player (for enemy bullets)
        if (projectile->type == PROJECTILE_ENEMY_BULLET) {
            CollisionResult collision;
            if (check_projectile_player_collision(projectile, &game_state->player, &collision)) {
                // Create damage info
                DamageInfo damage;
                damage.amount = projectile->damage;
                damage.hit_point = collision.hit_point;
                damage.hit_direction = vector3_normalize(projectile->velocity);
                damage.damage_type = 0; // bullet damage
                
                // Apply damage using new system
                apply_damage_to_player(&game_state->player, &damage);
                
                if (game_state->player.health <= 0) {
                    printf("Player died! Game Over!\n");
                    game_state->current_phase = GAME_OVER;
                }
                
                should_remove = 1;
            }
        }
        
        if (should_remove) {
            remove_projectile(i);
        }
    }
}

void remove_projectile(int projectile_id) {
    GameState* game_state = get_game_state();
    
    if (projectile_id < 0 || projectile_id >= game_state->projectile_count) {
        return;
    }
    
    // Shift remaining projectiles down
    for (int i = projectile_id; i < game_state->projectile_count - 1; i++) {
        game_state->projectiles[i] = game_state->projectiles[i + 1];
    }
    
    game_state->projectile_count--;
}

// Spawning functions
void spawn_enemy_wave(int count) {
    printf("Spawning enemy wave: %d enemies\n", count);
    
    for (int i = 0; i < count; i++) {
        // Choose random spawn point
        int spawn_index = rand() % num_spawn_points;
        Vector3 spawn_pos = enemy_spawn_points[spawn_index];
        
        // Add some randomness to spawn position
        spawn_pos.x += ((float)rand() / RAND_MAX - 0.5f) * 4.0f;
        spawn_pos.z += ((float)rand() / RAND_MAX - 0.5f) * 4.0f;
        
        // Choose random enemy type
        EnemyType type = (EnemyType)(rand() % 3);
        
        create_enemy(type, spawn_pos);
    }
}

void spawn_enemies_periodically(float delta_time) {
    static float spawn_timer = 0.0f;
    static float spawn_interval = 10.0f; // Spawn every 10 seconds
    
    GameState* game_state = get_game_state();
    
    // Only spawn in playing mode
    if (game_state->current_phase != GAME_PLAYING) {
        return;
    }
    
    spawn_timer += delta_time;
    
    if (spawn_timer >= spawn_interval) {
        // Don't spawn if too many enemies already
        if (game_state->enemy_count < MAX_ENEMIES / 2) {
            int enemies_to_spawn = 1 + (rand() % 3); // 1-3 enemies
            spawn_enemy_wave(enemies_to_spawn);
        }
        
        spawn_timer = 0.0f;
        
        // Gradually decrease spawn interval (increase difficulty)
        if (spawn_interval > 5.0f) {
            spawn_interval -= 0.5f;
            printf("Spawn interval decreased to %.1f seconds\n", spawn_interval);
        }
    }
}

int get_enemy_count() {
    GameState* game_state = get_game_state();
    return game_state->enemy_count;
}

int get_projectile_count() {
    GameState* game_state = get_game_state();
    return game_state->projectile_count;
}

void cleanup_object_manager() {
    printf("Object Manager cleaned up\n");
}
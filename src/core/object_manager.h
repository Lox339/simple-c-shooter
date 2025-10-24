#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include "game_api.h"

// Object Manager initialization
void init_object_manager();
void cleanup_object_manager();

// Enemy management functions
int create_enemy(EnemyType type, Vector3 position);
void update_enemies(float delta_time);
void update_enemy_ai(Enemy* enemy, PlayerState* player, float delta_time);
void update_enemy_movement(Enemy* enemy, float delta_time);
void attack_player(Enemy* enemy, PlayerState* player);
void remove_enemy(int enemy_id);

// Projectile management functions
int create_projectile(ProjectileType type, Vector3 position, Vector3 velocity, int owner_id);
void update_projectiles(float delta_time);
void remove_projectile(int projectile_id);

// Spawning functions
void spawn_enemy_wave(int count);
void spawn_enemies_periodically(float delta_time);

// Query functions
int get_enemy_count();
int get_projectile_count();

#endif // OBJECT_MANAGER_H
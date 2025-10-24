#ifndef ENEMY_AI_H
#define ENEMY_AI_H

#include "game_state.h"

typedef enum {
    AI_STATE_IDLE,
    AI_STATE_CHASING,
    AI_STATE_ATTACKING,
    AI_STATE_DEAD
} AIState;

typedef enum {
    ENEMY_TYPE_BASIC,
    ENEMY_TYPE_FAST,
    ENEMY_TYPE_HEAVY
} EnemyType;

typedef struct {
    AIState state;
    EnemyType type;
    float detection_range;
    float attack_range;
    float move_speed;
    float attack_cooldown;
    float last_attack_time;
    float health;
    float max_health;
    Vector3 target_position;
    Vector3 last_known_player_pos;
    int is_player_visible;
} EnemyAI;

// AI system functions
void enemy_ai_init(EnemyAI* ai, EnemyType type);
void enemy_ai_update(EnemyAI* ai, Enemy* enemy, const PlayerState* player, float delta_time);
void enemy_ai_set_state(EnemyAI* ai, AIState new_state);
int enemy_ai_can_see_player(const Enemy* enemy, const PlayerState* player);
void enemy_ai_move_towards_target(EnemyAI* ai, Enemy* enemy, Vector3 target, float delta_time);
void enemy_ai_attack_player(EnemyAI* ai, Enemy* enemy, const PlayerState* player, float current_time);

// Enemy type configurations
void enemy_ai_configure_basic(EnemyAI* ai);
void enemy_ai_configure_fast(EnemyAI* ai);
void enemy_ai_configure_heavy(EnemyAI* ai);

#endif // ENEMY_AI_H
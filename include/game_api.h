#ifndef GAME_API_H
#define GAME_API_H

#ifdef __cplusplus
extern "C" {
#endif

// Vector3 structure for 3D coordinates
typedef struct {
    float x, y, z;
} Vector3;

// Player state structure
typedef struct {
    // Position and orientation
    Vector3 position;
    Vector3 velocity;
    Vector3 rotation; // pitch, yaw, roll
    
    // Physical properties
    float speed;
    float max_speed;
    int on_ground; // Using int instead of bool for C compatibility
    float jump_time;
    
    // Game characteristics
    int health;
    int max_health;
    int ammo;
    int max_ammo;
    
    // Bunny hop data
    float horizontal_speed;
    float last_jump_time;
    int consecutive_jumps;
} PlayerState;

// Enemy types
typedef enum {
    ENEMY_BASIC,
    ENEMY_FAST,
    ENEMY_HEAVY
} EnemyType;

// AI states
typedef enum {
    AI_PATROL,
    AI_CHASE,
    AI_ATTACK,
    AI_DEAD
} AIState;

// Forward declaration for EnemyAI
struct EnemyAI;

// Enemy state structure
typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 target_position;
    float health;
    float speed;
    float attack_range;
    float last_attack_time;
    EnemyType type;
    AIState ai_state;
    int is_active;
    struct EnemyAI* ai; // AI component
} Enemy;

// Projectile types
typedef enum {
    PROJECTILE_PLAYER_BULLET,
    PROJECTILE_ENEMY_BULLET
} ProjectileType;

// Projectile state structure
typedef struct {
    Vector3 position;
    Vector3 velocity;
    float damage;
    float lifetime;
    ProjectileType type;
    int owner_id;
} Projectile;

// Game phases
typedef enum {
    GAME_MENU,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_OVER
} GamePhase;

// Constants
#define MAX_ENEMIES 50
#define MAX_PROJECTILES 100

// Main game state structure
typedef struct {
    PlayerState player;
    Enemy enemies[MAX_ENEMIES];
    Projectile projectiles[MAX_PROJECTILES];
    int score;
    int enemy_count;
    int projectile_count;
    float delta_time;
    int game_running;
    GamePhase current_phase;
} GameState;

// Input state structure
typedef struct {
    int keys[512];
    float mouse_x, mouse_y;
    float mouse_delta_x, mouse_delta_y;
    float mouse_sensitivity;
    int mouse_buttons[8];
    int jump_pressed;
} InputState;

// Exported functions for inter-language communication
void init_game();
void update_game(float delta_time);
GameState* get_game_state();
void handle_input(int key, int action);
void handle_mouse(float x_offset, float y_offset);
void handle_mouse_click(int button, int action);
void cleanup_game();

#ifdef __cplusplus
}
#endif

#endif // GAME_API_H
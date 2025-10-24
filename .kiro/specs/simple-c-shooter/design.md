# Дизайн 3D шутера

## Обзор

Проект представляет собой 3D шутер от первого лица, построенный на архитектуре из трех взаимодействующих компонентов:

- **Core Engine (C)** - основной игровой цикл, логика игры, управление состоянием
- **Graphics & Physics Engine (C++)** - 3D рендеринг, физический движок, банихоп механика  
- **UI Manager (C#)** - пользовательский интерфейс, меню, HUD, спидометр

Компоненты взаимодействуют через C API и разделяемую память для обеспечения высокой производительности.

## Архитектура

### Общая архитектура системы

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   UI Manager    │    │   Core Engine    │    │ Graphics Engine │
│     (C#)        │◄──►│      (C)         │◄──►│     (C++)       │
│                 │    │                  │    │                 │
│ - Меню          │    │ - Игровой цикл   │    │ - OpenGL/Vulkan │
│ - HUD           │    │ - Логика игры    │    │ - 3D рендеринг  │
│ - Спидометр     │    │ - Управление     │    │ - Физика        │
│ - Настройки     │    │ - Состояние      │    │ - Банихоп       │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                    ┌──────────────────┐
                    │  Shared Memory   │
                    │   & C API        │
                    │                  │
                    │ - Game State     │
                    │ - Player Data    │
                    │ - Input Events   │
                    │ - Render Queue   │
                    └──────────────────┘
```

### Межъязыковое взаимодействие

**C API Bridge** - центральный интерфейс для связи между компонентами:

```c
// game_api.h - общий интерфейс
typedef struct {
    float x, y, z;
    float velocity_x, velocity_y, velocity_z;
    float speed;
    int health;
    int ammo;
} PlayerState;

typedef struct {
    PlayerState player;
    int score;
    int enemy_count;
    float delta_time;
    int game_running;
} GameState;

// Экспортируемые функции для взаимодействия
extern "C" {
    void init_game();
    void update_game(float delta_time);
    GameState* get_game_state();
    void handle_input(int key, int action);
    void handle_mouse(float x_offset, float y_offset);
    void handle_mouse_click(int button, int action);
}
```

## Компоненты и интерфейсы

### 1. Core Engine (C)

**Ответственность:**
- Основной игровой цикл
- Управление состоянием игры
- Обработка пользовательского ввода
- Координация между компонентами

**Ключевые модули:**

```c
// core/game_loop.c
typedef struct {
    double last_time;
    double current_time;
    double delta_time;
    int target_fps;
} GameLoop;

// core/input_manager.c
typedef struct {
    int keys[512];
    float mouse_x, mouse_y;
    float mouse_sensitivity;
    int mouse_buttons[8];
} InputManager;

// core/game_state.c
typedef struct {
    PlayerState player;
    Enemy enemies[MAX_ENEMIES];
    Projectile projectiles[MAX_PROJECTILES];
    int score;
    GamePhase current_phase; // MENU, PLAYING, PAUSED, GAME_OVER
} GameState;
```

**Основные функции:**
- `init_core_engine()` - инициализация движка
- `run_game_loop()` - основной игровой цикл
- `process_input()` - обработка ввода
- `update_game_logic()` - обновление логики игры
- `cleanup_core()` - освобождение ресурсов

### 2. Graphics & Physics Engine (C++)

**Ответственность:**
- 3D рендеринг с использованием OpenGL/Vulkan
- Физический движок для движения и столкновений
- Банихоп механика и расчет скорости
- Управление 3D моделями и текстурами

**Ключевые классы:**

```cpp
// graphics/renderer.hpp
class Renderer {
private:
    GLuint shader_program;
    Camera camera;
    std::vector<Model> models;
    LightManager lights;
    
public:
    void initialize();
    void render_frame(const GameState& state);
    void load_model(const std::string& path);
    void set_camera_position(float x, float y, float z);
    void cleanup();
};

// physics/physics_engine.hpp
class PhysicsEngine {
private:
    std::vector<RigidBody> bodies;
    CollisionDetector collision_detector;
    BunnyHopController bhop_controller;
    
public:
    void initialize();
    void update(float delta_time);
    void add_rigid_body(const RigidBody& body);
    float calculate_player_speed(const PlayerState& player);
    void apply_bunny_hop(PlayerState& player, const InputState& input);
};

// physics/bunny_hop.hpp
class BunnyHopController {
private:
    float max_speed;
    float acceleration;
    float air_acceleration;
    float friction;
    
public:
    void update_movement(PlayerState& player, const InputState& input, float delta_time);
    float calculate_speed_gain(float current_speed, float input_angle);
    bool is_on_ground(const PlayerState& player);
};
```

**Банихоп механика:**
- Отслеживание состояния "на земле" vs "в воздухе"
- Расчет ускорения на основе угла поворота мыши
- Сохранение горизонтальной скорости при прыжках
- Ограничение максимальной скорости

### 3. UI Manager (C#)

**Ответственность:**
- Главное меню и настройки
- Игровой HUD (здоровье, боеприпасы, счет)
- Спидометр в правом верхнем углу
- Меню паузы

**Ключевые классы:**

```csharp
// UI/UIManager.cs
public class UIManager {
    private MainMenu mainMenu;
    private GameHUD gameHUD;
    private SettingsMenu settingsMenu;
    private Speedometer speedometer;
    
    public void Initialize();
    public void Update(GameState gameState);
    public void Render();
    public void HandleInput(InputEvent inputEvent);
}

// UI/Speedometer.cs
public class Speedometer {
    private Vector2 position;
    private Font font;
    private Color textColor;
    
    public void Update(float playerSpeed);
    public void Render();
    public void SetPosition(int x, int y);
}

// UI/GameHUD.cs
public class GameHUD {
    private HealthBar healthBar;
    private AmmoCounter ammoCounter;
    private ScoreDisplay scoreDisplay;
    private Crosshair crosshair;
    
    public void Update(PlayerState player, int score);
    public void Render();
}
```

## Модели данных

### Player State
```c
typedef struct {
    // Позиция и ориентация
    Vector3 position;
    Vector3 velocity;
    Vector3 rotation; // pitch, yaw, roll
    
    // Физические свойства
    float speed;
    float max_speed;
    bool on_ground;
    float jump_time;
    
    // Игровые характеристики
    int health;
    int max_health;
    int ammo;
    int max_ammo;
    
    // Банихоп данные
    float horizontal_speed;
    float last_jump_time;
    int consecutive_jumps;
} PlayerState;
```

### Enemy State
```c
typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 target_position;
    float health;
    float speed;
    float attack_range;
    float last_attack_time;
    EnemyType type;
    AIState ai_state; // PATROL, CHASE, ATTACK, DEAD
} Enemy;
```

### Projectile State
```c
typedef struct {
    Vector3 position;
    Vector3 velocity;
    float damage;
    float lifetime;
    ProjectileType type; // PLAYER_BULLET, ENEMY_BULLET
    int owner_id;
} Projectile;
```

## Обработка ошибок

### Стратегия обработки ошибок
- **C компонент**: Коды возврата и глобальная переменная errno
- **C++ компонент**: Исключения для критических ошибок, логирование для предупреждений
- **C# компонент**: Исключения .NET с детальными сообщениями

### Логирование
```c
// logging/logger.h
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

void log_message(LogLevel level, const char* component, const char* message);
void init_logger(const char* log_file);
void cleanup_logger();
```

## Стратегия тестирования

### Unit тесты
- **C компонент**: Unity testing framework
- **C++ компонент**: Google Test
- **C# компонент**: NUnit

### Integration тесты
- Тестирование межъязыкового взаимодействия
- Проверка производительности игрового цикла
- Валидация банихоп механики

### Performance тесты
- Профилирование рендеринга (60+ FPS)
- Тестирование памяти на утечки
- Бенчмарки физических расчетов

## Технические детали

### Банихоп алгоритм
```cpp
void BunnyHopController::update_movement(PlayerState& player, const InputState& input, float delta_time) {
    if (is_on_ground(player)) {
        // Обычное движение по земле
        apply_ground_movement(player, input, delta_time);
        if (input.jump_pressed) {
            player.velocity.y = JUMP_VELOCITY;
            player.on_ground = false;
        }
    } else {
        // Воздушное движение с банихопом
        apply_air_movement(player, input, delta_time);
        
        // Расчет прироста скорости от поворота мыши
        float mouse_angle = calculate_mouse_angle(input.mouse_delta);
        float speed_gain = calculate_speed_gain(player.horizontal_speed, mouse_angle);
        
        // Применение ускорения
        Vector3 forward = get_forward_vector(player.rotation.yaw + mouse_angle);
        player.velocity += forward * speed_gain * AIR_ACCELERATION * delta_time;
        
        // Ограничение максимальной скорости
        if (length(player.velocity) > MAX_BHOP_SPEED) {
            player.velocity = normalize(player.velocity) * MAX_BHOP_SPEED;
        }
    }
    
    // Обновление отображаемой скорости
    player.speed = length(Vector2(player.velocity.x, player.velocity.z));
}
```

### Рендеринг спидометра
```csharp
public void Speedometer::Update(float playerSpeed) {
    // Конвертация скорости в удобные единицы
    float displaySpeed = playerSpeed * SPEED_MULTIPLIER;
    
    // Форматирование текста
    string speedText = $"Speed: {displaySpeed:F1} u/s";
    
    // Цветовое кодирование скорости
    if (displaySpeed > HIGH_SPEED_THRESHOLD) {
        textColor = Color.Red;
    } else if (displaySpeed > MEDIUM_SPEED_THRESHOLD) {
        textColor = Color.Yellow;
    } else {
        textColor = Color.White;
    }
    
    this.text = speedText;
}
```

### Система сборки
- **CMake** для C/C++ компонентов
- **MSBuild** для C# компонента
- **GitHub Actions** для автоматической сборки всех платформ
- **Conan** для управления зависимостями C++
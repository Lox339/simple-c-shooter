#ifndef HIT_EFFECTS_HPP
#define HIT_EFFECTS_HPP

#include "../game_api.h"
#include <vector>

struct HitEffect {
    Vector3 position;
    Vector3 color;
    float lifetime;
    float max_lifetime;
    float size;
    int type; // 0 = explosion, 1 = blood, 2 = spark
};

class HitEffectsSystem {
private:
    std::vector<HitEffect> effects;
    unsigned int particle_vao, particle_vbo;
    
public:
    HitEffectsSystem();
    ~HitEffectsSystem();
    
    bool initialize();
    void cleanup();
    
    // Effect creation
    void create_explosion_effect(Vector3 position, float size = 1.0f);
    void create_blood_effect(Vector3 position, float size = 0.8f);
    void create_spark_effect(Vector3 position, float size = 0.5f);
    void create_damage_number(Vector3 position, float damage);
    
    // Update and render
    void update(float delta_time);
    void render(unsigned int shader_program);
    
    // Utility
    void clear_all_effects();
    int get_effect_count() const { return effects.size(); }
};

#endif // HIT_EFFECTS_HPP
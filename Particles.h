#ifndef PARTICLES_H
#define PARTICLES_H

#include "Utils.h"
#include <vector>

enum class ParticleType {
    DUST,
    POLLEN,
    LEAF,
    SNOW_MICRO,
    CHIMNEY_SMOKE
};

struct Particle {
    ParticleType type;
    float x, y;
    float speedX, speedY;
    float life;      // 0.0 to 1.0 (or seconds)
    float maxLife;
    float size;
    float rotation;  // For leaves
    float rotSpeed;
    Utils::Color color;
    bool active;
};

namespace ParticleSystem {
    void init(int maxParticles = 500);
    void update(float timeSpeed, Utils::Season season, float windStrength, int width, int height);
    void draw(float timeOfDay);
    
    // Spawners
    void spawnPollen(int count, int width, int height);
    void spawnDust(int count, int width, int height);
    void spawnLeaves(int count, int width, int height, float wind);
    void spawnSnowMicro(int count, int width, int height);
    void spawnSmoke(float x, float y);
}

#endif // PARTICLES_H

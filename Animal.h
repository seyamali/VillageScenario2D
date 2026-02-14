#ifndef ANIMAL_H
#define ANIMAL_H

#include "Utils.h"
#include <vector>

enum class AnimalType {
    COW,
    SHEEP,
    BIRD
};

enum class AnimalState {
    IDLE,
    GRAZING,
    MOVING,
    SLEEPING,
    FLYING // Bird specific
};

struct Animal {
    AnimalType type;
    float x, y;
    float targetX, targetY;
    float speed;
    int direction; // 1 or -1
    
    AnimalState currentState;
    float stateTimer;
    
    // Animation
    float animFrame;
    
    // Bounding / Herd
    int herdId;
};

namespace AnimalSystem {
    Animal create(AnimalType type, float x, float y);
    void update(Animal& a, float time, bool isNight, float windSway); // Wind affects bird flight
    void draw(const Animal& a, const Utils::Color& ambientLight);
}

#endif // ANIMAL_H

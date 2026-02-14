#ifndef BUILDING_H
#define BUILDING_H

#include <cmath>
#include "Utils.h"

struct BuildingProps {
    float x, y;
    float width, height;
    float roofHeight;
    Utils::Color wallColor;
    Utils::Color roofColor;
    Utils::Color doorColor;
    bool hasChimney;
    bool lightOn;
    
    // Animation states
    float doorAngle; // 0.0 (closed) to 1.0 (open)
};

namespace Building {
    // Generate a building with randomized properties
    BuildingProps create(float x, float y);
    
    // Draw the building with its current state
    void draw(BuildingProps& props, float time, const Utils::Color& ambientLight, Utils::Season season = Utils::Season::SPRING);
    
    // Update animation states (smoke, door, lights)
    void update(BuildingProps& props, float time, bool isNight);
}

#endif // BUILDING_H

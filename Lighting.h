#ifndef LIGHTING_H
#define LIGHTING_H

#include "Utils.h"
#include <vector>

struct LightSource {
    float x, y;
    float radius;
    Utils::Color color;
    float flickerOffset;
    bool active; // Day vs Night logic
};

namespace LightingSystem {
    // Collects all active lights (street lamps, windows, fireflies?)
    void collectLights(std::vector<LightSource>& lights, float timeOfDay, 
                       const std::vector<float>& houseX, const std::vector<float>& houseY);

    // Renders the dark overlay with punch-outs for lights (Multiplicative blending)
    void drawLightingOverlay(int width, int height, float timeOfDay, 
                             const Utils::Color& ambientLight, 
                             const std::vector<LightSource>& lights);
                             
    // Renders volumetric light shafts (Additive blending) during sunrise/sunset
    void drawGodRays(int width, int height, float timeOfDay);
    
    // Renders simple bloom/glow sprites over bright spots
    void drawBloom(const std::vector<LightSource>& lights);
}

#endif // LIGHTING_H

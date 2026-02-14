#ifndef SCENE_ELEMENTS_H
#define SCENE_ELEMENTS_H

#include "Utils.h"

namespace SceneElements {

    // Environment
    void drawSky(const Utils::Color& top, const Utils::Color& bottom, float timeOfDay); 
    void drawStars(float time, float intensity);
    void drawSunAndMoon(float timeOfDay);
    void drawClouds(float cloudOffset, float alpha, float scale, float yPos);
    
    // Landscape
    void drawMountains(float parallaxOffset, const Utils::Color& tint, Utils::Season season = Utils::Season::SPRING);
    void drawGround(int width, int height, const Utils::Color& tint, Utils::Season season = Utils::Season::SPRING);
    void drawRiver(float time, const Utils::Color& skyColor, const Utils::Color& tint, Utils::Season season = Utils::Season::SPRING);
    
    // Objects
    void drawHouse(float x, float y, const Utils::Color& tint, Utils::Season season = Utils::Season::SPRING);
    void drawTree(float x, float y, const Utils::Color& tint, float sway = 0.0f, Utils::Season season = Utils::Season::SPRING);
    void drawBoat(float x, float y, const Utils::Color& tint);
    void drawBird(float x, float y, float wingAngle, const Utils::Color& tint);
    
    // Details
    void drawFoliage(float x, float y, const Utils::Color& tint, float sway = 0.0f, Utils::Season season = Utils::Season::SPRING);
    void drawFlowers(float x, float y, const Utils::Color& tint, float sway = 0.0f, Utils::Season season = Utils::Season::SPRING);
    
    // Helper
    void setTint(const Utils::Color& tint);
}

#endif // SCENE_ELEMENTS_H

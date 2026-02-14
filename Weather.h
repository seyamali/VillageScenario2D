#ifndef WEATHER_H
#define WEATHER_H

#include <vector>
#include "Utils.h"

enum class WeatherType {
    CLEAR,
    RAIN,
    SNOW,
    STORM
};

struct WeatherParticle {
    float x, y;
    float speedY;
    float speedX; // Wind influence
    bool active;
};

struct WeatherState {
    WeatherType currentType;
    float intensity; // 0.0 to 1.0 (Rain/Snow density)
    float windStrength; // -5.0 to 5.0
    float fogDensity;
    
    // Lightning
    float lightningTimer;
    bool isLightningActive;

    // Cycle
    float transitionTimer;
    
    std::vector<WeatherParticle> particles;
    
    WeatherState();
};

namespace WeatherSystem {
    void init(WeatherState& state);
    void update(WeatherState& state, float timeSpeed, int width, int height); // Added width/height for particle bounds
    void draw(const WeatherState& state, int width, int height);
    
    // Helper to get wind sway for trees/grass
    float getWindSway(const WeatherState& state, float time);
}

#endif // WEATHER_H

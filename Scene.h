#ifndef SCENE_H
#define SCENE_H

#include <cmath>
#include <vector>
#include "Utils.h"
#include "Building.h"
#include "Character.h"
#include "Weather.h"
#include "Animal.h"
#include "Lighting.h"
#include "Particles.h"
#include "Camera.h"
#include "Events.h"
#include "Analytics.h"

namespace Scene {
    
    struct CloudLayer {
        float x;
        float y;
        float speed;
        float scale;
        float alpha;
        
        CloudLayer(float x, float y, float speed, float scale, float alpha) 
            : x(x), y(y), speed(speed), scale(scale), alpha(alpha) {}
    };

    struct State {
        // Time/Cycle (0.00 to 24.00)
        float timeOfDay; // 0.0 -> 24.0
        float timeSpeed;
        
        // Seasons
        Utils::Season currentSeason;
        float seasonTimer; // To track progression
        
        bool isDay; // For simpler checks, but we mostly use the gradient
        bool showClouds;
        bool showBirds;
        bool showStars;
        
        // Dynamic Sky Colors
        Utils::Color skyTop;
        Utils::Color skyBottom;
        Utils::Color ambientLight;
        
        // Animation variables
        float boatX;
        float sunX;
        float starTwinkleOffset;
        float waveOffset;
        
        // Multi-Layer Clouds
        CloudLayer layers[3];
        
        // Settings
        int width;
        int height;
        
        // Objects
        std::vector<BuildingProps> houses;
        std::vector<Character> villagers;
        std::vector<Animal> animals;
        
        // Weather
        WeatherState weather;
        float currentWindSway;
        
        // Lighting
        std::vector<LightSource> lights;
        
        // Camera
        CameraSystem::CameraState camera;
        
        // Advanced
        EventState events;
        Analytics::Metrics metrics;
        
        State() : 
            timeOfDay(12.0f), timeSpeed(0.01f), // Start at Noon
            currentSeason(Utils::Season::SPRING), seasonTimer(0.0f),
            isDay(true), showClouds(true), showBirds(true), showStars(false),
            skyTop(0.0f, 0.4f, 0.8f), skyBottom(0.5f, 0.7f, 1.0f), ambientLight(1.0f, 1.0f, 1.0f),
            boatX(0), sunX(0), starTwinkleOffset(0), waveOffset(0),
            layers{
                CloudLayer(10, 48, 0.05f, 3.0f, 0.8f),
                CloudLayer(40, 52, 0.03f, 4.0f, 0.6f),
                CloudLayer(-10, 45, 0.07f, 2.5f, 0.9f)
            },
            width(800), height(600),
            currentWindSway(0.0f)
        {}
    };

    void init();
    void update(int value); // Timer callback
    void display();
    void reshape(int w, int h);
    void handleKeyboard(unsigned char key, int x, int y);
    
    // Calculate sky colors based on time
    void updateSkyColors();
    
    // Accessor for main
    State& getState(); 
}

#endif // SCENE_H

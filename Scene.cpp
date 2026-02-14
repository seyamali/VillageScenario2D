#include "Scene.h"
#include "SceneElements.h"
#include "Building.h"
#include "Character.h"
#include "Style.h"
#include <GL/glut.h>
#include <iostream>

namespace Scene {
    State state;

    State& getState() { return state; }

    void init() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_BLEND);
        
        // Create diverse buildings
        state.houses.push_back(Building::create(5, 20));
        state.houses.push_back(Building::create(50, 22)); // Hill House
        state.houses.push_back(Building::create(-15, 18));
        
        // Populate villagers
        state.villagers.clear();
        for(int i=0; i<5; i++) {
             float x = float(i) * 15.0f - 10.0f;
             state.villagers.push_back(CharacterSystem::create(x));
        }
        
        // Animals
        state.animals.push_back(AnimalSystem::create(AnimalType::COW, 5, 20));
        state.animals.push_back(AnimalSystem::create(AnimalType::COW, 10, 20));
        state.animals.push_back(AnimalSystem::create(AnimalType::SHEEP, 35, 20));
        state.animals.push_back(AnimalSystem::create(AnimalType::SHEEP, 40, 20));
        state.animals.push_back(AnimalSystem::create(AnimalType::BIRD, -10, 50));
        
        // Weather
        WeatherSystem::init(state.weather);
        state.currentWindSway = 0.0f;
        
        // Particles
        ParticleSystem::init(500);
        
        // Camera
        CameraSystem::init(state.camera);
        
        // Events & Analytics
        EventSystem::init(state.events);
        Analytics::init(state.metrics);
        state.metrics.active = true; // Enable by default for demo
    }

    void reshape(int w, int h) {
        state.width = w;
        state.height = h;
        glViewport(0, 0, w, h);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(-20.0, 80.0, 0.0, 60.0); // Expanded view
        glMatrixMode(GL_MODELVIEW);
    }

    void updateSkyColors() {
        float t = state.timeOfDay;
        float weatherIntensity = (state.weather.currentType == WeatherType::CLEAR) ? 0.0f : state.weather.intensity;
        
        // Use Style Palette for cleaner transitions
        // 1. Night (20-5)
        if (t < 5.0f || t >= 20.0f) {
            state.skyBottom = Style::Palette::SKY_NIGHT_BOT;
            state.skyTop = Style::Palette::SKY_NIGHT_TOP;
            state.starTwinkleOffset = sin(t); 
            // Ambient handled via Style::applyAtmosphere
        } 
        // 2. Dawn (5-7)
        else if (t >= 5.0f && t < 7.0f) {
            float factor = (t - 5.0f) / 2.0f;
            state.skyBottom = Utils::Color::lerp(Style::Palette::SKY_NIGHT_BOT, Style::Palette::SKY_DAWN_BOT, factor);
            state.skyTop = Utils::Color::lerp(Style::Palette::SKY_NIGHT_TOP, Style::Palette::SKY_DAWN_TOP, factor);
        } 
        // 3. Day (7-17)
        else if (t >= 7.0f && t < 17.0f) {
            // Day
            state.skyBottom = Style::Palette::SKY_DAY_BOT;
            state.skyTop = Style::Palette::SKY_DAY_TOP;
        } 
        // 4. Dusk (17-20)
        else if (t >= 17.0f && t < 20.0f) {
            float factor = (t - 17.0f) / 3.0f;
            state.skyBottom = Utils::Color::lerp(Style::Palette::SKY_DAY_BOT, Style::Palette::SKY_DUSK_BOT, factor);
            state.skyTop = Utils::Color::lerp(Style::Palette::SKY_DAY_TOP, Style::Palette::SKY_DUSK_TOP, factor);
        }
        
        // Calculate ambient light using Art Direction helper
        // This unifies the "Subtle saturation variation" rule
        Utils::Color baseAmbient(1.0f, 1.0f, 1.0f);
        state.ambientLight = Style::applyAtmosphere(baseAmbient, t, weatherIntensity);
    }

    void update(int value) {
        state.timeOfDay += state.timeSpeed;
        if (state.timeOfDay >= 24.0f) state.timeOfDay = 0.0f;
        
        // Weather Update
        // Weather Update
        WeatherSystem::update(state.weather, state.timeSpeed * 50.0f, state.width, state.height); // Scaling speed for weather
        
        // Season Cycle
        state.seasonTimer += state.timeSpeed;
        if (state.seasonTimer > 48.0f) { // Every 2 days
            state.seasonTimer = 0.0f;
            int s = (int)state.currentSeason + 1;
            if (s > 3) s = 0;
            state.currentSeason = (Utils::Season)s;
        }

        updateSkyColors();
        
        // Calculate Wind Sway
        state.currentWindSway = WeatherSystem::getWindSway(state.weather, state.timeOfDay + state.waveOffset);

        // Lighting Update
        std::vector<float> houseX, houseY;
        for(const auto& h : state.houses) {
            houseX.push_back(h.x);
            houseY.push_back(h.y);
        }
        LightingSystem::collectLights(state.lights, state.timeOfDay, houseX, houseY);

        // Particle Update
        ParticleSystem::update(state.timeSpeed, state.currentSeason, state.weather.windStrength, state.width, state.height);
        
        // Camera Update
        CameraSystem::updateCinematic(state.camera, state.timeOfDay);
        CameraSystem::update(state.camera);
        
        // Event Update
        EventSystem::update(state.events, state.timeSpeed, state.timeOfDay);
        
        // Analytics Update
        Analytics::update(state.metrics, (int)state.villagers.size() + (int)state.animals.size(), 500); // 500 fixed particles

        // Clouds (Multiple Layers + Weather Wind)
        for(int i = 0; i < 3; i++) {
            state.layers[i].x += state.layers[i].speed;
            state.layers[i].x += state.weather.windStrength * 0.01f; // Wind effect
        }

        // Boat
        state.boatX += 0.05f;
        if (state.boatX > 100) state.boatX = -30;
        
        state.waveOffset += 0.1f;
        
        glutPostRedisplay();
        glutTimerFunc(16, update, 0); // ~60 FPS
    }

    void display() {
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();
        
        // 0. Camera - Apply for World Objects
        glPushMatrix(); 
        CameraSystem::apply(state.camera);

        // 1. Atmospheric Sky & Stars (Draw HUGE to cover camera pan)
        glPushMatrix();
        glScalef(2.0f, 1.2f, 1.0f); 
        glTranslatef(-20, -10, 0); 
        SceneElements::drawSky(state.skyTop, state.skyBottom, state.timeOfDay);
        glPopMatrix();
        SceneElements::drawStars(state.timeOfDay, (state.timeOfDay > 19 || state.timeOfDay < 6) ? 0.8f : 0.0f);
        
        // 2. Celestial Bodies
        SceneElements::drawSunAndMoon(state.timeOfDay);
        
        // 3. Background Volumetric Clouds
        if (state.showClouds) 
            SceneElements::drawClouds(state.layers[1].x, state.layers[1].alpha, state.layers[1].scale, state.layers[1].y);

        // 4. Mountains
        SceneElements::drawMountains(0, state.ambientLight, state.currentSeason);

        // 5. Ground (Scale up X to cover pan)
        glPushMatrix();
        glScalef(3.0f, 1.0f, 1.0f);
        glTranslatef(-40, 0, 0);
        SceneElements::drawGround(state.width, state.height, state.ambientLight, state.currentSeason);
        glPopMatrix();

        // 6. River
        SceneElements::drawRiver(state.timeOfDay, state.skyBottom, state.ambientLight, state.currentSeason);
        
        // 7. House & Trees & Boat (Midground)
        SceneElements::drawBoat(state.boatX, 6 + sin(state.waveOffset * 0.5f) * 0.5f, state.ambientLight);
        
        // Update & Draw Buildings
        bool isNight = (state.timeOfDay < 6.0f || state.timeOfDay > 19.0f);
        for(size_t i = 0; i < state.houses.size(); ++i) {
            Building::update(state.houses[i], state.timeOfDay, isNight);
            Building::draw(state.houses[i], state.timeOfDay, state.ambientLight, state.currentSeason);
        }
        
        SceneElements::drawTree(-8, 20, state.ambientLight, state.currentWindSway, state.currentSeason);
        SceneElements::drawTree(60, 20, state.ambientLight, state.currentWindSway, state.currentSeason);
        
        // Update & Draw Characters
        float charSpeedMod = 1.0f;
        if (state.weather.currentType == WeatherType::RAIN) charSpeedMod = 0.7f;
        if (state.weather.currentType == WeatherType::STORM) charSpeedMod = 0.4f;
        
        for(size_t i = 0; i < state.villagers.size(); ++i) {
            CharacterSystem::update(state.villagers[i], state.timeOfDay, state.villagers, int(i), charSpeedMod);
            CharacterSystem::draw(state.villagers[i], state.ambientLight);
        }
        
        // Update & Draw Animals
        for(size_t i = 0; i < state.animals.size(); ++i) {
            AnimalSystem::update(state.animals[i], state.timeOfDay, isNight, state.currentWindSway);
            AnimalSystem::draw(state.animals[i], state.ambientLight);
        }
        
        // 8. Foreground Clouds
        if (state.showClouds) {
             SceneElements::drawClouds(state.layers[0].x, state.layers[0].alpha, state.layers[0].scale, state.layers[0].y);
             SceneElements::drawClouds(state.layers[2].x, state.layers[2].alpha, state.layers[2].scale, state.layers[2].y);
        }
           
        // 9. Details
        SceneElements::drawFoliage(-12, 18, state.ambientLight, state.currentWindSway, state.currentSeason);
        SceneElements::drawFoliage(2, 18, state.ambientLight, state.currentWindSway, state.currentSeason);
        SceneElements::drawFlowers(10, 15, state.ambientLight, state.currentWindSway, state.currentSeason);
        SceneElements::drawFlowers(12, 16, state.ambientLight, state.currentWindSway, state.currentSeason);
        
        // 10. Birds (Particles/Elements)
        if (state.showBirds && (state.timeOfDay > 6 && state.timeOfDay < 19)) {
             float bx = -20 + fmod(state.timeOfDay * 15, 120); 
             float by = 45 + sin(bx * 0.1f) * 4.0f;
             SceneElements::drawBird(bx, by, sin(state.waveOffset), state.ambientLight);
        }
        
        // 11. Weather/Particles (World Space)
        WeatherSystem::draw(state.weather, state.width, state.height);
        ParticleSystem::draw(state.timeOfDay);
        EventSystem::drawWorld(state.events);
        
        // --- Population Heatmap (Visual Data Overlay) ---
        if (state.metrics.showHeatmap) {
            std::vector<float> xs;
            for(auto& v : state.villagers) xs.push_back(v.x);
            for(auto& a : state.animals) xs.push_back(a.x);
            Analytics::drawHeatmap(xs, std::vector<float>(xs.size(), 10.0f));
        }

        glPopMatrix(); // End World Space (Camera)
        
        // 12. Lighting Overlay & Effects
        glPushMatrix();
        CameraSystem::apply(state.camera);
        LightingSystem::drawLightingOverlay(state.width, state.height, state.timeOfDay, state.ambientLight, state.lights);
        LightingSystem::drawGodRays(state.width, state.height, state.timeOfDay);
        LightingSystem::drawBloom(state.lights);
        glPopMatrix();

        // 13. Screen Space Overlays (Fireflies, Analytics)
        // Set Pixel-perfect 2D Projection for UI
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, state.width, 0, state.height);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        EventSystem::drawScreen(state.events, state.width, state.height);
        Analytics::draw(state.metrics, state.width, state.height);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glFlush();
        glutSwapBuffers();
    }

    void handleKeyboard(unsigned char key, int x, int y) {
        switch (key) {
        case 'n': case 'N':
            state.timeSpeed = 0.5f; // Fast forward
            break;
        case 'm': case 'M':
            state.timeSpeed = 0.01f; // Normal
            break;
        case 'g': case 'G':
            state.metrics.active = !state.metrics.active;
            break;
        case 'h': case 'H':
            state.metrics.showHeatmap = !state.metrics.showHeatmap;
            break;
        case 'c': case 'C':
            state.showClouds = !state.showClouds;
            break;
        case 'b': case 'B':
            state.showBirds = !state.showBirds;
            break;
        case 27: // ESC
            exit(0);
            break;
        }
        glutPostRedisplay();
    }
}

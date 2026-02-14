#include "Building.h"
#include <GL/glut.h>
#include <cstdlib>
#include "Style.h"
#include "Particles.h"

namespace Building {

    BuildingProps create(float x, float y) {
        BuildingProps p;
        p.x = x;
        p.y = y;
        
        // Random dimensions (Variation)
        p.width = Utils::random(8.0f, 14.0f);
        p.height = Utils::random(8.0f, 12.0f);
        p.roofHeight = Utils::random(5.0f, 8.0f);
        
        // Random Colors from Style Palette
        bool isStone = (rand() % 4 == 0);
        if (isStone) {
            p.wallColor = Style::Palette::STONE_GRAY;
        } else {
            p.wallColor = (rand() % 2 == 0) ? Style::Palette::WALL_WOOD_LIGHT : Style::Palette::WALL_WOOD_DARK;
        }
        
        p.roofColor = (rand() % 2 == 0) ? Style::Palette::ROOF_RED : Style::Palette::ROOF_THATCH;
        p.doorColor = Utils::Color(0.4f, 0.2f, 0.1f);
        
        p.hasChimney = (rand() % 2 == 0);
        p.lightOn = false;
        p.doorAngle = 0.0f;
        
        return p;
    }

    void update(BuildingProps& props, float time, bool isNight) {
        // Smoke Spawning
        if (props.hasChimney) {
            if (rand() % 10 < 3) {
                ParticleSystem::spawnSmoke(props.x + props.width - 3, props.y + props.height + 6);
            }
        }
        
        // Light Emission
        // Random flickering for window light
        if (isNight) {

            if (rand() % 100 < 2) props.lightOn = !props.lightOn; // Rare toggle
             // Or just always on with flicker? Let's say mostly on at night.
            props.lightOn = true; 
        } else {
            props.lightOn = false;
        }
        
        // Door Animation (Cycle)
        // Open during day sometimes?
        if (!isNight && (int(time) % 10 < 3)) {
             props.doorAngle = Utils::lerp(props.doorAngle, 1.0f, 0.05f);
        } else {
             props.doorAngle = Utils::lerp(props.doorAngle, 0.0f, 0.05f);
        }
    }

    void draw(BuildingProps& p, float time, const Utils::Color& ambientLight, Utils::Season season) {
        float x = p.x;
        float y = p.y;
        float w = p.width;
        float h = p.height;
        
        // Shadow (AO / Soft Shadow)
        Style::drawSoftShadow(x + w/2, y, w * 0.7f, 0.3f);
        
        // Walls (Atmospheric)
        Utils::Color wallC = Style::applyAtmosphere(p.wallColor, time, (season == Utils::Season::WINTER?0.2f:0.0f));
        Utils::drawRect(x, y, x + w, y + h, wallC);
        
        // Roof
        Utils::Color rColor = p.roofColor;
        if (season == Utils::Season::WINTER) rColor = Style::Palette::LEAF_WINTER; // Snow covered roof color
        
        Utils::Color roofC = Style::applyAtmosphere(rColor, time, (season == Utils::Season::WINTER?0.2f:0.0f));
        roofC.apply();
        glBegin(GL_TRIANGLES);
        glVertex2f(x - 2, y + h);
        glVertex2f(x + w/2, y + h + p.roofHeight);
        glVertex2f(x + w + 2, y + h);
        glEnd();
        
        // Chimney (Base only, smoke handled by ParticleSystem)
        if (p.hasChimney) {
            Utils::Color brick(0.6f, 0.3f, 0.2f);
            Utils::drawRect(x + w - 4, y + h + 2, x + w - 2, y + h + 6, Utils::Color::lerp(brick, ambientLight, 0.3f));
        }
        
        // Door (Animated)
        float doorW = 4.0f;
        float doorH = 6.0f;
        float doorX = x + w/2 - doorW/2;
        
        // Door Frame (Dark Hole behind)
        glColor3f(0.1f, 0.05f, 0.0f); 
        Utils::drawRect(doorX, y, doorX+doorW, y+doorH, Utils::Color(0.1f, 0.1f, 0.1f));
        
        // The Door itself (Projected differently based on angle)
        // Simple scaled width to simulate opening
        float openFactor = 1.0f - p.doorAngle * 0.8f; // Width shrinks as it opens 'inwards'
        Utils::Color doorC = Utils::Color::lerp(p.doorColor, ambientLight, 0.3f);
        Utils::drawRect(doorX, y, doorX + doorW * openFactor, y + doorH, doorC);
        
        // Window (Light Emission)
        float winSize = 3.0f;
        float winX = x + 2.0f;
        float winY = y + h/2;
        
        Utils::Color winColor(0.2f, 0.3f, 0.4f); // Dark/Day glass
        if (p.lightOn) {
            // Flicker logic
             float f = Utils::random(0.9f, 1.0f);
             winColor = Utils::Color(1.0f * f, 0.9f * f, 0.5f * f, 0.9f);
        }
        
        Utils::drawRect(winX, winY, winX+winSize, winY+winSize, winColor);
        
        // Add Bloom/Glow for active light
        if (p.lightOn) {
            glEnable(GL_BLEND);
            glColor4f(1.0f, 0.9f, 0.5f, 0.2f);
            Utils::drawCircle(winSize * 2.0f, winX + winSize/2, winY + winSize/2, 20, true);
            glDisable(GL_BLEND);
        }
    }
}

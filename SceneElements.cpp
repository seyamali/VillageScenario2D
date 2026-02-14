#include "SceneElements.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include "Style.h"

namespace SceneElements {

    // Helper for applying tints inside drawing functions
    void applyTintedColor(const Utils::Color& base, const Utils::Color& tint, float factor) {
        Utils::Color final = Utils::Color::lerp(base, tint, factor);
        final.apply();
    }

    void drawSky(const Utils::Color& top, const Utils::Color& bottom, float timeOfDay) {
        Utils::drawGradientRect(-20, 30, 80, 60, bottom, top, true);
    }
    
    void drawStars(float time, float intensity) {
        if (intensity <= 0.05f) return;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glPointSize(2.0f);
        glBegin(GL_POINTS);
        
        // Simple procedural star field based on pseudo-random coordinates
        for(int i = 0; i < 50; i++) {
             float x = (i * 13) % 100 - 20;
             float y = (i * 7) % 30 + 35;
             float twinkle = sin(time * 5.0f + i) * 0.5f + 0.5f; // 0 to 1
             
             glColor4f(1.0f, 1.0f, 1.0f, intensity * twinkle);
             glVertex2f(x, y);
        }
        glEnd();
        glDisable(GL_BLEND);
    }

    void drawSunAndMoon(float timeOfDay) {
        // Time 0-24. 
        // Sun logic: 5:00 to 19:00
        float sunX = -50, sunY = -10, sunAlpha = 0;
        float moonX = -50, moonY = -10, moonAlpha = 0;
        
        if (timeOfDay >= 5.0f && timeOfDay <= 19.0f) {
            float t = (timeOfDay - 5.0f) / 14.0f; // 0 to 1
            float angle = t * M_PI; // 0 to PI
            sunX = -20.0f + t * 100.0f; 
            sunY = 25.0f + sin(angle) * 35.0f;
            sunAlpha = 1.0f;
            if (t < 0.2f) sunAlpha = t * 5.0f; 
            if (t > 0.8f) sunAlpha = (1.0f - t) * 5.0f; 
        }
        
        // Moon logic: 18:00 to 6:00
        float moonTime = timeOfDay;
        if (moonTime < 6.0f) moonTime += 24.0f; 
        
        if (moonTime >= 18.0f && moonTime <= 30.0f) {
             float t = (moonTime - 18.0f) / 12.0f;
             float angle = t * M_PI;
             moonX = -20.0f + t * 100.0f;
             moonY = 25.0f + sin(angle) * 35.0f;
             moonAlpha = 1.0f;
             if (t < 0.1f) moonAlpha = t * 10.0f;
             if (t > 0.9f) moonAlpha = (1.0f - t) * 10.0f;
        }

        // Draw Sun
        if (sunAlpha > 0) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            // Core
            glColor4f(1.0f, 0.9f, 0.2f, sunAlpha); 
            Utils::drawCircle(4.0f, sunX, sunY, 50, true);
            
             // Rays
            glColor4f(1.0f, 0.6f, 0.0f, 0.3f * sunAlpha);
            Utils::drawCircle(6.0f, sunX, sunY, 50, true);
            
            glColor4f(1.0f, 0.5f, 0.0f, 0.1f * sunAlpha);
            Utils::drawCircle(10.0f, sunX, sunY, 50, true);
            
            glDisable(GL_BLEND);
        }

        // Draw Moon
        if (moonAlpha > 0) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            // Glow
            glColor4f(0.9f, 0.9f, 1.0f, 0.2f * moonAlpha);
            Utils::drawCircle(5.0f, moonX, moonY, 30, true);
            
            // Body
            glColor4f(0.9f, 0.9f, 0.95f, moonAlpha); 
            Utils::drawCircle(3.0f, moonX, moonY, 40, true);
            
            // Craters
            glColor4f(0.7f, 0.7f, 0.8f, moonAlpha);
            Utils::drawCircle(0.6f, moonX - 0.5f, moonY + 0.5f, 20, true);
            Utils::drawCircle(0.8f, moonX + 0.8f, moonY - 0.5f, 20, true);
            
            glDisable(GL_BLEND);
        }
    }
    
    void drawClouds(float offset, float alpha, float scale, float yPos) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, alpha);
        
        float baseX = offset;
        // Simple wrap logic handled by caller or cyclic visual
        // Let's just draw relative to offset
        
        glPushMatrix();
        glTranslatef(baseX, yPos, 0);
        glScalef(scale, scale, 1.0f);
        
        Utils::drawCircle(1.0f, 0, 0, 20, true);
        Utils::drawCircle(0.8f, -1.2f, -0.5f, 20, true);
        Utils::drawCircle(0.9f, 1.1f, -0.3f, 20, true);
        Utils::drawCircle(0.7f, 0.5f, 0.6f, 20, true);
        
        // Draw a second cloud cluster nearby for density
        glTranslatef(3.5f, 0.5f, 0);
        Utils::drawCircle(0.9f, 0, 0, 20, true);
        Utils::drawCircle(0.7f, -1.0f, -0.4f, 20, true);
        
        glPopMatrix();
        glDisable(GL_BLEND);
    }
    
    void drawMountains(float parallaxOffset, const Utils::Color& tint, Utils::Season season) {       
        // Background Layer
        float bgOffset = parallaxOffset * 0.5f;
        Utils::Color bgBase(0.4f, 0.45f, 0.5f);
        if (season == Utils::Season::SPRING) bgBase = Utils::Color(0.4f, 0.55f, 0.45f);
        if (season == Utils::Season::AUTUMN) bgBase = Utils::Color(0.6f, 0.5f, 0.4f);
        if (season == Utils::Season::WINTER) bgBase = Utils::Color(0.7f, 0.75f, 0.85f);
        
        Utils::Color bgColor = Style::applyAtmosphere(bgBase, 12, 0.2f); // Slightly desaturated distant mountains
        bgColor.r *= tint.r; bgColor.g *= tint.g; bgColor.b *= tint.b;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(bgColor.r, bgColor.g, bgColor.b, 1.0f);
        
        glBegin(GL_POLYGON);
        glVertex2f(-30 + bgOffset, 25);
        glVertex2f(0 + bgOffset, 50); 
        glVertex2f(30 + bgOffset, 28);
        glVertex2f(60 + bgOffset, 55);
        glVertex2f(100 + bgOffset, 25);
        glEnd();
        
        // Foreground Layer
        Utils::Color fgBase = Style::getSeasonalColor(season, 
            Style::Palette::GRASS_SPRING, Style::Palette::GRASS_SUMMER, 
            Style::Palette::GRASS_AUTUMN, Style::Palette::GRASS_WINTER);
            
        Utils::Color fgColor = Style::applyAtmosphere(fgBase, 12, 0.1f);
        fgColor.r *= tint.r; fgColor.g *= tint.g; fgColor.b *= tint.b;
        
        glColor4f(fgColor.r * 0.8f, fgColor.g * 0.8f, fgColor.b * 0.8f, 1.0f);
        
        glBegin(GL_POLYGON);
        glVertex2f(-20, 25);
        glVertex2f(-5, 42);
        glVertex2f(15, 23);
        glVertex2f(35, 45);
        glVertex2f(55, 24);
        glVertex2f(80, 25);
        glEnd();
        
        glDisable(GL_BLEND);
    }
    
    void drawGround(int width, int height, const Utils::Color& tint, Utils::Season season) {
        Utils::Color groundBase = Style::getSeasonalColor(season, 
            Style::Palette::GRASS_SPRING, Style::Palette::GRASS_SUMMER, 
            Style::Palette::GRASS_AUTUMN, Style::Palette::GRASS_WINTER);
            
        Utils::Color g1 = groundBase;
        Utils::Color g2 = Utils::Color(groundBase.r * 0.8f, groundBase.g * 0.8f, groundBase.b * 0.8f);
        
        // Apply tint (Ambient)
        g1.r *= tint.r; g1.g *= tint.g; g1.b *= tint.b;
        g2.r *= tint.r; g2.g *= tint.g; g2.b *= tint.b;
        
        Utils::drawGradientRect(-20, 0, 80, 25, g2, g1, true); 
    }

    void drawRiver(float time, const Utils::Color& skyColor, const Utils::Color& tint, Utils::Season season) {
        Utils::Color waterTop = Style::Palette::WATER_RIVER;
        Utils::Color waterBot = Style::Palette::WATER_DEEP;
        
        if (season == Utils::Season::WINTER) {
            waterTop = Utils::Color(0.9f, 0.95f, 1.0f);
            waterBot = Utils::Color(0.7f, 0.8f, 0.9f);
        }
        
        // Reflect sky
        waterTop = Utils::Color::lerp(waterTop, skyColor, 0.3f);
        
        // Apply tint
        waterTop.r *= tint.r; waterTop.g *= tint.g; waterTop.b *= tint.b;
        waterBot.r *= tint.r; waterBot.g *= tint.g; waterBot.b *= tint.b;

        Utils::drawGradientRect(-20, 0, 80, 15, waterBot, waterTop, true);
        
        if (season != Utils::Season::WINTER) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, 0.2f); // Subtle ripples
            glLineWidth(1.5f);
            glBegin(GL_LINES);
            for(int i = 0; i < 25; i++) {
                float y = 1.0f + i * 0.5f;
                float speed = 1.0f + (i % 4) * 0.3f;
                float xOffset = sin(time * 0.4f * speed + i) * 2.0f;
                
                float xStart = -20 + ((i * 19) % 100) + xOffset;
                glVertex2f(xStart, y);
                glVertex2f(xStart + 3.0f, y);
            }
            glEnd();
            glLineWidth(1.0f);
            glDisable(GL_BLEND);
        }
    }

    void drawHouse(float x, float y, const Utils::Color& tint, Utils::Season season) {
        // House Shadow (Fake AO)
        glEnable(GL_BLEND);
        glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
        Utils::drawCircle(6.0f, x + 6, y, 20, true);
        glDisable(GL_BLEND);

        // Walls
        Utils::Color wallColor = Utils::Color::lerp(Utils::Color(0.8f, 0.6f, 0.4f), tint, 0.3f);
        Utils::drawRect(x, y, x + 12, y + 10, wallColor);
        
        // Roof
        Utils::Color rBase(0.6f, 0.2f, 0.1f);
        if (season == Utils::Season::WINTER) rBase = Utils::Color(0.9f, 0.9f, 0.95f); // Snow roof
        Utils::Color roofColor = Utils::Color::lerp(rBase, tint, 0.3f);
        roofColor.apply();
        glBegin(GL_TRIANGLES);
        glVertex2f(x - 2, y + 10);
        glVertex2f(x + 6, y + 16); 
        glVertex2f(x + 14, y + 10);
        glEnd();
        
        // Door
        Utils::Color doorColor = Utils::Color::lerp(Utils::Color(0.4f, 0.2f, 0.1f), tint, 0.3f);
        Utils::drawRect(x + 4, y, x + 8, y + 6, doorColor);
        
        // Window
        Utils::Color winColor = Utils::Color::lerp(Utils::Color(0.6f, 0.8f, 1.0f), tint, 0.1f);
        // Windows might be lit up at night?
        bool nightLight = (tint.r < 0.3f); // Heuristic
        if (nightLight) winColor = Utils::Color(1.0f, 0.9f, 0.5f, 0.9f); // Lit window
        
        Utils::drawRect(x + 1, y + 5, x + 3, y + 8, winColor);
        Utils::drawRect(x + 9, y + 5, x + 11, y + 8, winColor);
    }
    
    void drawTree(float x, float y, const Utils::Color& tint, float sway, Utils::Season season) {
        // Shadow
        glEnable(GL_BLEND);
        glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
        Utils::drawCircle(2.0f, x, y, 10, true);
        glDisable(GL_BLEND);

         // Trunk (Slight lean?)
        Utils::drawRect(x - 1.5, y, x + 1.5, y + 8, Utils::Color::lerp(Utils::Color(0.4f, 0.25f, 0.15f), tint, 0.4f));
        
        // Leaves (Sway)
        float sway1 = sway * 1.0f;
        float sway2 = sway * 1.5f;
        
        Utils::Color lBase(0.1f, 0.5f, 0.1f);
        if (season == Utils::Season::SPRING) lBase = Utils::Color(0.2f, 0.7f, 0.2f);
        if (season == Utils::Season::AUTUMN) lBase = Utils::Color(0.8f, 0.4f, 0.1f);
        if (season == Utils::Season::WINTER) lBase = Utils::Color(0.9f, 0.9f, 0.95f); // Snowy

        Utils::Color leafColor = Utils::Color::lerp(lBase, tint, 0.3f);
        leafColor.apply();
        
        if (season != Utils::Season::WINTER || true) { // Draw 'leaves' as snow clumps if winter
             Utils::drawCircle(3.5f, x + sway1, y + 8, 20, true);
             Utils::drawCircle(3.0f, x - 2.5 + sway1, y + 6.5, 20, true);
             Utils::drawCircle(3.0f, x + 2.5 + sway1, y + 6.5, 20, true);
             Utils::drawCircle(2.5f, x - 1.5 + sway2, y + 10, 20, true);
             Utils::drawCircle(2.5f, x + 1.5 + sway2, y + 10, 20, true);
        } else {
            // Bare tree logic could go here, but snowy clumps look nicer for now
             Utils::drawCircle(3.5f, x + sway1, y + 8, 20, true);
        }
        
        // Highlight logic... skipped for brevity or adjusted
    }
    
    void drawBoat(float x, float y, const Utils::Color& tint) {
        Utils::Color hull = Utils::Color::lerp(Utils::Color(0.5f, 0.3f, 0.1f), tint, 0.4f);
        hull.apply();
        glBegin(GL_POLYGON);
        glVertex2f(x - 6, y);
        glVertex2f(x - 4, y - 3);
        glVertex2f(x + 4, y - 3);
        glVertex2f(x + 6, y);
        glEnd();
        
        Utils::Color sail = Utils::Color::lerp(Utils::Color(0.9f, 0.9f, 0.9f), tint, 0.1f);
        sail.apply();
        glBegin(GL_TRIANGLES);
        glVertex2f(x, y);       
        glVertex2f(x, y + 10); 
        glVertex2f(x + 5, y + 2);
        glEnd();
        
        applyTintedColor(Utils::Color(0.2f, 0.1f, 0.05f), tint, 0.4f); // Mast
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x, y + 10);
        glEnd();
        glLineWidth(1.0f);
    }
    
    void drawBird(float x, float y, float wingAngle, const Utils::Color& tint) {
        Utils::Color birdColor = Utils::Color::lerp(Utils::Color(1.0f, 1.0f, 1.0f), tint, 0.1f);
        birdColor.apply();
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(x - 2, y + sin(wingAngle));
        glVertex2f(x, y);
        glVertex2f(x + 2, y + sin(wingAngle));
        glEnd();
        glLineWidth(1.0f);
    }
    
    void drawFoliage(float x, float y, const Utils::Color& tint, float sway, Utils::Season season) {
         if (season == Utils::Season::WINTER) return; // Dead/Buried in snow
         
         Utils::Color bBase(0.1f, 0.6f, 0.1f);
         if (season == Utils::Season::AUTUMN) bBase = Utils::Color(0.7f, 0.5f, 0.2f);
         
         Utils::Color bush = Utils::Color::lerp(bBase, tint, 0.4f);
         bush.apply();
         float s = sway * 0.5f;
         Utils::drawCircle(1.0f, x + s, y, 10, true);
         Utils::drawCircle(1.2f, x + 1 + s, y, 10, true);
         Utils::drawCircle(1.0f, x + 2 + s, y, 10, true);
    }
    
    void drawFlowers(float x, float y, const Utils::Color& tint, float sway, Utils::Season season) {
        if (season == Utils::Season::WINTER || season == Utils::Season::AUTUMN) return; // Only spring/summer
        
        // Stem (Sway)
        glColor3f(0.0f, 0.6f, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + sway, y + 3);
        glEnd();
        
        // Flower Head
        Utils::Color petal = Utils::Color::lerp(Utils::Color(1.0f, 0.0f, 0.0f), tint, 0.2f);
        petal.apply();
        Utils::drawCircle(0.8f, x + sway, y + 3, 10, true);
        
        Utils::Color center = Utils::Color::lerp(Utils::Color(1.0f, 1.0f, 0.0f), tint, 0.2f);
        center.apply();
        Utils::drawCircle(0.3f, x + sway, y + 3, 10, true);
    }
}

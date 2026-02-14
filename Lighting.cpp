#include "Lighting.h"
#include <GL/glut.h>
#include <cmath>
#include <iostream>

namespace LightingSystem {

    void collectLights(std::vector<LightSource>& lights, float timeOfDay, 
                       const std::vector<float>& houseX, const std::vector<float>& houseY) {
        // Collect house window lights (only at night - 18:00 to 06:00)
        bool isNight = (timeOfDay < 6.0f || timeOfDay > 18.0f);
        
        lights.clear();
        
        if (isNight) {
            for (size_t i = 0; i < houseX.size(); ++i) {
                // Window light 1
                LightSource l1;
                l1.x = houseX[i] + 2; l1.y = houseY[i] + 6.5f;
                l1.radius = 8.0f;
                l1.color = Utils::Color(1.0f, 0.9f, 0.4f, 0.8f); // Warm yellow
                l1.active = true;
                lights.push_back(l1);
                
                // Window light 2
                LightSource l2;
                l2.x = houseX[i] + 10; l2.y = houseY[i] + 6.5f;
                l2.radius = 8.0f;
                l2.color = Utils::Color(1.0f, 0.9f, 0.4f, 0.8f);
                l2.active = true;
                lights.push_back(l2);
            }
            
            // Random ambient fireflies?
            // Static for now, but could be dynamic
        }
    }

    void drawLightingOverlay(int width, int height, float timeOfDay, 
                             const Utils::Color& ambientLight, 
                             const std::vector<LightSource>& lights) {
                             
        // Darkness Factor: How dark is the overlay?
        // 0.0 = Full Day, 0.7 = Full Night
        float darkness = 0.0f;
        
        if (timeOfDay >= 19.0f) {
             // Sunset to Night (19-24)
             // Transition 19-21
             if (timeOfDay < 21.0f) darkness = (timeOfDay - 19.0f) / 2.0f * 0.7f;
             else darkness = 0.7f;
        } else if (timeOfDay < 5.0f) {
             // Deep Night (0-5)
             darkness = 0.7f;
        } else if (timeOfDay >= 5.0f && timeOfDay < 7.0f) {
             // Sunrise (5-7)
             darkness = (1.0f - (timeOfDay - 5.0f) / 2.0f) * 0.7f;
        }
        
        if (darkness <= 0.05f) return; // Almost day, no overlay
        
        // We need blending: DEST * SRC (Multiplicative) or similar?
        // Standard approach for 2D lighting: Draw full screen dark quad, 
        // subtract/punch holes using alpha channel, or use specialized blending.
        // Simple approach: Draw dark quad with holes? No, hard in fixed pipeline.
        // Alternative: Draw full dark quad with transparency, then draw light sprites on top 
        // with Additive Blending to "brighten" the area.
        
        // 1. Draw Darkness (Multiply or Alpha Subtraction)
        // With standard glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), drawing black @ 0.7 alpha darkens everything.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Base Darkness Color (Dark Blue/Purple Night)
        glColor4f(0.0f, 0.05f, 0.15f, darkness); 
        
        glBegin(GL_QUADS);
        glVertex2f(-100, -100);
        glVertex2f(width + 100, -100);
        glVertex2f(width + 100, height + 100);
        glVertex2f(-100, height + 100);
        glEnd();
        
        // 2. Draw Lights (Additive Blending) to cancel out darkness and add glow
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive
        
        for (const auto& l : lights) {
            if (!l.active) continue;
            
            // Draw radial gradient (center opaque, edge transparent)
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(l.color.r, l.color.g, l.color.b, l.color.a * 0.8f); // Center
            glVertex2f(l.x, l.y);
            
            glColor4f(0.0f, 0.0f, 0.0f, 0.0f); // Edge (Black adds nothing)
            int segments = 20;
            for (int i = 0; i <= segments; ++i) {
                float theta = 2.0f * 3.14159f * float(i) / float(segments);
                float dx = l.radius * cos(theta);
                float dy = l.radius * sin(theta);
                glVertex2f(l.x + dx, l.y + dy);
            }
            glEnd();
        }
        
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Reset
    }
    
    void drawGodRays(int width, int height, float timeOfDay) {
        // Only during sunrise (5-8) and sunset (17-20)
        float intensity = 0.0f;
        float sunAngle = 0.0f; // For direction
        Utils::Color rayColor(1.0f, 0.9f, 0.6f, 0.0f);
        
        // Sunrise: 5 to 7. Peak 6.
        if (timeOfDay >= 5.0f && timeOfDay < 7.0f) {
            intensity = 1.0f - abs(timeOfDay - 6.0f) / 1.0f; // 0 at 5, 1 at 6, 0 at 7
            sunAngle = -0.5f; 
            rayColor = Utils::Color(1.0f, 0.8f, 0.5f, 0.0f);
        } 
        // Sunset: 17 to 19. Peak 18.
        else if (timeOfDay >= 17.0f && timeOfDay < 19.0f) {
            intensity = 1.0f - abs(timeOfDay - 18.0f) / 1.0f;
            sunAngle = 0.5f; 
            rayColor = Utils::Color(1.0f, 0.5f, 0.2f, 0.0f);
        }
        
        if (intensity <= 0.05f) return;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for light shafts
        
        // Ray Origin (Sun Position Approx)
        float sourceX = (sunAngle < 0) ? -20.0f : width + 20.0f;
        float sourceY = height * 0.8f;
        
        // Draw dramatic triangles from sun position towards ground
        glColor4f(rayColor.r, rayColor.g, rayColor.b, intensity * 0.2f);
        
        glBegin(GL_TRIANGLES);
        // Ray 1
        glVertex2f(sourceX, sourceY); // Source
        glVertex2f(width * 0.3f, -50);
        glVertex2f(width * 0.1f, -50);
        
        // Ray 2
        glVertex2f(sourceX, sourceY);
        glVertex2f(width * 0.7f, -50);
        glVertex2f(width * 0.5f, -50);
        
        // Ray 3
        glVertex2f(sourceX, sourceY);
        glVertex2f(width * 0.9f, -50);
        glVertex2f(width * 0.8f, -50);
        
        glEnd();
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void drawBloom(const std::vector<LightSource>& lights) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
        for (const auto& l : lights) {
            if (!l.active) continue;
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(l.color.r, l.color.g, l.color.b, 0.15f);
            glVertex2f(l.x, l.y);
            glColor4f(0, 0, 0, 0);
            for(int i=0; i<=16; i++) {
                float a = 2.0f * 3.14159f * i / 16;
                glVertex2f(l.x + cos(a) * l.radius * 2.5f, l.y + sin(a) * l.radius * 2.5f);
            }
            glEnd();
        }
        glDisable(GL_BLEND);
    }

}

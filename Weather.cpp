#include "Weather.h"
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include <iostream>

WeatherState::WeatherState() : 
    currentType(WeatherType::CLEAR), 
    intensity(0.0f), 
    windStrength(0.0f),
    fogDensity(0.0f),
    lightningTimer(0.0f),
    isLightningActive(false),
    transitionTimer(0.0f)
{
    particles.resize(500); // 500 particles
}

namespace WeatherSystem {

    void init(WeatherState& state) {
        // Initialize particle pool
        for(auto& p : state.particles) {
            p.active = false;
        }
    }

    void update(WeatherState& state, float timeSpeed, int width, int height) { // Updated signature
        state.transitionTimer += timeSpeed;
        
        // Random Weather Transitions
        if (state.transitionTimer > 50.0f) { // Every ~50 units of time
            state.transitionTimer = 0;
            int r = rand() % 100;
            if (r < 60) {
                state.currentType = WeatherType::CLEAR;
                state.intensity = 0.0f;
                state.windStrength = Utils::random(-0.5f, 0.5f);
            } else if (r < 80) {
                state.currentType = WeatherType::RAIN;
                state.intensity = 0.7f;
                state.windStrength = Utils::random(1.0f, 3.0f);
            } else if (r < 90) { // Storm
                state.currentType = WeatherType::STORM;
                state.intensity = 1.0f;
                state.windStrength = Utils::random(4.0f, 6.0f);
            } else { // Snow
                state.currentType = WeatherType::SNOW;
                state.intensity = 0.5f;
                state.windStrength = Utils::random(-1.0f, 1.0f);
            }
        }
        
        // Update Particles
        if (state.currentType != WeatherType::CLEAR) {
            int activeCount = int(state.intensity * state.particles.size());
            
            for(size_t i = 0; i < state.particles.size(); ++i) {
                
                // If this particle is meant to be active but isn't, respawn it
                if (i < size_t(activeCount)) {
                    if (!state.particles[i].active) {
                        state.particles[i].active = true;
                        state.particles[i].x = Utils::random(-20.0f, width + 20.0f); // Screen width coords
                        state.particles[i].y = height + Utils::random(0.0f, 20.0f); // Above
                        
                        if (state.currentType == WeatherType::SNOW) {
                            state.particles[i].speedY = Utils::random(0.1f, 0.3f);
                            state.particles[i].speedX = state.windStrength * 0.2f + Utils::random(-0.1f, 0.1f);
                        } else { // Rain
                            state.particles[i].speedY = Utils::random(1.0f, 2.5f);
                            state.particles[i].speedX = state.windStrength * 0.3f;
                        }
                    } else {
                        // Move
                        state.particles[i].y -= state.particles[i].speedY;
                        state.particles[i].x += state.particles[i].speedX;

                        // Wrap or Reset
                        if (state.particles[i].y < 0) {
                            state.particles[i].y = height + Utils::random(0.0f, 10.0f);
                            state.particles[i].x = Utils::random(-20.0f, width + 20.0f);
                        }
                    }
                } else {
                    state.particles[i].active = false;
                }
            }
        } else {
            // Clear all
            for(auto& p : state.particles) p.active = false;
        }
        
        // Lightning
        state.isLightningActive = false;
        if (state.currentType == WeatherType::STORM) {
            state.lightningTimer -= 0.1f;
            if (state.lightningTimer <= 0) {
                 if (rand() % 100 < 2) { // Random flash chance (2%)
                     state.isLightningActive = true;
                     state.lightningTimer = Utils::random(5.0f, 15.0f); // Cooldown
                 }
            }
        }
        
        // Fog
        if (state.currentType == WeatherType::RAIN || state.currentType == WeatherType::STORM) {
            state.fogDensity = 0.3f;
        } else {
            state.fogDensity = 0.0f;
        }
    }
    
    void draw(const WeatherState& state, int width, int height) {
        // Lightning Flash (Full Screen)
        if (state.isLightningActive) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            // Full screen flash
            glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
            glBegin(GL_QUADS);
            glVertex2f(-100, -100); glVertex2f(width + 100, -100);
            glVertex2f(width + 100, height + 100); glVertex2f(-100, height + 100);
            glEnd();
            
            // Jagged Bolt
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glLineWidth(2.5f);
            float startX = Utils::random(0, width);
            float startY = height;
            glBegin(GL_LINE_STRIP);
            glVertex2f(startX, startY);
            for (int i = 1; i <= 5; i++) {
                startX += Utils::random(-10.0f, 10.0f);
                startY -= height / 5.0f;
                glVertex2f(startX, startY);
            }
            glEnd();
            glLineWidth(1.0f);
            
            glDisable(GL_BLEND);
        }
        
        // Particles
        if (state.currentType != WeatherType::CLEAR) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
             
            if (state.currentType == WeatherType::SNOW) {
                glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
                 for(const auto& p : state.particles) {
                    if(p.active) Utils::drawCircle(0.5f, p.x, p.y, 10, true);
                }
            } else { // Rain
                glLineWidth(1.5f);
                glColor4f(0.6f, 0.7f, 1.0f, 0.5f);
                glBegin(GL_LINES);
                 for(const auto& p : state.particles) {
                    if(p.active) {
                        glVertex2f(p.x, p.y);
                        glVertex2f(p.x - p.speedX * 2.0f, p.y + p.speedY * 2.0f); // Tail (upwards)
                    }
                }
                glEnd();
                glLineWidth(1.0f);
            }
            glDisable(GL_BLEND);
        }
        
        // Fog Overlay (Gradient for depth)
        if (state.fogDensity > 0.0f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            Utils::Color fogCol(0.7f, 0.7f, 0.8f, state.fogDensity);
            Utils::Color fogTop(0.7f, 0.7f, 0.8f, 0.0f); // Fades out at top
            
            Utils::drawGradientRect(-50, 0, width + 100, height, fogCol, fogTop, true);
            glDisable(GL_BLEND);
        }
    }
    
    float getWindSway(const WeatherState& state, float time) {
        float baseSway = sin(time * 0.1f) * 0.2f;
        if (state.currentType == WeatherType::STORM) {
            return baseSway + sin(time * 3.0f) * 1.5f + state.windStrength * 0.5f;
        } else if (state.currentType == WeatherType::RAIN) {
             return baseSway + sin(time * 1.5f) * 0.8f + state.windStrength * 0.3f;
        }
        return baseSway + sin(time * 0.5f) * 0.3f; // Gentle
    }
}

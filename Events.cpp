#include "Events.h"
#include <GL/glut.h>
#include <cstdlib>
#include <iostream>
#include "Utils.h"

namespace EventSystem {

    void init(EventState& state) {
        state.currentEvent = EventType::NONE;
        state.eventTimer = 0.0f;
        state.isActive = false;
        state.currentEventName = "";
    }

    void update(EventState& state, float timeSpeed, float timeOfDay) {
        state.eventTimer += timeSpeed;
        
        // Random Event Trigger (Simple)
        // Check every "hour" (timeOfDay changes by 1.0) or continuously
        // Let's use timer.
        
        if (!state.isActive) {
            if (state.eventTimer > 100.0f) { // Every ~100 units (approx 2 days)
                state.eventTimer = 0.0f;
                // Chance to start event
                int r = rand() % 100;
                if (r < 30) {
                    // Start Market Day (Morning only)
                    if (timeOfDay > 6.0f && timeOfDay < 10.0f) {
                        state.currentEvent = EventType::MARKET_DAY;
                        state.isActive = true;
                        state.currentEventName = "Market Day";
                        state.eventTimer = 0.0f; // Reuse timer for duration
                    } 
                    // Start Fireflies (Night only)
                    else if (timeOfDay > 20.0f || timeOfDay < 4.0f) {
                        state.currentEvent = EventType::FIREFLIES;
                        state.isActive = true;
                        state.currentEventName = "Firefly Swarm";
                        state.eventTimer = 0.0f;
                    }
                    // Start Birds (Sunrise only)
                    else if (timeOfDay > 5.0f && timeOfDay < 6.5f) {
                        state.currentEvent = EventType::BIRD_CONGREGATION;
                        state.isActive = true;
                        state.currentEventName = "Bird Gathering";
                        state.eventTimer = 0.0f;
                    }
                }
            }
        } else {
            // Event Active Logic
            if (state.currentEvent == EventType::MARKET_DAY) {
                if (timeOfDay > 18.0f) {
                    state.isActive = false;
                    state.currentEvent = EventType::NONE;
                    state.currentEventName = "";
                }
            } else if (state.currentEvent == EventType::FIREFLIES) {
                if (timeOfDay > 5.0f && timeOfDay < 20.0f) {
                    state.isActive = false;
                    state.currentEvent = EventType::NONE;
                     state.currentEventName = "";
                }
            } else if (state.currentEvent == EventType::BIRD_CONGREGATION) {
                if (timeOfDay > 8.0f) { // Disperse by 8am
                    state.isActive = false;
                    state.currentEvent = EventType::NONE;
                    state.currentEventName = "";
                }
            }
        }
    }

    void drawWorld(const EventState& state) {
        if (!state.isActive) return;
        
        if (state.currentEvent == EventType::MARKET_DAY) {
             glLineWidth(2.0f);
             glColor3f(0.8f, 0.2f, 0.2f);
             glBegin(GL_LINES);
             glVertex2f(0, 25); glVertex2f(20, 30);
             glVertex2f(20, 30); glVertex2f(50, 27);
             glEnd();
             
             glColor3f(1.0f, 0.8f, 0.0f);
             for(int i=0; i<5; i++) {
                 float x = 5 + i * 15;
                 glBegin(GL_TRIANGLES);
                 glVertex2f(x, 27); glVertex2f(x + 2, 25); glVertex2f(x + 4, 27);
                 glEnd();
             }
             glLineWidth(1.0f);
        } else if (state.currentEvent == EventType::BIRD_CONGREGATION) {
             // Birds sitting on the ground or wire (simplified)
             glColor3f(0.1f, 0.1f, 0.1f);
             for(int i=0; i<15; i++) {
                 float x = 10.0f + i * 4.0f;
                 float y = 25.0f; // On the horizon line
                 // Small bird shape
                 glBegin(GL_TRIANGLES);
                 glVertex2f(x-0.5f, y);
                 glVertex2f(x+0.5f, y);
                 glVertex2f(x, y+1.0f);
                 glEnd();
             }
        }
    }
    
    void drawScreen(const EventState& state, int width, int height) {
        if (!state.isActive) return;
        
        if (state.currentEvent == EventType::FIREFLIES) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            for(int i=0; i<30; i++) {
                float t = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
                float x = fmod(i * 133.5f + t * 5.0f, float(width));
                float y = fmod(i * 77.2f + t * 4.0f, float(height * 0.4f)) + 10.0f; // Lower half
                
                glColor4f(0.8f, 1.0f, 0.2f, 0.8f * abs(sin(t * 5.0f + i)));
                Utils::drawCircle(1.0f, x, y, 6, true);
            }
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Restore
        }
    }
}

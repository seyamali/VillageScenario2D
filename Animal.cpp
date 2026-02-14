#include "Animal.h"
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include "Style.h"

namespace AnimalSystem {
    Animal create(AnimalType type, float x, float y) {
        Animal a;
        a.type = type;
        a.x = x;
        a.y = y;
        a.targetX = x;
        a.targetY = y;
        a.speed = (type == AnimalType::BIRD) ? Utils::random(0.3f, 0.6f) : Utils::random(0.02f, 0.05f);
        a.direction = (rand() % 2 == 0) ? 1 : -1;
        a.currentState = (type == AnimalType::BIRD) ? AnimalState::FLYING : AnimalState::IDLE;
        a.stateTimer = Utils::random(2.0f, 5.0f);
        a.animFrame = 0.0f;
        a.herdId = 0;
        return a;
    }

    void update(Animal& a, float time, bool isNight, float windSway) {
        a.stateTimer -= 0.1f;
        a.animFrame += 0.1f;

        if (a.type == AnimalType::BIRD) {
            // Bird Logic
            // Always flying or occasional perched? Simplified: always fly across.
            if (a.stateTimer <= 0) {
                 // Change Y target slightly
                 a.targetY = Utils::random(30.0f, 60.0f);
                 a.stateTimer = Utils::random(2.0f, 5.0f);
            }
            // Move
            a.x += a.speed * a.direction;
            // Wind handling?
            a.x += windSway * 0.1f; // Blown by wind
            
            // Loop screen
            if (a.x > 100) a.x = -20;
            if (a.x < -20) a.x = 100;
            
            // Y movement
            float dy = a.targetY - a.y;
            a.y += dy * 0.05f + sin(time * 0.5f) * 0.1f;
            
        } else {
            // Ground Animal Logic
            if (isNight && a.type != AnimalType::BIRD) {
                if (a.currentState != AnimalState::SLEEPING) {
                    a.currentState = AnimalState::SLEEPING; // Go to sleep
                }
            } else if (a.currentState == AnimalState::SLEEPING && !isNight) {
                 a.currentState = AnimalState::IDLE; // Wake up
            }

            if (a.stateTimer <= 0 && !isNight) {
                // Pick new state
                int r = rand() % 100;
                if (r < 40) { // Grazing
                    a.currentState = AnimalState::GRAZING;
                    a.stateTimer = Utils::random(3.0f, 8.0f);
                } else if (r < 70) { // Walk
                    a.currentState = AnimalState::MOVING;
                    a.targetX = a.x + Utils::random(-15.0f, 15.0f) * float(a.direction);
                    // Herd logic: stay near herd center? 
                    // Simplified: just random nearby.
                    a.stateTimer = 10.0f; // Limit walk time
                } else {
                    a.currentState = AnimalState::IDLE;
                    a.stateTimer = Utils::random(2.0f, 5.0f);
                }
            }
            
            if (a.currentState == AnimalState::MOVING) {
                float dx = a.targetX - a.x;
                if (abs(dx) < 0.5f) {
                    a.currentState = AnimalState::IDLE;
                } else {
                    a.direction = (dx > 0) ? 1 : -1;
                    a.x += a.speed * a.direction;
                    a.animFrame += 0.2f;
                }
                // Keep boundaries
                if (a.x < -20) a.x = -20; if (a.x > 80) a.x = 80;
            }
        }
    }

    void draw(const Animal& a, const Utils::Color& ambientLight) {
        float x = a.x;
        float y = a.y;
        
        // Shadow (Soft Shadow)
        if (a.type != AnimalType::BIRD) {
            Style::drawSoftShadow(x, y, 1.5f, 0.3f);
        }
        
        // Base Atmosphere
        Utils::Color tint = ambientLight;

        if (a.type == AnimalType::COW) {
            // Body
            Utils::Color bodyC = Style::applyAtmosphere(Utils::Color(0.9f, 0.9f, 0.9f), 12, 0.0f);
            bodyC.r *= tint.r; bodyC.g *= tint.g; bodyC.b *= tint.b;
            bodyC.apply();
            Utils::drawRect(x - 2, y + 2, x + 2, y + 5, bodyC);
            
            // Head
            float headY = y + 4.5f;
            if (a.currentState == AnimalState::GRAZING) headY = y + 2.5f; // Head down
            Utils::drawRect(x + (1.5f * a.direction), headY - 1, x + (3.0f * a.direction), headY + 1, bodyC);
            
            // Legs
            glColor3f(0.1f, 0.1f, 0.1f);
            float legSway = (a.currentState == AnimalState::MOVING) ? sin(a.animFrame) * 0.5f : 0;
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            glVertex2f(x - 1.5f, y + 2); glVertex2f(x - 1.5f + legSway, y);
            glVertex2f(x + 1.5f, y + 2); glVertex2f(x + 1.5f - legSway, y);
            glEnd();
            
            if (a.currentState == AnimalState::SLEEPING) {
                // Lying down: body lower
                 Utils::drawRect(x - 2.5, y, x + 2.5, y + 3, bodyC);
            }
            
        } else if (a.type == AnimalType::SHEEP) {
            // Fluffy Body (Circles)
            Utils::Color wool = Style::applyAtmosphere(Utils::Color(0.85f, 0.85f, 0.85f), 12, 0.0f);
            wool.r *= tint.r; wool.g *= tint.g; wool.b *= tint.b;
            wool.apply();
            Utils::drawCircle(1.5f, x, y + 2.5f, 15, true);
            Utils::drawCircle(1.2f, x - 1.0f, y + 2.0f, 15, true);
            Utils::drawCircle(1.2f, x + 1.0f, y + 2.0f, 15, true);
            
             // Head
            float headY = y + 3.0f;
            if (a.currentState == AnimalState::GRAZING) headY = y + 1.5f;
            glColor3f(0.1f, 0.1f, 0.1f); // Black face
            Utils::drawCircle(0.8f, x + (1.8f * a.direction), headY, 10, true);
            
            // Legs
             glLineWidth(2.0f);
            glBegin(GL_LINES);
            float legSway = (a.currentState == AnimalState::MOVING) ? sin(a.animFrame) * 0.5f : 0;
            glVertex2f(x - 0.8f, y + 1.5f); glVertex2f(x - 0.8f + legSway, y);
            glVertex2f(x + 0.8f, y + 1.5f); glVertex2f(x + 0.8f - legSway, y);
            glEnd();
            
        } else if (a.type == AnimalType::BIRD) {
             // Simple V shape
            Utils::Color birdC = Utils::Color::lerp(Utils::Color(1.0f, 1.0f, 1.0f), ambientLight, 0.1f);
            birdC.apply();
            float wing = sin(a.animFrame * 2.0f) * 1.5f;
            glLineWidth(1.5f);
             glBegin(GL_LINE_STRIP);
            glVertex2f(x - 1.5f, y + wing);
            glVertex2f(x, y);
            glVertex2f(x + 1.5f, y + wing);
            glEnd();
        }
    }
}

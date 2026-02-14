#include "Character.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include "Style.h"

namespace CharacterSystem {

    Character create(float startX) {
        Character c;
        c.x = startX;
        c.y = 10.0f; // Ground level
        c.targetX = startX;
        c.targetY = 10.0f;
        c.speed = Utils::random(0.1f, 0.2f);
        c.height = Utils::random(3.5f, 4.2f);
        
        c.clothingColor = Utils::Color(Utils::random(0.1f, 0.9f), 
                                       Utils::random(0.1f, 0.9f), 
                                       Utils::random(0.1f, 0.9f));
                                       
        c.currentActivity = Activity::IDLE;
        c.activityTimer = Utils::random(5.0f, 15.0f); // 5-15 seconds
        
        c.animFrame = 0.0f;
        c.direction = (rand() % 2 == 0) ? 1 : -1;
        
        c.conversationPartnerId = -1;
        
        return c;
    }

    void update(Character& c, float time, const std::vector<Character>& others, int myIndex, float weatherSpeedMod) {
        c.activityTimer -= 0.1f;
        if (c.activityTimer <= 0) {
            // New decision
            int r = rand() % 100;
            if (r < 40) {
                // Walk to new random location
                c.currentActivity = Activity::WALKING;
                c.targetX = Utils::random(-20.0f, 60.0f);
                c.activityTimer = 20.0f; // Give enough time
            } else if (r < 70) {
                // Just idle
                c.currentActivity = Activity::IDLE;
                c.activityTimer = Utils::random(3.0f, 8.0f);
            } else {
                // Interact / Work (if farmer)
                c.currentActivity = Activity::WORKING; // e.g. bending down
                c.activityTimer = Utils::random(4.0f, 10.0f);
            }
        }
        
        // Social Interaction Check (Micro-Interaction)
        if (c.currentActivity == Activity::WALKING) {
            for (size_t i = 0; i < others.size(); ++i) {
                if (int(i) == myIndex) continue;
                
                const Character& other = others[i];
                float dist = std::abs(c.x - other.x);
                
                // If close and other is idle/walking, maybe stop to talk
                if (dist < 3.0f && other.currentActivity != Activity::WORKING && rand() % 100 < 2) {
                     c.currentActivity = Activity::SOCIALIZING;
                     c.activityTimer = Utils::random(5.0f, 10.0f);
                     c.conversationPartnerId = int(i);
                     // Ideally we'd set the other person too, but simplified local logic for now
                }
            }
        }

        // Logic
        if (c.currentActivity == Activity::WALKING) {
            float dx = c.targetX - c.x;
            if (std::abs(dx) < 0.5f) {
                c.currentActivity = Activity::IDLE;
                c.activityTimer = Utils::random(2.0f, 5.0f);
            } else {
                c.direction = (dx > 0) ? 1 : -1;
                c.x += c.speed * c.direction * weatherSpeedMod;
                c.animFrame += 0.2f * weatherSpeedMod;
            }
        } else if (c.currentActivity == Activity::SOCIALIZING) {
             // Face each other?
             // Just idle anim
        } else if (c.currentActivity == Activity::WORKING) {
             c.animFrame += 0.1f; // Slow work anim
        }
        
        // Keep on screen
        if (c.x < -25) c.x = -25;
        if (c.x > 85) c.x = 85;
    }
    
    void draw(const Character& c, const Utils::Color& ambientLight) {
        float x = c.x;
        float y = c.y;

        
        // Apply atmospheric lighting
        Utils::Color skinColor = Style::applyAtmosphere(Utils::Color(0.9f, 0.7f, 0.6f), 12, 0.0f);
        skinColor.r *= ambientLight.r; skinColor.g *= ambientLight.g; skinColor.b *= ambientLight.b;
        
        Utils::Color clothes = Style::applyAtmosphere(c.clothingColor, 12, 0.0f);
        clothes.r *= ambientLight.r; clothes.g *= ambientLight.g; clothes.b *= ambientLight.b;
        
        // Shadow/Ground contact (Soft Shadow)
        Style::drawSoftShadow(x, y, 1.2f, 0.3f);

        // Body Animation
        float legAngle = 0;
        float armAngle = 0;
        float bodyY = y + 2.0f;
        
        if (c.currentActivity == Activity::WALKING) {
            legAngle = sin(c.animFrame) * 0.5f;
            armAngle = -sin(c.animFrame) * 0.5f;
            bodyY += abs(sin(c.animFrame * 2)) * 0.2f; // Bob
        } else if (c.currentActivity == Activity::WORKING) {
            // Bent over working
             armAngle = sin(c.animFrame) * 0.8f;
        }

        // Legs
        Utils::Color pantsColor(0.2f, 0.2f, 0.2f); // Dark pants
        pantsColor = Utils::Color::lerp(pantsColor, ambientLight, 0.4f);
        pantsColor.apply();
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        // Left Leg
        glVertex2f(x, bodyY);
        glVertex2f(x + sin(legAngle) * 1.5f, y);
        // Right Leg
        glVertex2f(x, bodyY);
        glVertex2f(x - sin(legAngle) * 1.5f, y);
        glEnd();
        
        // Torso
        clothes.apply();
        Utils::drawRect(x - 0.7f, bodyY, x + 0.7f, bodyY + 2.5f, clothes);
        
        // Arms
        skinColor.apply();
        glBegin(GL_LINES);
        glVertex2f(x, bodyY + 2.0f);
        glVertex2f(x + sin(armAngle) * 2.0f, bodyY + 1.0f); // Simple arm swing
        glEnd();
        
        // Head
        Utils::drawCircle(1.0f, x, bodyY + 3.0f, 20, true);
        
        // Talk bubble? (Micro-Interaction)
        if (c.currentActivity == Activity::SOCIALIZING) {
             glEnable(GL_BLEND);
             glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
             Utils::drawCircle(0.8f, x + 1.5f, bodyY + 4.5f, 10, true);
             glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
             Utils::drawCircle(0.2f, x + 1.5f, bodyY + 4.5f, 5, true); // "..."
             glDisable(GL_BLEND);
        }

        glLineWidth(1.0f);
    }
}

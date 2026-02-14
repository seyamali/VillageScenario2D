#include "Style.h"
#include <GL/glut.h>
#include <cmath>

namespace Style {
    Utils::Color getSeasonalColor(Utils::Season season, 
                                  Utils::Color spring, 
                                  Utils::Color summer, 
                                  Utils::Color autumn, 
                                  Utils::Color winter) {
        switch (season) {
            case Utils::Season::SPRING: return spring;
            case Utils::Season::SUMMER: return summer;
            case Utils::Season::AUTUMN: return autumn;
            case Utils::Season::WINTER: return winter;
        }
        return spring;
    }
    
    Utils::Color applyAtmosphere(Utils::Color base, float time, float weather) {
        Utils::Color ambient(1.0f, 1.0f, 1.0f);
        
        // Simple Color Grading based on time
        if (time < 5.0f || time > 20.0f) {
            ambient = Utils::Color(0.2f, 0.2f, 0.5f); // Night Blue
        } else if (time >= 5.0f && time < 7.0f) {
            float t = (time - 5.0f) / 2.0f;
            ambient = Utils::Color::lerp(Utils::Color(0.2f, 0.2f, 0.4f), Utils::Color(1.0f, 0.8f, 0.6f), t); // Sunrise warm
        } else if (time >= 7.0f && time < 17.0f) {
            ambient = Utils::Color(1.0f, 1.0f, 0.95f); // Bright Day, slight warm
        } else if (time >= 17.0f && time <= 20.0f) {
            float t = (time - 17.0f) / 3.0f;
            ambient = Utils::Color::lerp(Utils::Color(1.0f, 0.9f, 0.8f), Utils::Color(0.8f, 0.5f, 0.6f), t); // Sunset purple/orange
        }
        
        // Saturation reduction based on weather (Rain/Storm)
        if (weather > 0.0f) {
            // Desaturate slightly
            float gray = (base.r + base.g + base.b) / 3.0f;
            base.r = Utils::lerp(base.r, gray, weather * 0.5f);
            base.g = Utils::lerp(base.g, gray, weather * 0.5f);
            base.b = Utils::lerp(base.b, gray, weather * 0.5f);
            
            // Darken slightly
             ambient.r *= (1.0f - weather * 0.3f);
             ambient.g *= (1.0f - weather * 0.3f);
             ambient.b *= (1.0f - weather * 0.3f);
        }
        
        // Combine (Multiply)
        return Utils::Color(base.r * ambient.r, base.g * ambient.g, base.b * ambient.b, base.a);
    }
    
    void drawSoftShadow(float x, float y, float w, float scaleY) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Soft Ellipse
        // Outer faded ring
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(0.0f, 0.0f, 0.0f, 0.4f); // Center dark
        glVertex2f(x, y);
        
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f); // Edge transparent
        int segments = 16;
        for(int i = 0; i <= segments; i++) {
            float angle = 2.0f * 3.14159f * float(i) / float(segments);
            float dx = cos(angle) * w;
            float dy = sin(angle) * w * scaleY;
            glVertex2f(x + dx, y + dy);
        }
        glEnd();
        
        glDisable(GL_BLEND);
    }
}

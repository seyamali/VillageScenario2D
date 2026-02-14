#ifndef UTILS_H
#define UTILS_H

#include <GL/glut.h>
#include <cmath>
#include <vector>

namespace Utils {
    
    struct Color {
        float r, g, b, a;
        Color(float r = 0, float g = 0, float b = 0, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
        
        // Linear interpolation
        static Color lerp(const Color& start, const Color& end, float t) {
            float inv = 1.0f - t;
            return Color(
                start.r * inv + end.r * t,
                start.g * inv + end.g * t,
                start.b * inv + end.b * t,
                start.a * inv + end.a * t
            );
        }

        void apply() const { glColor4f(r, g, b, a); }
    };

    enum class Season {
        SPRING,
        SUMMER,
        AUTUMN,
        WINTER
    };

    // Math & Random
    float random(float min, float max);
    float lerp(float a, float b, float t);
    
    // Drawing Primitives
    void drawCircle(float r, float x, float y, int segments = 50, bool filled = true);
    void drawRect(float x1, float y1, float x2, float y2, const Color& c);
    void drawGradientRect(float x1, float y1, float x2, float y2, const Color& c1, const Color& c2, bool vertical = true);

}

#endif // UTILS_H

#include "Utils.h"
#include <cstdlib>
#include <ctime>

namespace Utils {

    // Math & Random
    float random(float min, float max) {
        static bool seeded = false;
        if (!seeded) {
            srand(time(NULL));
            seeded = true;
        }
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    }

    float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    // Drawing Primitives
    void drawCircle(float r, float x, float y, int segments, bool filled) {
        if (filled) glBegin(GL_POLYGON);
        else glBegin(GL_LINE_LOOP);

        for (int i = 0; i < segments; i++) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float cx = r * cosf(theta);
            float cy = r * sinf(theta);
            glVertex2f(x + cx, y + cy);
        }
        glEnd();
    }

    void drawRect(float x1, float y1, float x2, float y2, const Color& c) {
        c.apply();
        glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
        glEnd();
    }

    void drawGradientRect(float x1, float y1, float x2, float y2, const Color& c1, const Color& c2, bool vertical) {
        glBegin(GL_QUADS);
        if (vertical) {
            c1.apply(); // Bottom
            glVertex2f(x1, y1);
            glVertex2f(x2, y1);

            c2.apply(); // Top
            glVertex2f(x2, y2);
            glVertex2f(x1, y2);
        } else {
            c1.apply(); // Left
            glVertex2f(x1, y1);
            
            c2.apply(); // Right
            glVertex2f(x2, y1);
            glVertex2f(x2, y2);
            
            c1.apply(); // Left corner
            glVertex2f(x1, y2);
        }
        glEnd();
    }
}

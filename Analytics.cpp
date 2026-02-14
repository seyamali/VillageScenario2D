#include "Analytics.h"
#include <GL/glut.h>
#include <cstdio>
#include <string>
#include <vector>
#include "Utils.h"

namespace Analytics {
    
    void init(Metrics& m) {
        m.fps = 60.0f;
        m.frameTime = 16.6f;
        m.entityCount = 0;
        m.particleCount = 0;
        m.memoryUsage = 0.0f;
        m.active = false;
        m.showHeatmap = false;
    }
    
    void update(Metrics& m, int entCount, int partCount) {
        // Compute FPS (Simple approximation based on timer)
        static int lastTime = 0;
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        float dt = (currentTime - lastTime) * 0.001f;
        lastTime = currentTime;
        
        if (dt > 0) m.fps = 0.9f * m.fps + 0.1f * (1.0f / dt);
        m.frameTime = dt * 1000.0f;
        
        m.entityCount = entCount;
        m.particleCount = partCount;
        m.memoryUsage = entCount * 0.05f + partCount * 0.001f; // Simulated MB
    }
    
    void draw(const Metrics& m, int width, int height) {
        if (!m.active) return;
        
        // --- HUD OVERLAY ---
        // Matrices already set by Scene::display for all Screen Overlays
        
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        
        // Semi-transparent bg
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(10, height - 10);
        glVertex2f(220, height - 10);
        glVertex2f(220, height - 120);
        glVertex2f(10, height - 120);
        glEnd();
        
        glColor3f(0.0f, 1.0f, 0.2f);
        char buffer[128];
        void* font = GLUT_BITMAP_HELVETICA_12;
        
        sprintf(buffer, "SIMULATION ANALYTICS [ON]");
        glRasterPos2f(15, height - 25);
        for(char* c = buffer; *c != '\0'; c++) glutBitmapCharacter(font, *c);
        
        glColor3f(1.0f, 1.0f, 1.0f);
        sprintf(buffer, "FPS: %.1f", m.fps);
        glRasterPos2f(15, height - 40);
        for(char* c = buffer; *c != '\0'; c++) glutBitmapCharacter(font, *c);

        sprintf(buffer, "Entities: %d", m.entityCount);
        glRasterPos2f(15, height - 55);
        for(char* c = buffer; *c != '\0'; c++) glutBitmapCharacter(font, *c);

        sprintf(buffer, "Particles: %d", m.particleCount);
        glRasterPos2f(15, height - 70);
        for(char* c = buffer; *c != '\0'; c++) glutBitmapCharacter(font, *c);

        sprintf(buffer, "Sim Mem: %.2f MB", m.memoryUsage);
        glRasterPos2f(15, height - 85);
        for(char* c = buffer; *c != '\0'; c++) glutBitmapCharacter(font, *c);
        
        glColor3f(0.5f, 0.5f, 0.5f);
        sprintf(buffer, "F1: Toggle Overlay | F2: Heatmap");
        glRasterPos2f(15, height - 105);
        for(char* c = buffer; *c != '\0'; c++) glutBitmapCharacter(font, *c);

        glDisable(GL_BLEND);
    }
    
    // Draw population heatmap (to be called in World Space context but from Scene)
    void drawHeatmap(const std::vector<float>& entityX, const std::vector<float>& entityY) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive Glow
        
        for (float x : entityX) {
            // Heatmap Blob
            for (float r = 5.0f; r > 0; r -= 1.0f) {
                glColor4f(1.0f, 0.0f, 0.0f, 0.1f); 
                Utils::drawCircle(r, x, 5.0f /* Ground level approx */, 12, true);
            }
        }
        
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void toggle(Metrics& m) {
        m.active = !m.active;
    }
}

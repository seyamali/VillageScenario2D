#include "Camera.h"
#include <GL/glut.h>
#include <cmath>

namespace CameraSystem {

    float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
 
    void init(CameraState& cam) {
        cam.x = 0; cam.y = 0;
        cam.zoom = 1.0f;
        cam.targetX = 0; cam.targetY = 0;
        cam.targetZoom = 1.0f;
        cam.smoothSpeed = 0.05f;
        cam.isCinematic = true;
    }
    
    void update(CameraState& cam) {
        cam.x = lerp(cam.x, cam.targetX, cam.smoothSpeed);
        cam.y = lerp(cam.y, cam.targetY, cam.smoothSpeed);
        cam.zoom = lerp(cam.zoom, cam.targetZoom, cam.smoothSpeed);
    }
    
    void apply(const CameraState& cam) {
        // Translation -> Scale -> Translation (Center on target)
        // With Ortho (0,0 bottom left usually, but we have -20 to 100 on X?)
        // Let's assume (40, 30) is "center" of our scene (80 width / 2).
        
        float centerX = 40.0f; // Approx
        float centerY = 30.0f;
        
        glTranslatef(centerX, centerY, 0); // Move to pivot
        glScalef(cam.zoom, cam.zoom, 1.0f); // Scale
        glTranslatef(-centerX, -centerY, 0); // Move back pivot
        
        glTranslatef(-cam.x, -cam.y, 0); // Pan
    }
    
    void panTo(CameraState& cam, float x, float y, float zoom) {
         cam.targetX = x;
         cam.targetY = y;
         cam.targetZoom = zoom;
    }
    
    void jumpTo(CameraState& cam, float x, float y, float zoom) {
         cam.x = x; cam.y = y; cam.zoom = zoom;
         panTo(cam, x, y, zoom);
    }
    
    void updateCinematic(CameraState& cam, float timeOfDay) {
        if (!cam.isCinematic) return;
        
        // Automated Logic based on time
        // Morning: Wide shot
        if (timeOfDay >= 5.0f && timeOfDay < 8.0f) {
             panTo(cam, 0.0f, 0.0f, 1.0f);
        } 
        // Noon: Slight zoom on village center
        else if (timeOfDay >= 11.0f && timeOfDay < 13.0f) {
             panTo(cam, 0.0f, 0.0f, 1.1f);
        }
        // Evening: Focus on river/boat?
        else if (timeOfDay >= 17.0f && timeOfDay < 19.0f) {
             panTo(cam, 10.0f, -5.0f, 1.1f);
        }
        // Night: Wide again or slow drift
        else if (timeOfDay > 22.0f || timeOfDay < 4.0f) {
             // Drifting pan
             float drift = sin(timeOfDay * 0.5f) * 10.0f;
             panTo(cam, drift, 0.0f, 1.0f);
        }
        else {
             // Default transition
             panTo(cam, 0.0f, 0.0f, 1.0f);
        }
    }
}

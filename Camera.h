#ifndef CAMERA_H
#define CAMERA_H

namespace CameraSystem {

    struct CameraState {
        float x, y;
        float zoom;
        
        // Target for interpolation
        float targetX, targetY;
        float targetZoom;
        
        // Settings
        float smoothSpeed; // 0.0 to 1.0
        
        bool isCinematic; // Automated mode
    };

    void init(CameraState& cam);
    void update(CameraState& cam);
    void apply(const CameraState& cam);
    
    // Actions
    void panTo(CameraState& cam, float x, float y, float zoom = 1.0f);
    void jumpTo(CameraState& cam, float x, float y, float zoom = 1.0f);
    
    // Automated Logic hook
    void updateCinematic(CameraState& cam, float timeOfDay);
}

#endif // CAMERA_H

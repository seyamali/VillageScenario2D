#ifndef CHARACTER_H
#define CHARACTER_H

#include <vector>
#include "Utils.h"

enum class Activity {
    IDLE,
    WALKING,
    WORKING, // e.g. farming
    SOCIALIZING // talking
};

struct Character {
    float x, y;
    float targetX, targetY;
    float speed;
    Utils::Color clothingColor;
    float height;
    
    // State
    Activity currentActivity;
    float activityTimer; // How long to stay in current state
    
    // Animation
    float animFrame; // Walk cycle frame
    int direction; // 1 = right, -1 = left
    
    // Social
    int conversationPartnerId; // -1 if none
};

namespace CharacterSystem {
    Character create(float startX);
    
    void update(Character& c, float time, const std::vector<Character>& others, int myIndex, float weatherSpeedMod);
    
    void draw(const Character& c, const Utils::Color& ambientLight);
}

#endif // CHARACTER_H

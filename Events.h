#ifndef EVENTS_H
#define EVENTS_H

#include <string>
#include <vector>

enum class EventType {
    NONE,
    MARKET_DAY,
    FESTIVAL,
    FIREFLIES,
    BIRD_CONGREGATION
};

struct EventState {
    EventType currentEvent;
    float eventTimer;
    bool isActive;
    std::string currentEventName;
};

namespace EventSystem {
    void init(EventState& state);
    void update(EventState& state, float timeSpeed, float timeOfDay);
    
    // World Space Elements (Decorations)
    void drawWorld(const EventState& state);
    
    // Screen Space Elements (Fireflies)
    void drawScreen(const EventState& state, int width, int height);
}

#endif // EVENTS_H

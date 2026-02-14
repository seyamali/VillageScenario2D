#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <string>
#include <vector>

namespace Analytics {
    struct Metrics {
        float fps;
        float frameTime;
        int entityCount;
        int particleCount;
        float memoryUsage; // Estimated or tracked simulated
        
        bool active;
        bool showHeatmap;
    };
    
    void init(Metrics& m);
    void update(Metrics& m, int entCount, int partCount);
    void draw(const Metrics& m, int width, int height);
    void drawHeatmap(const std::vector<float>& entityX, const std::vector<float>& entityY);
    void toggle(Metrics& m);
}

#endif // ANALYTICS_H

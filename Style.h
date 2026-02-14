#ifndef STYLE_H
#define STYLE_H

#include "Utils.h"

namespace Style {
    namespace Palette {
        // --- 1. Nature Colors ---
        // Grass/Ground
        const Utils::Color GRASS_SPRING(0.35f, 0.75f, 0.35f);
        const Utils::Color GRASS_SUMMER(0.25f, 0.65f, 0.25f);
        const Utils::Color GRASS_AUTUMN(0.75f, 0.55f, 0.25f);
        const Utils::Color GRASS_WINTER(0.90f, 0.90f, 0.95f);
        
        // Foliage (Trees)
        const Utils::Color LEAF_SPRING(0.40f, 0.85f, 0.40f);
        const Utils::Color LEAF_SUMMER(0.15f, 0.55f, 0.15f);
        const Utils::Color LEAF_AUTUMN(0.85f, 0.45f, 0.10f); // Vibrant Orange
        const Utils::Color LEAF_WINTER(0.80f, 0.85f, 0.90f); // Snow covered
        
        // Water
        const Utils::Color WATER_DEEP(0.1f, 0.3f, 0.8f);
        const Utils::Color WATER_SHALLOW(0.2f, 0.5f, 0.9f);
        const Utils::Color WATER_RIVER(0.2f, 0.4f, 0.8f, 0.7f);

        // --- 2. Architecture Colors ---
        const Utils::Color WALL_WOOD_LIGHT(0.75f, 0.60f, 0.40f);
        const Utils::Color WALL_WOOD_DARK(0.55f, 0.40f, 0.25f);
        const Utils::Color ROOF_THATCH(0.80f, 0.70f, 0.40f);
        const Utils::Color ROOF_RED(0.70f, 0.30f, 0.30f);
        const Utils::Color STONE_GRAY(0.60f, 0.60f, 0.65f);

        // --- 3. Atmosphere (Sky Gradients) ---
        // Night
        const Utils::Color SKY_NIGHT_TOP(0.00f, 0.00f, 0.10f);
        const Utils::Color SKY_NIGHT_BOT(0.05f, 0.05f, 0.20f);
        
        // Dawn
        const Utils::Color SKY_DAWN_TOP(0.40f, 0.20f, 0.50f);
        const Utils::Color SKY_DAWN_BOT(0.80f, 0.40f, 0.40f);
        
        // Clay Day (Generic Blue)
        const Utils::Color SKY_DAY_TOP(0.00f, 0.40f, 0.90f);
        const Utils::Color SKY_DAY_BOT(0.50f, 0.70f, 1.00f);
        
        // Dusk
        const Utils::Color SKY_DUSK_TOP(0.30f, 0.10f, 0.40f);
        const Utils::Color SKY_DUSK_BOT(0.90f, 0.30f, 0.20f);
    }
    
    // --- Art Direction Framework Functions ---

    // Get color based on season
    Utils::Color getSeasonalColor(Utils::Season season, 
                                  Utils::Color spring, 
                                  Utils::Color summer, 
                                  Utils::Color autumn, 
                                  Utils::Color winter);

    // Apply unified lighting and saturation adjustments based on time
    // This ensures all elements "fit" the current lighting mood
    Utils::Color applyAtmosphere(Utils::Color base, float timeOfDay, float weatherIntensity = 0.0f);
    
    // Soft Shadow Helper
    void drawSoftShadow(float x, float y, float width, float scaleY);
}

#endif // STYLE_H

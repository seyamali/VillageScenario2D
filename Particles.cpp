#include "Particles.h"
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>

#include <vector>

namespace ParticleSystem {
    
    std::vector<Particle> particles; // Use the header definition
    int maxP = 0;

    void init(int maxParticles) {
        maxP = maxParticles;
        particles.resize(maxP);
        for(auto& p : particles) p.active = false;
    }

    void spawnPollen(int count, int width, int height) {
        for(int i=0; i<count; i++) {
            for(auto& p : particles) {
                if(!p.active) {
                    p.active = true;
                    p.type = ParticleType::POLLEN;
                    p.x = Utils::random(-20.0f, width + 20.0f);
                    p.y = Utils::random(0.0f, height + 20.0f);
                    p.speedX = Utils::random(-0.1f, 0.1f);
                    p.speedY = Utils::random(-0.05f, 0.05f); // Drifting
                    p.size = Utils::random(0.2f, 0.5f);
                    p.color = Utils::Color(1.0f, 1.0f, 0.8f, 0.8f);
                    p.life = 100.0f; // Long life
                    p.maxLife = 100.0f;
                    break;
                }
            }
        }
    }

    void spawnLeaves(int count, int width, int height, float wind) {
        for(int i=0; i<count; i++) {
             for(auto& p : particles) {
                if(!p.active) {
                    p.active = true;
                    p.type = ParticleType::LEAF;
                    p.x = Utils::random(-20.0f, width + 20.0f);
                    p.y = height + Utils::random(0.0f, 20.0f);
                    p.speedX = wind * 0.5f + Utils::random(-0.2f, 0.2f);
                    p.speedY = Utils::random(-0.5f, -1.0f); // Falling
                    p.size = Utils::random(0.5f, 1.0f);
                    float r = Utils::random(0.7f, 1.0f);
                    p.color = Utils::Color(r, r * 0.5f, 0.1f, 1.0f); // Orange/Yellow
                    p.rotation = Utils::random(0.0f, 360.0f);
                    p.rotSpeed = Utils::random(-5.0f, 5.0f);
                    p.life = 100.0f; 
                    p.maxLife = 100.0f;
                    break;
                }
            }
        }
    }
    
    void spawnSnowMicro(int count, int width, int height) {
        // Large flakes closer to camera
         for(int i=0; i<count; i++) {
             for(auto& p : particles) {
                if(!p.active) {
                    p.active = true;
                    p.type = ParticleType::SNOW_MICRO;
                    p.x = Utils::random(-20.0f, width + 20.0f);
                    p.y = height + Utils::random(0.0f, 20.0f);
                    p.speedX = Utils::random(-0.2f, 0.2f);
                    p.speedY = Utils::random(-0.5f, -1.5f);
                    p.size = Utils::random(0.8f, 1.5f); // Bigger
                    p.color = Utils::Color(1.0f, 1.0f, 1.0f, 0.9f);
                    p.life = 100.0f; 
                    break;
                }
            }
         }
    }
    
    void spawnDust(int count, int width, int height) {
        for(int i=0; i<count; i++) {
             for(auto& p : particles) {
                if(!p.active) {
                    p.active = true;
                    p.type = ParticleType::DUST;
                    p.x = Utils::random(-20.0f, width + 20.0f);
                    p.y = Utils::random(0.0f, height + 20.0f);
                    p.speedX = Utils::random(-0.05f, 0.05f); // Very slow drift
                    p.speedY = Utils::random(-0.05f, 0.05f);
                    p.size = Utils::random(0.1f, 0.3f);
                    p.color = Utils::Color(0.9f, 0.9f, 0.8f, 0.5f);
                    p.life = 200.0f; 
                    break;
                }
            }
        }
    }

    void spawnSmoke(float x, float y) {
        for(auto& p : particles) {
            if(!p.active) {
                p.active = true;
                p.type = ParticleType::CHIMNEY_SMOKE;
                p.x = x + Utils::random(-0.5f, 0.5f);
                p.y = y;
                p.speedX = Utils::random(-0.05f, 0.05f);
                p.speedY = Utils::random(0.1f, 0.2f);
                p.size = Utils::random(1.0f, 2.0f);
                p.color = Utils::Color(0.8f, 0.8f, 0.8f, 0.4f);
                p.life = Utils::random(50.0f, 100.0f);
                p.maxLife = p.life;
                break;
            }
        }
    }

    void update(float timeSpeed, Utils::Season season, float windStrength, int width, int height) {
        
        // Seasonal Spawning Logic (Simple chance based)
        if (season == Utils::Season::SPRING) {
            if (rand() % 100 < 5) spawnPollen(1, width, height); // Consistent flow
        } else if (season == Utils::Season::AUTUMN) {
            if (rand() % 100 < 3) spawnLeaves(1, width, height, windStrength);
        } else if (season == Utils::Season::WINTER) {
             if (rand() % 100 < 5) spawnSnowMicro(1, width, height);
        } else { // Summer
             if (rand() % 100 < 2) spawnDust(1, width, height);
        }
        
        for(auto& p : particles) {
            if (!p.active) continue;
            
            p.x += p.speedX;
            p.y += p.speedY; // Gravity/Drift
            
            if (p.type == ParticleType::LEAF) {
                p.rotation += p.rotSpeed;
                p.x += windStrength * 0.05f; // Extra wind push
                p.speedY = -0.5f + sin(p.x * 0.1f) * 0.2f; // Flutter
            } else if (p.type == ParticleType::POLLEN) {
                 p.x += windStrength * 0.02f;
                 p.y += sin(p.x * 0.5f) * 0.02f; // Wavy
            } else if (p.type == ParticleType::SNOW_MICRO) {
                 p.x += windStrength * 0.05f;
            } else if (p.type == ParticleType::CHIMNEY_SMOKE) {
                 p.x += windStrength * 0.1f + sin(p.y * 0.2f) * 0.1f;
                 p.size += 0.01f;
            }

            // Life cycle
            p.life -= 0.5f; 
            float alphaMod = p.life / p.maxLife;
            if (p.type == ParticleType::CHIMNEY_SMOKE) p.color.a = 0.4f * alphaMod;

            if (p.life <= 0 || p.y < -10 || p.y > height + 50 || p.x < -50 || p.x > width + 50) {
                p.active = false;
            }
        }
    }

    void draw(float timeOfDay) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for(const auto& p : particles) {
            if (!p.active) continue;
            
            p.color.apply();
            
            if (p.type == ParticleType::LEAF) {
                glPushMatrix();
                glTranslatef(p.x, p.y, 0);
                glRotatef(p.rotation, 0, 0, 1);
                // Simple leaf shape
                glBegin(GL_TRIANGLES);
                glVertex2f(-p.size, 0);
                glVertex2f(p.size, 0);
                glVertex2f(0, p.size * 2.0f);
                glEnd();
                glPopMatrix();
            } else if (p.type == ParticleType::CHIMNEY_SMOKE) {
                // Smoky blobs
                glEnable(GL_BLEND);
                p.color.apply();
                Utils::drawCircle(p.size, p.x, p.y, 10, true);
            } else {
                // Circle/Dot
                glPointSize(p.size * 2.0f); // Simple point
                glBegin(GL_POINTS);
                glVertex2f(p.x, p.y);
                glEnd();
            }
        }
        
        glDisable(GL_BLEND);
    }
}

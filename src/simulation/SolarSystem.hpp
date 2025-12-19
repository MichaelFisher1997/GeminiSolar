#pragma once

#include <vector>
#include <memory>
#include <string>
#include "CelestialBody.hpp"

namespace Simulation {

class SolarSystem {
public:
    SolarSystem();

    void init(); // Loads default Solar System
    void loadSystem(const std::string& systemName);

    const std::vector<std::unique_ptr<CelestialBody>>& getBodies() const { return m_bodies; }
    const CelestialBody* getSun() const; // Returns the central star
    
    const std::string& getCurrentSystemName() const { return m_currentSystemName; }
    float getSystemScale() const { return m_systemScale; } 
    float getPlanetScale() const { return m_planetScale; }

    // Physics
    void setPhysicsEnabled(bool enabled);
    bool isPhysicsEnabled() const { return m_physicsEnabled; }
    void updatePhysics(double dt);
    void resetPhysics();

private:
    void loadFallbackSolarSystem();
    void computeGravity();
    
    std::vector<std::unique_ptr<CelestialBody>> m_bodies;
    std::string m_currentSystemName;
    float m_systemScale = 10.0f; 
    float m_planetScale = 1.0f; 
    
    bool m_physicsEnabled = false;
    double m_gravityConstant = 0.0001; // Tuned for the visual scale
};

} // namespace Simulation

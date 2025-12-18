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

private:
    void loadFallbackSolarSystem();
    
    std::vector<std::unique_ptr<CelestialBody>> m_bodies;
    std::string m_currentSystemName;
    float m_systemScale = 10.0f; 
    float m_planetScale = 1.0f; // Visual multiplier for planet radius
};

} // namespace Simulation

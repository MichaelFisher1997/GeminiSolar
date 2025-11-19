#include "SolarSystem.hpp"

namespace Simulation {

SolarSystem::SolarSystem() {
}

void SolarSystem::init() {
    // Data roughly based on real solar system, scaled
    // Distances in AU approx, Periods in Years approx
    // Radii in Earth radii approx
    
    // Sun
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Sun", 
        109.0, // Scaled radius 
        glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
        OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0} // Stationary
    ));

    // Mercury
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Mercury", 
        0.38, 
        glm::vec3(0.7f, 0.7f, 0.7f), // Grey
        OrbitalParams{0.39, 0.205, glm::radians(7.0), 0.24, 0.0}
    ));

    // Venus
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Venus", 
        0.95, 
        glm::vec3(0.9f, 0.8f, 0.5f), // Yellow-white
        OrbitalParams{0.72, 0.007, glm::radians(3.4), 0.615, 0.0}
    ));

    // Earth
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Earth", 
        1.0, 
        glm::vec3(0.2f, 0.4f, 0.9f), // Blue
        OrbitalParams{1.0, 0.017, glm::radians(0.0), 1.0, 0.0}
    ));

    // Mars
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Mars", 
        0.53, 
        glm::vec3(0.8f, 0.4f, 0.3f), // Red
        OrbitalParams{1.52, 0.094, glm::radians(1.85), 1.88, 0.0}
    ));

    // Jupiter
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Jupiter", 
        11.2, 
        glm::vec3(0.8f, 0.7f, 0.6f), // Orange-ish
        OrbitalParams{5.20, 0.049, glm::radians(1.3), 11.86, 0.0}
    ));

    // Saturn
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Saturn", 
        9.45, 
        glm::vec3(0.9f, 0.8f, 0.6f), // Pale gold
        OrbitalParams{9.58, 0.057, glm::radians(2.48), 29.45, 0.0}
    ));

    // Uranus
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Uranus", 
        4.0, 
        glm::vec3(0.6f, 0.8f, 0.9f), // Pale blue
        OrbitalParams{19.22, 0.046, glm::radians(0.77), 84.0, 0.0}
    ));

    // Neptune
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Neptune", 
        3.88, 
        glm::vec3(0.3f, 0.3f, 0.8f), // Deep blue
        OrbitalParams{30.05, 0.011, glm::radians(1.77), 164.8, 0.0}
    ));
}

const CelestialBody* SolarSystem::getSun() const {
    if (m_bodies.empty()) return nullptr;
    return m_bodies[0].get();
}

} // namespace Simulation

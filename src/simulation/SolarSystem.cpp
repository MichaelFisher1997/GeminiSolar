#include "SolarSystem.hpp"

namespace Simulation {

SolarSystem::SolarSystem() {
}

void SolarSystem::init() {
    // Accurate J2000 Orbital Elements
    // Distances in AU
    // Periods in Earth Years
    // Angles in Radians
    // Radius in AU (converted from km, approx)
    // Sun Radius: 0.00465 AU
    // Earth Radius: 0.0000426 AU
    
    // Scaling planets for visibility handled in Renderer, here we store physical radius in AU if possible,
    // OR we stick to "Earth Radii" as the unit for radius and let renderer scale it.
    // Previous code used Earth Radii. Let's stick to Earth Radii for "radius" field to avoid tiny numbers,
    // and renderer handles conversion.
    // 1 Earth Radius = 6371 km.
    // 1 AU = 149.6e6 km.
    
    // Sun
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Sun", 
        109.0, // 109 Earth Radii
        glm::vec3(1.0f, 1.0f, 0.0f), 
        OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0}
    ));

    // Mercury
    // a=0.387, e=0.2056, i=7.00, O=48.33, w=29.12, M=174.79
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Mercury", 
        0.38, 
        glm::vec3(0.7f, 0.7f, 0.7f), 
        OrbitalParams{0.387, 0.2056, glm::radians(7.00), 0.2408, glm::radians(174.79), glm::radians(48.33), glm::radians(29.12)}
    ));

    // Venus
    // a=0.723, e=0.0068, i=3.39, O=76.68, w=54.85, M=50.11
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Venus", 
        0.95, 
        glm::vec3(0.9f, 0.8f, 0.5f), 
        OrbitalParams{0.723, 0.0068, glm::radians(3.39), 0.6152, glm::radians(50.11), glm::radians(76.68), glm::radians(54.85)}
    ));

    // Earth
    // a=1.000, e=0.0167, i=0.00, O=-11.26, w=102.94, M=357.5
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Earth", 
        1.0, 
        glm::vec3(0.2f, 0.4f, 0.9f), 
        OrbitalParams{1.000, 0.0167, glm::radians(0.00), 1.0000, glm::radians(357.52), glm::radians(-11.26), glm::radians(102.94)}
    ));

    // Mars
    // a=1.524, e=0.0934, i=1.85, O=49.58, w=286.5, M=19.41
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Mars", 
        0.53, 
        glm::vec3(0.8f, 0.4f, 0.3f), 
        OrbitalParams{1.524, 0.0934, glm::radians(1.85), 1.8808, glm::radians(19.41), glm::radians(49.58), glm::radians(286.50)}
    ));

    // Jupiter
    // a=5.203, e=0.0484, i=1.30, O=100.55, w=273.87, M=20.02
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Jupiter", 
        11.2, 
        glm::vec3(0.8f, 0.7f, 0.6f), 
        OrbitalParams{5.203, 0.0484, glm::radians(1.30), 11.862, glm::radians(20.02), glm::radians(100.55), glm::radians(273.87)}
    ));

    // Saturn
    // a=9.537, e=0.0541, i=2.49, O=113.72, w=339.39, M=317.02
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Saturn", 
        9.45, 
        glm::vec3(0.9f, 0.8f, 0.6f), 
        OrbitalParams{9.537, 0.0541, glm::radians(2.49), 29.457, glm::radians(317.02), glm::radians(113.72), glm::radians(339.39)}
    ));

    // Uranus
    // a=19.191, e=0.0472, i=0.77, O=74.00, w=96.66, M=142.59
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Uranus", 
        4.0, 
        glm::vec3(0.6f, 0.8f, 0.9f), 
        OrbitalParams{19.191, 0.0472, glm::radians(0.77), 84.011, glm::radians(142.59), glm::radians(74.00), glm::radians(96.66)}
    ));

    // Neptune
    // a=30.069, e=0.0086, i=1.77, O=131.78, w=272.85, M=260.25
    m_bodies.push_back(std::make_unique<CelestialBody>(
        "Neptune", 
        3.88, 
        glm::vec3(0.3f, 0.3f, 0.8f), 
        OrbitalParams{30.069, 0.0086, glm::radians(1.77), 164.79, glm::radians(260.25), glm::radians(131.78), glm::radians(272.85)}
    ));
}

const CelestialBody* SolarSystem::getSun() const {
    if (m_bodies.empty()) return nullptr;
    return m_bodies[0].get();
}

} // namespace Simulation

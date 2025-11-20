#include "SolarSystem.hpp"
#include <iostream>

namespace Simulation {

SolarSystem::SolarSystem() {
}

void SolarSystem::init() {
    loadSystem("Solar System");
}

void SolarSystem::loadSystem(const std::string& systemName) {
    m_bodies.clear();
    m_currentSystemName = systemName;
    
    if (systemName == "Solar System") {
        m_systemScale = 10.0f;
        m_planetScale = 0.5f; // Standard visual size
        // Accurate J2000 Orbital Elements for Solar System (Copied from previous implementation)
        // Sun
        m_bodies.push_back(std::make_unique<CelestialBody>("Sun", 109.0, glm::vec3(1.0f, 1.0f, 0.0f), OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0}));
        // Mercury
        m_bodies.push_back(std::make_unique<CelestialBody>("Mercury", 0.38, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.387, 0.2056, glm::radians(7.00), 0.2408, glm::radians(174.79), glm::radians(48.33), glm::radians(29.12)}));
        // Venus
        m_bodies.push_back(std::make_unique<CelestialBody>("Venus", 0.95, glm::vec3(0.9f, 0.8f, 0.5f), OrbitalParams{0.723, 0.0068, glm::radians(3.39), 0.6152, glm::radians(50.11), glm::radians(76.68), glm::radians(54.85)}));
        
        // Earth & Moon
        auto earth = std::make_unique<CelestialBody>("Earth", 1.0, glm::vec3(0.2f, 0.4f, 0.9f), OrbitalParams{1.000, 0.0167, glm::radians(0.00), 1.0000, glm::radians(357.52), glm::radians(-11.26), glm::radians(102.94)});
        earth->addChild(std::make_unique<CelestialBody>("Moon", 0.27, glm::vec3(0.6f, 0.6f, 0.6f), OrbitalParams{0.15, 0.0549, glm::radians(5.145), 0.0748, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::move(earth));
        
        // Mars & Moons
        auto mars = std::make_unique<CelestialBody>("Mars", 0.53, glm::vec3(0.8f, 0.4f, 0.3f), OrbitalParams{1.524, 0.0934, glm::radians(1.85), 1.8808, glm::radians(19.41), glm::radians(49.58), glm::radians(286.50)});
        mars->addChild(std::make_unique<CelestialBody>("Phobos", 0.15, glm::vec3(0.5f, 0.4f, 0.4f), OrbitalParams{0.08, 0.0151, glm::radians(1.093), 0.0008, 0.0, 0.0, 0.0}));
        mars->addChild(std::make_unique<CelestialBody>("Deimos", 0.12, glm::vec3(0.6f, 0.5f, 0.5f), OrbitalParams{0.12, 0.0002, glm::radians(0.93), 0.003, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::move(mars));

        // Jupiter & Moons
        auto jupiter = std::make_unique<CelestialBody>("Jupiter", 11.2, glm::vec3(0.8f, 0.7f, 0.6f), OrbitalParams{5.203, 0.0484, glm::radians(1.30), 11.862, glm::radians(20.02), glm::radians(100.55), glm::radians(273.87)});
        jupiter->addChild(std::make_unique<CelestialBody>("Io", 0.28, glm::vec3(0.8f, 0.8f, 0.4f), OrbitalParams{0.40, 0.004, 0.0, 0.0048, 0.0, 0.0, 0.0}));
        jupiter->addChild(std::make_unique<CelestialBody>("Europa", 0.24, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.55, 0.009, 0.0, 0.0097, 0.0, 0.0, 0.0}));
        jupiter->addChild(std::make_unique<CelestialBody>("Ganymede", 0.41, glm::vec3(0.6f, 0.6f, 0.6f), OrbitalParams{0.75, 0.001, 0.0, 0.0196, 0.0, 0.0, 0.0}));
        jupiter->addChild(std::make_unique<CelestialBody>("Callisto", 0.37, glm::vec3(0.5f, 0.5f, 0.5f), OrbitalParams{1.0, 0.007, 0.0, 0.0457, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::move(jupiter));
        
        // Saturn
        auto saturn = std::make_unique<CelestialBody>("Saturn", 9.45, glm::vec3(0.9f, 0.8f, 0.6f), OrbitalParams{9.537, 0.0541, glm::radians(2.49), 29.457, glm::radians(317.02), glm::radians(113.72), glm::radians(339.39)});
        saturn->addChild(std::make_unique<CelestialBody>("Titan", 0.40, glm::vec3(0.8f, 0.6f, 0.2f), OrbitalParams{0.82, 0.028, 0.0, 0.0437, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::move(saturn));
        
        // Uranus
        auto uranus = std::make_unique<CelestialBody>("Uranus", 4.0, glm::vec3(0.6f, 0.8f, 0.9f), OrbitalParams{19.191, 0.0472, glm::radians(0.77), 84.011, glm::radians(142.59), glm::radians(74.00), glm::radians(96.66)});
        uranus->addChild(std::make_unique<CelestialBody>("Titania", 0.12, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.29, 0.001, 0.0, 0.0238, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::move(uranus));

        // Neptune
        auto neptune = std::make_unique<CelestialBody>("Neptune", 3.88, glm::vec3(0.3f, 0.3f, 0.8f), OrbitalParams{30.069, 0.0086, glm::radians(1.77), 164.79, glm::radians(260.25), glm::radians(131.78), glm::radians(272.85)});
        neptune->addChild(std::make_unique<CelestialBody>("Triton", 0.21, glm::vec3(0.7f, 0.6f, 0.8f), OrbitalParams{0.24, 0.0, glm::radians(157.0), 0.016, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::move(neptune));
        
    } else if (systemName == "TRAPPIST-1") {
        m_systemScale = 200.0f; // Even more spread out
        m_planetScale = 0.15f; // Shrink planets significantly to avoid overlap in tight orbits
        // Star: 0.12 Solar Radii => 109 * 0.12 = 13 Earth Radii
        m_bodies.push_back(std::make_unique<CelestialBody>("TRAPPIST-1", 13.0, glm::vec3(1.0f, 0.2f, 0.2f), OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0}));
        
        // b: a=0.011, P=1.51d -> 0.004 yrs
        m_bodies.push_back(std::make_unique<CelestialBody>("b", 1.1, glm::vec3(0.8f, 0.6f, 0.4f), OrbitalParams{0.011, 0.006, glm::radians(89.0), 0.0041, 0.0, 0.0, 0.0}));
        // c: a=0.015, P=2.42d -> 0.0066 yrs
        m_bodies.push_back(std::make_unique<CelestialBody>("c", 1.1, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.015, 0.006, glm::radians(89.0), 0.0066, 1.0, 0.0, 0.0}));
        // d: a=0.021, P=4.05d -> 0.011 yrs
        m_bodies.push_back(std::make_unique<CelestialBody>("d", 0.7, glm::vec3(0.5f, 0.5f, 0.6f), OrbitalParams{0.021, 0.008, glm::radians(89.0), 0.0111, 2.0, 0.0, 0.0}));
        // e: a=0.028, P=6.10d -> 0.016 yrs
        m_bodies.push_back(std::make_unique<CelestialBody>("e", 0.9, glm::vec3(0.4f, 0.4f, 0.8f), OrbitalParams{0.028, 0.005, glm::radians(89.0), 0.0167, 3.0, 0.0, 0.0}));
        // f: a=0.037, P=9.21d -> 0.025 yrs
        m_bodies.push_back(std::make_unique<CelestialBody>("f", 1.0, glm::vec3(0.6f, 0.7f, 0.7f), OrbitalParams{0.037, 0.006, glm::radians(89.0), 0.0252, 4.0, 0.0, 0.0}));
        // g: a=0.045, P=12.35d -> 0.034 yrs
        m_bodies.push_back(std::make_unique<CelestialBody>("g", 1.1, glm::vec3(0.5f, 0.8f, 0.5f), OrbitalParams{0.045, 0.002, glm::radians(89.0), 0.0338, 5.0, 0.0, 0.0}));
        // h: a=0.063, P=18.76d -> 0.051 yrs
        m_bodies.push_back(std::make_unique<CelestialBody>("h", 0.7, glm::vec3(0.4f, 0.4f, 0.4f), OrbitalParams{0.063, 0.005, glm::radians(89.0), 0.0514, 6.0, 0.0, 0.0}));
    
    } else if (systemName == "Kepler-90") {
        m_systemScale = 20.0f; // Moderate scale
        m_planetScale = 0.4f; // Slightly smaller planets
        // Star: 1.2 R_sun => 130 Earth Radii
        m_bodies.push_back(std::make_unique<CelestialBody>("Kepler-90", 130.0, glm::vec3(1.0f, 0.9f, 0.6f), OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0}));
        // b, c, i, d, e, f, g, h
        m_bodies.push_back(std::make_unique<CelestialBody>("b", 1.3, glm::vec3(0.6f), OrbitalParams{0.074, 0.0, glm::radians(89.0), 0.019, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("c", 1.2, glm::vec3(0.6f), OrbitalParams{0.089, 0.0, glm::radians(89.0), 0.024, 1.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("i", 1.3, glm::vec3(0.6f), OrbitalParams{0.12, 0.0, glm::radians(89.0), 0.039, 2.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("d", 2.9, glm::vec3(0.7f), OrbitalParams{0.32, 0.0, glm::radians(89.0), 0.16, 3.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("e", 2.7, glm::vec3(0.7f), OrbitalParams{0.42, 0.0, glm::radians(89.0), 0.25, 4.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("f", 2.9, glm::vec3(0.7f), OrbitalParams{0.48, 0.0, glm::radians(89.0), 0.34, 5.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("g", 8.1, glm::vec3(0.8f, 0.7f, 0.5f), OrbitalParams{0.71, 0.0, glm::radians(89.0), 0.57, 6.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("h", 11.3, glm::vec3(0.7f, 0.7f, 0.9f), OrbitalParams{1.01, 0.0, glm::radians(89.0), 0.91, 7.0, 0.0, 0.0}));

    } else if (systemName == "HR 8799") {
        m_systemScale = 1.0f; // Very wide system, keep scale low
        m_planetScale = 1.0f; // Planets are large, distance is huge, standard scale fine
        // Star: 1.5 R_sun, White
        m_bodies.push_back(std::make_unique<CelestialBody>("HR 8799", 163.0, glm::vec3(0.9f, 0.9f, 1.0f), OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0}));
        // e: 15 AU
        m_bodies.push_back(std::make_unique<CelestialBody>("e", 7.0, glm::vec3(0.8f, 0.4f, 0.4f), OrbitalParams{15.0, 0.0, glm::radians(20.0), 50.0, 0.0, 0.0, 0.0}));
        // d: 24 AU
        m_bodies.push_back(std::make_unique<CelestialBody>("d", 7.0, glm::vec3(0.7f, 0.5f, 0.5f), OrbitalParams{24.0, 0.1, glm::radians(20.0), 100.0, 2.0, 0.0, 0.0}));
        // c: 38 AU
        m_bodies.push_back(std::make_unique<CelestialBody>("c", 7.0, glm::vec3(0.6f, 0.6f, 0.5f), OrbitalParams{38.0, 0.0, glm::radians(20.0), 190.0, 4.0, 0.0, 0.0}));
        // b: 68 AU
        m_bodies.push_back(std::make_unique<CelestialBody>("b", 6.0, glm::vec3(0.5f, 0.5f, 0.7f), OrbitalParams{68.0, 0.0, glm::radians(20.0), 450.0, 5.0, 0.0, 0.0}));
    
    } else if (systemName == "Kepler-11") {
        m_systemScale = 40.0f; // Compact system, increase orbit spread
        m_planetScale = 0.3f; // Smaller planets
        m_bodies.push_back(std::make_unique<CelestialBody>("Kepler-11", 119.0, glm::vec3(1.0f, 1.0f, 0.0f), OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("b", 2.0, glm::vec3(0.7f), OrbitalParams{0.091, 0.0, glm::radians(89.0), 0.028, 0.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("c", 3.1, glm::vec3(0.6f), OrbitalParams{0.106, 0.0, glm::radians(89.0), 0.035, 1.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("d", 3.4, glm::vec3(0.6f), OrbitalParams{0.159, 0.0, glm::radians(89.0), 0.061, 2.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("e", 4.5, glm::vec3(0.6f), OrbitalParams{0.194, 0.0, glm::radians(89.0), 0.087, 3.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("f", 2.6, glm::vec3(0.6f), OrbitalParams{0.25, 0.0, glm::radians(89.0), 0.12, 4.0, 0.0, 0.0}));
        m_bodies.push_back(std::make_unique<CelestialBody>("g", 3.6, glm::vec3(0.6f), OrbitalParams{0.46, 0.0, glm::radians(89.0), 0.32, 5.0, 0.0, 0.0}));

    } else if (systemName == "55 Cancri") {
        m_systemScale = 20.0f;
        m_planetScale = 0.3f;
        m_bodies.push_back(std::make_unique<CelestialBody>("55 Cancri", 100.0, glm::vec3(1.0f, 0.8f, 0.4f), OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0}));
        // e: 0.015 AU, 0.7d
        m_bodies.push_back(std::make_unique<CelestialBody>("e", 2.0, glm::vec3(0.9f, 0.5f, 0.5f), OrbitalParams{0.015, 0.0, 0.0, 0.002, 0.0, 0.0, 0.0}));
        // b: 0.11 AU
        m_bodies.push_back(std::make_unique<CelestialBody>("b", 9.0, glm::vec3(0.8f, 0.6f, 0.4f), OrbitalParams{0.11, 0.0, 0.0, 0.04, 1.0, 0.0, 0.0}));
        // c: 0.24 AU
        m_bodies.push_back(std::make_unique<CelestialBody>("c", 9.0, glm::vec3(0.7f, 0.6f, 0.4f), OrbitalParams{0.24, 0.0, 0.0, 0.12, 2.0, 0.0, 0.0}));
        // f: 0.78 AU
        m_bodies.push_back(std::make_unique<CelestialBody>("f", 9.0, glm::vec3(0.6f, 0.5f, 0.4f), OrbitalParams{0.78, 0.0, 0.0, 0.71, 3.0, 0.0, 0.0}));
        // d: 5.7 AU
        m_bodies.push_back(std::make_unique<CelestialBody>("d", 10.0, glm::vec3(0.5f, 0.5f, 0.8f), OrbitalParams{5.7, 0.0, 0.0, 14.0, 4.0, 0.0, 0.0}));
    }
}

const CelestialBody* SolarSystem::getSun() const {
    if (m_bodies.empty()) return nullptr;
    return m_bodies[0].get();
}

} // namespace Simulation

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
    auto earth = std::make_unique<CelestialBody>(
        "Earth", 
        1.0, 
        glm::vec3(0.2f, 0.4f, 0.9f), 
        OrbitalParams{1.000, 0.0167, glm::radians(0.00), 1.0000, glm::radians(357.52), glm::radians(-11.26), glm::radians(102.94)}
    );

    // Moon
    // a = 0.15 Visual AU (approx 60000 Earth Radii? No. 0.15 AU ~ 22 Million km. Moon is 0.38M km. This is 60x exaggeration).
    // Period = 0.0748 years.
    earth->addChild(std::make_unique<CelestialBody>(
        "Moon",
        0.27, // 0.27 Earth Radii
        glm::vec3(0.6f, 0.6f, 0.6f),
        OrbitalParams{0.15, 0.0549, glm::radians(5.145), 0.0748, 0.0, 0.0, 0.0}
    ));

    m_bodies.push_back(std::move(earth));

    // Mars
    auto mars = std::make_unique<CelestialBody>(
        "Mars", 
        0.53, 
        glm::vec3(0.8f, 0.4f, 0.3f), 
        OrbitalParams{1.524, 0.0934, glm::radians(1.85), 1.8808, glm::radians(19.41), glm::radians(49.58), glm::radians(286.50)}
    );
    
    // Phobos
    // a = visual exaggerated (0.08 AU?)
    mars->addChild(std::make_unique<CelestialBody>(
        "Phobos", 0.15, glm::vec3(0.5f, 0.4f, 0.4f),
        OrbitalParams{0.08, 0.0151, glm::radians(1.093), 0.0008, 0.0, 0.0, 0.0} // Very fast
    ));
    
    // Deimos
    mars->addChild(std::make_unique<CelestialBody>(
        "Deimos", 0.12, glm::vec3(0.6f, 0.5f, 0.5f),
        OrbitalParams{0.12, 0.0002, glm::radians(0.93), 0.003, 0.0, 0.0, 0.0}
    ));
    
    m_bodies.push_back(std::move(mars));
    
    // Jupiter
    auto jupiter = std::make_unique<CelestialBody>(
        "Jupiter", 
        11.2, 
        glm::vec3(0.8f, 0.7f, 0.6f), 
        OrbitalParams{5.203, 0.0484, glm::radians(1.30), 11.862, glm::radians(20.02), glm::radians(100.55), glm::radians(273.87)}
    );

    // Io
    jupiter->addChild(std::make_unique<CelestialBody>("Io", 0.28, glm::vec3(0.8f, 0.8f, 0.4f), OrbitalParams{0.40, 0.004, 0.0, 0.0048, 0.0, 0.0, 0.0}));
    // Europa
    jupiter->addChild(std::make_unique<CelestialBody>("Europa", 0.24, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.55, 0.009, 0.0, 0.0097, 0.0, 0.0, 0.0}));
    // Ganymede
    jupiter->addChild(std::make_unique<CelestialBody>("Ganymede", 0.41, glm::vec3(0.6f, 0.6f, 0.6f), OrbitalParams{0.75, 0.001, 0.0, 0.0196, 0.0, 0.0, 0.0}));
    // Callisto
    jupiter->addChild(std::make_unique<CelestialBody>("Callisto", 0.37, glm::vec3(0.5f, 0.5f, 0.5f), OrbitalParams{1.0, 0.007, 0.0, 0.0457, 0.0, 0.0, 0.0}));

    m_bodies.push_back(std::move(jupiter));

    // Titan
    auto saturn = std::make_unique<CelestialBody>(
        "Saturn", 
        9.45, 
        glm::vec3(0.9f, 0.8f, 0.6f), 
        OrbitalParams{9.537, 0.0541, glm::radians(2.49), 29.457, glm::radians(317.02), glm::radians(113.72), glm::radians(339.39)}
    );
    saturn->addChild(std::make_unique<CelestialBody>("Titan", 0.40, glm::vec3(0.8f, 0.6f, 0.2f), OrbitalParams{0.82, 0.028, 0.0, 0.0437, 0.0, 0.0, 0.0}));
    
    m_bodies.push_back(std::move(saturn));

    // Uranus
    auto uranus = std::make_unique<CelestialBody>(
        "Uranus", 
        4.0, 
        glm::vec3(0.6f, 0.8f, 0.9f), 
        OrbitalParams{19.191, 0.0472, glm::radians(0.77), 84.011, glm::radians(142.59), glm::radians(74.00), glm::radians(96.66)}
    );
    // Titania
    uranus->addChild(std::make_unique<CelestialBody>("Titania", 0.12, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.29, 0.001, 0.0, 0.0238, 0.0, 0.0, 0.0}));
    
    m_bodies.push_back(std::move(uranus));

    // Neptune
    auto neptune = std::make_unique<CelestialBody>(
        "Neptune", 
        3.88, 
        glm::vec3(0.3f, 0.3f, 0.8f), 
        OrbitalParams{30.069, 0.0086, glm::radians(1.77), 164.79, glm::radians(260.25), glm::radians(131.78), glm::radians(272.85)}
    );
    // Triton
    neptune->addChild(std::make_unique<CelestialBody>("Triton", 0.21, glm::vec3(0.7f, 0.6f, 0.8f), OrbitalParams{0.24, 0.0, glm::radians(157.0), 0.016, 0.0, 0.0, 0.0})); // Retrograde

    m_bodies.push_back(std::move(neptune));
}

const CelestialBody* SolarSystem::getSun() const {
    if (m_bodies.empty()) return nullptr;
    return m_bodies[0].get();
}

} // namespace Simulation

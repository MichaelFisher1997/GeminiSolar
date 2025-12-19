#include "SolarSystem.hpp"
#include "SystemLoader.hpp"
#include "core/Logger.hpp"

namespace Simulation {

SolarSystem::SolarSystem() 
    : m_physicsSimulator(std::make_unique<PhysicsSimulator>()) {
}

void SolarSystem::init() {
    loadSystem("Solar System");
}

void SolarSystem::loadSystem(const std::string& systemName) {
    m_bodies.clear();
    m_currentSystemName = systemName;
    
    // Try to load from JSON file
    std::string filePath = SystemLoader::getSystemFilePath(systemName);
    auto systemData = SystemLoader::loadFromFile(filePath);
    
    if (systemData) {
        m_systemScale = systemData->systemScale;
        m_planetScale = systemData->planetScale;
        m_bodies = std::move(systemData->bodies);
        LOG_INFO("SolarSystem", "Loaded '", systemName, "' from JSON");
    } else {
        // Fallback to hardcoded Solar System if JSON loading fails
        LOG_WARN("SolarSystem", "Failed to load '", systemName, "' from JSON, using fallback");
        loadFallbackSolarSystem();
    }
    
    if (m_physicsEnabled) {
        resetPhysics();
    }
}

void SolarSystem::setPhysicsEnabled(bool enabled) {
    if (m_physicsEnabled == enabled) return;
    m_physicsEnabled = enabled;
    if (m_physicsEnabled) {
        resetPhysics();
    }
}

void SolarSystem::resetPhysics() {
    // Delegate to PhysicsSimulator (SRP)
    m_physicsSimulator->initializeFromOrbits(m_bodies, 0.0);
}

void SolarSystem::updatePhysics(double dt) {
    if (!m_physicsEnabled) return;
    // Delegate to PhysicsSimulator (SRP)
    m_physicsSimulator->update(m_bodies, dt);
}

void SolarSystem::loadFallbackSolarSystem() {
    m_currentSystemName = "Solar System";
    m_systemScale = 10.0f;
    m_planetScale = 0.5f;
    
    // Sun
    m_bodies.push_back(std::make_unique<CelestialBody>("Sun", 109.0, glm::vec3(1.0f, 1.0f, 0.0f), OrbitalParams{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0}, BodyType::Star));
    // Mercury
    m_bodies.push_back(std::make_unique<CelestialBody>("Mercury", 0.38, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.387, 0.2056, glm::radians(7.00), 0.2408, glm::radians(174.79), glm::radians(48.33), glm::radians(29.12)}, BodyType::Planet));
    // Venus
    m_bodies.push_back(std::make_unique<CelestialBody>("Venus", 0.95, glm::vec3(0.9f, 0.8f, 0.5f), OrbitalParams{0.723, 0.0068, glm::radians(3.39), 0.6152, glm::radians(50.11), glm::radians(76.68), glm::radians(54.85)}, BodyType::Planet));
    
    // Earth & Moon
    auto earth = std::make_unique<CelestialBody>("Earth", 1.0, glm::vec3(0.2f, 0.4f, 0.9f), OrbitalParams{1.000, 0.0167, glm::radians(0.00), 1.0000, glm::radians(357.52), glm::radians(-11.26), glm::radians(102.94)}, BodyType::Planet);
    earth->addChild(std::make_unique<CelestialBody>("Moon", 0.27, glm::vec3(0.6f, 0.6f, 0.6f), OrbitalParams{0.15, 0.0549, glm::radians(5.145), 0.0748, 0.0, 0.0, 0.0}, BodyType::Moon));
    m_bodies.push_back(std::move(earth));
    
    // Mars & Moons
    auto mars = std::make_unique<CelestialBody>("Mars", 0.53, glm::vec3(0.8f, 0.4f, 0.3f), OrbitalParams{1.524, 0.0934, glm::radians(1.85), 1.8808, glm::radians(19.41), glm::radians(49.58), glm::radians(286.50)}, BodyType::Planet);
    mars->addChild(std::make_unique<CelestialBody>("Phobos", 0.15, glm::vec3(0.5f, 0.4f, 0.4f), OrbitalParams{0.08, 0.0151, glm::radians(1.093), 0.0008, 0.0, 0.0, 0.0}, BodyType::Moon));
    mars->addChild(std::make_unique<CelestialBody>("Deimos", 0.12, glm::vec3(0.6f, 0.5f, 0.5f), OrbitalParams{0.12, 0.0002, glm::radians(0.93), 0.003, 0.0, 0.0, 0.0}, BodyType::Moon));
    m_bodies.push_back(std::move(mars));

    // Jupiter & Moons
    auto jupiter = std::make_unique<CelestialBody>("Jupiter", 11.2, glm::vec3(0.8f, 0.7f, 0.6f), OrbitalParams{5.203, 0.0484, glm::radians(1.30), 11.862, glm::radians(20.02), glm::radians(100.55), glm::radians(273.87)}, BodyType::Planet);
    jupiter->addChild(std::make_unique<CelestialBody>("Io", 0.28, glm::vec3(0.8f, 0.8f, 0.4f), OrbitalParams{0.40, 0.004, 0.0, 0.0048, 0.0, 0.0, 0.0}, BodyType::Moon));
    jupiter->addChild(std::make_unique<CelestialBody>("Europa", 0.24, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.55, 0.009, 0.0, 0.0097, 0.0, 0.0, 0.0}, BodyType::Moon));
    jupiter->addChild(std::make_unique<CelestialBody>("Ganymede", 0.41, glm::vec3(0.6f, 0.6f, 0.6f), OrbitalParams{0.75, 0.001, 0.0, 0.0196, 0.0, 0.0, 0.0}, BodyType::Moon));
    jupiter->addChild(std::make_unique<CelestialBody>("Callisto", 0.37, glm::vec3(0.5f, 0.5f, 0.5f), OrbitalParams{1.0, 0.007, 0.0, 0.0457, 0.0, 0.0, 0.0}, BodyType::Moon));
    m_bodies.push_back(std::move(jupiter));
    
    // Saturn
    auto saturn = std::make_unique<CelestialBody>("Saturn", 9.45, glm::vec3(0.9f, 0.8f, 0.6f), OrbitalParams{9.537, 0.0541, glm::radians(2.49), 29.457, glm::radians(317.02), glm::radians(113.72), glm::radians(339.39)}, BodyType::Planet);
    saturn->addChild(std::make_unique<CelestialBody>("Titan", 0.40, glm::vec3(0.8f, 0.6f, 0.2f), OrbitalParams{0.82, 0.028, 0.0, 0.0437, 0.0, 0.0, 0.0}, BodyType::Moon));
    m_bodies.push_back(std::move(saturn));
    
    // Uranus
    auto uranus = std::make_unique<CelestialBody>("Uranus", 4.0, glm::vec3(0.6f, 0.8f, 0.9f), OrbitalParams{19.191, 0.0472, glm::radians(0.77), 84.011, glm::radians(142.59), glm::radians(74.00), glm::radians(96.66)}, BodyType::Planet);
    uranus->addChild(std::make_unique<CelestialBody>("Titania", 0.12, glm::vec3(0.7f, 0.7f, 0.7f), OrbitalParams{0.29, 0.001, 0.0, 0.0238, 0.0, 0.0, 0.0}, BodyType::Moon));
    m_bodies.push_back(std::move(uranus));

    // Neptune
    auto neptune = std::make_unique<CelestialBody>("Neptune", 3.88, glm::vec3(0.3f, 0.3f, 0.8f), OrbitalParams{30.069, 0.0086, glm::radians(1.77), 164.79, glm::radians(260.25), glm::radians(131.78), glm::radians(272.85)}, BodyType::Planet);
    neptune->addChild(std::make_unique<CelestialBody>("Triton", 0.21, glm::vec3(0.7f, 0.6f, 0.8f), OrbitalParams{0.24, 0.0, glm::radians(157.0), 0.016, 0.0, 0.0, 0.0}, BodyType::Moon));
    m_bodies.push_back(std::move(neptune));
}

const CelestialBody* SolarSystem::getSun() const {
    if (m_bodies.empty()) return nullptr;
    return m_bodies[0].get();
}

} // namespace Simulation

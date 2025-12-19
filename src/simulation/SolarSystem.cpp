#include "SolarSystem.hpp"
#include "SystemLoader.hpp"
#include "core/Logger.hpp"

namespace Simulation {

SolarSystem::SolarSystem() {
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
    // Initialize physics state from current orbital positions
    double time = 0.0; // Current epoch
    
    std::function<void(CelestialBody&, glm::vec3, glm::vec3)> initBody;
    initBody = [&](CelestialBody& body, glm::vec3 parentPos, glm::vec3 parentVel) {
        glm::vec3 localPos = body.getPosition(time);
        glm::vec3 worldPos = parentPos + localPos;
        
        // Calculate orbital velocity: v = sqrt(G*M/r)
        // For simplicity, we'll estimate velocity based on the change in position over a small step
        double dt = 0.0001;
        glm::vec3 nextPos = body.getPosition(time + dt);
        glm::vec3 localVel = (nextPos - localPos) / static_cast<float>(dt);
        glm::vec3 worldVel = parentVel + localVel;
        
        body.setPosition(worldPos);
        body.setVelocity(worldVel);
        
        // Set mass based on radius cubed (approx density = 1)
        double r = body.getRadius();
        body.setMass(r * r * r);
        
        for (auto& child : body.getChildren()) {
            initBody(*child, worldPos, worldVel);
        }
    };
    
    for (auto& body : m_bodies) {
        initBody(*body, glm::vec3(0.0f), glm::vec3(0.0f));
    }
    
    // Special handling for the star mass (Sun is huge)
    if (!m_bodies.empty()) {
        m_bodies[0]->setMass(1000000.0); // Mass in arbitrary units
    }
}

void SolarSystem::updatePhysics(double dt) {
    if (!m_physicsEnabled) return;

    // Collect all bodies into a flat vector for easier iteration
    std::vector<CelestialBody*> allBodies;
    std::function<void(CelestialBody*)> collect;
    collect = [&](CelestialBody* b) {
        allBodies.push_back(b);
        for (auto& child : b->getChildren()) collect(child.get());
    };
    for (auto& b : m_bodies) collect(b.get());

    // 1. Compute forces (Gravity)
    std::vector<glm::vec3> accelerations(allBodies.size(), glm::vec3(0.0f));
    for (size_t i = 0; i < allBodies.size(); ++i) {
        for (size_t j = i + 1; j < allBodies.size(); ++j) {
            glm::vec3 rVec = allBodies[j]->getPhysicsPosition() - allBodies[i]->getPhysicsPosition();
            float distSq = glm::dot(rVec, rVec);
            
            // Softening factor prevents infinite forces at near-zero distances
            const float softeningSq = 0.001f;
            float forceMag = static_cast<float>(m_gravityConstant / (distSq + softeningSq));
            
            float dist = std::sqrt(distSq);
            if (dist < 0.0001f) continue; 
            
            glm::vec3 dir = rVec / dist;
            
            accelerations[i] += dir * forceMag * static_cast<float>(allBodies[j]->getMass());
            accelerations[j] -= dir * forceMag * static_cast<float>(allBodies[i]->getMass());
        }
    }

    // 2. Update positions and velocities (Semi-Implicit Euler)
    for (size_t i = 0; i < allBodies.size(); ++i) {
        glm::vec3 v = allBodies[i]->getPhysicsVelocity();
        v += accelerations[i] * static_cast<float>(dt);
        allBodies[i]->setVelocity(v);
        
        glm::vec3 p = allBodies[i]->getPhysicsPosition();
        p += v * static_cast<float>(dt);
        allBodies[i]->setPosition(p);
    }
}

void SolarSystem::loadFallbackSolarSystem() {
    m_currentSystemName = "Solar System";
    m_systemScale = 10.0f;
    m_planetScale = 0.5f;
    
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
}

const CelestialBody* SolarSystem::getSun() const {
    if (m_bodies.empty()) return nullptr;
    return m_bodies[0].get();
}

} // namespace Simulation

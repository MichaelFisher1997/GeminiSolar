#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Simulation {

/// Type of celestial body for proper type-based behavior
enum class BodyType {
    Star,       // Central star (Sun, etc.)
    Planet,     // Major planet
    Moon,       // Natural satellite
    DwarfPlanet,
    Asteroid,
    Comet,
    BlackHole,  // Special case for singularities
    Other
};

struct OrbitalParams {
    double semiMajorAxis;  // AU
    double eccentricity;   // 0..1
    double inclination;    // Radians
    double orbitalPeriod;  // Earth years
    double meanAnomaly0;   // Radians at t=0 (Mean Anomaly at Epoch)
    double longitudeAscendingNode; // Radians (Omega)
    double argumentPeriapsis;      // Radians (omega)
};

class CelestialBody {
public:
    CelestialBody(const std::string& name, 
                  double radius, 
                  const glm::vec3& color, 
                  const OrbitalParams& orbitalParams,
                  BodyType type = BodyType::Planet);

    glm::vec3 getPosition(double time) const;

    const std::string& getName() const { return m_name; }
    double getRadius() const { return m_radius; }
    const glm::vec3& getColor() const { return m_color; }
    const OrbitalParams& getOrbitalParams() const { return m_orbitalParams; }
    
    // Body type methods
    BodyType getType() const { return m_type; }
    void setType(BodyType type) { m_type = type; }
    bool isStar() const { return m_type == BodyType::Star || m_type == BodyType::BlackHole; }
    
    // Physics State
    void setPosition(const glm::vec3& pos) { m_physicsPosition = pos; }
    void setVelocity(const glm::vec3& vel) { m_physicsVelocity = vel; }
    void setMass(double mass) { m_mass = mass; }
    
    glm::vec3 getPhysicsPosition() const { return m_physicsPosition; }
    glm::vec3 getPhysicsVelocity() const { return m_physicsVelocity; }
    double getMass() const { return m_mass; }
    
    void addChild(std::unique_ptr<CelestialBody> child);
    const std::vector<std::unique_ptr<CelestialBody>>& getChildren() const { return m_children; }
    
    // Recursive position calculation (in AU)
    glm::vec3 getWorldPosition(double time) const;
    
    void setParent(const CelestialBody* parent) { m_parent = parent; }
    const CelestialBody* getParent() const { return m_parent; }

private:
    std::string m_name;
    double m_radius;
    glm::vec3 m_color;
    OrbitalParams m_orbitalParams;
    BodyType m_type;
    std::vector<std::unique_ptr<CelestialBody>> m_children;
    const CelestialBody* m_parent = nullptr;

    // Physics State
    glm::vec3 m_physicsPosition = glm::vec3(0.0f);
    glm::vec3 m_physicsVelocity = glm::vec3(0.0f);
    double m_mass = 1.0;
};

} // namespace Simulation

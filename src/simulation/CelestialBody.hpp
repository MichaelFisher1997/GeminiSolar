#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace Simulation {

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
                  const OrbitalParams& orbitalParams);

    glm::vec3 getPosition(double time) const;

    const std::string& getName() const { return m_name; }
    double getRadius() const { return m_radius; }
    const glm::vec3& getColor() const { return m_color; }
    const OrbitalParams& getOrbitalParams() const { return m_orbitalParams; }
    
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
    std::vector<std::unique_ptr<CelestialBody>> m_children;
    const CelestialBody* m_parent = nullptr;
};

} // namespace Simulation

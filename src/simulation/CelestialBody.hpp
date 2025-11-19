#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Simulation {

struct OrbitalParams {
    double semiMajorAxis;  // AU (or scaled units)
    double eccentricity;   // 0..1
    double inclination;    // Radians
    double orbitalPeriod;  // Earth years (or seconds)
    double meanAnomaly0;   // Radians at t=0
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

private:
    std::string m_name;
    double m_radius;
    glm::vec3 m_color;
    OrbitalParams m_orbitalParams;
};

} // namespace Simulation

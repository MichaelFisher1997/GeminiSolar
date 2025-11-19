#include "CelestialBody.hpp"
#include "OrbitModel.hpp"

namespace Simulation {

CelestialBody::CelestialBody(const std::string& name, 
                             double radius, 
                             const glm::vec3& color, 
                             const OrbitalParams& orbitalParams)
    : m_name(name)
    , m_radius(radius)
    , m_color(color)
    , m_orbitalParams(orbitalParams) {
}

glm::vec3 CelestialBody::getPosition(double time) const {
    return OrbitModel::calculatePosition(m_orbitalParams, time);
}

} // namespace Simulation

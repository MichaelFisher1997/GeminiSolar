#include "CelestialBody.hpp"
#include "OrbitModel.hpp"

namespace Simulation {

CelestialBody::CelestialBody(const std::string& name, 
                             double radius, 
                             const glm::vec3& color, 
                             const OrbitalParams& orbitalParams,
                             BodyType type)
    : m_name(name)
    , m_radius(radius)
    , m_color(color)
    , m_orbitalParams(orbitalParams)
    , m_type(type) {
}

glm::vec3 CelestialBody::getPosition(double time) const {
    return OrbitModel::calculatePosition(m_orbitalParams, time);
}

void CelestialBody::addChild(std::unique_ptr<CelestialBody> child) {
    child->setParent(this);
    m_children.push_back(std::move(child));
}

glm::vec3 CelestialBody::getWorldPosition(double time) const {
    glm::vec3 localPos = getPosition(time);
    if (m_parent) {
        return m_parent->getWorldPosition(time) + localPos;
    }
    return localPos;
}

} // namespace Simulation

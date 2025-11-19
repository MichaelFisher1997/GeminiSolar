#pragma once

#include "CelestialBody.hpp"
#include <glm/glm.hpp>

namespace Simulation {

class OrbitModel {
public:
    // Returns position in 3D space given orbital parameters and time
    static glm::vec3 calculatePosition(const OrbitalParams& params, double time);
};

} // namespace Simulation

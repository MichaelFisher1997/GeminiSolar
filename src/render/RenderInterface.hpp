#pragma once

#include "simulation/SolarSystem.hpp"
#include "Camera.hpp"

namespace Render {

class RenderInterface {
public:
    virtual ~RenderInterface() = default;
    virtual void render(const Simulation::SolarSystem& solarSystem, const Camera& camera, double simulationTime) = 0;
};

} // namespace Render

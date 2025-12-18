#pragma once

#include "simulation/SolarSystem.hpp"
#include "Camera.hpp"
#include <functional>

namespace Render {

class RenderInterface {
public:
    virtual ~RenderInterface() = default;
    virtual void render(const Simulation::SolarSystem& solarSystem, 
                        const Camera& camera, 
                        double simulationTime, 
                        const Simulation::CelestialBody* hoveredBody = nullptr,
                        std::function<void()> uiCallback = nullptr) = 0;
    virtual void resize(int width, int height) = 0;
};

} // namespace Render

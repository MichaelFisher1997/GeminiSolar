#pragma once

#include "simulation/SolarSystem.hpp"
#include "Camera.hpp"
#include <functional>

namespace Render {

class RenderInterface {
public:
    virtual ~RenderInterface() = default;
    virtual void render(const Simulation::SolarSystem& solarSystem, const Camera& camera, double simulationTime, std::function<void()> uiCallback = nullptr) = 0;
    virtual void resize(int width, int height) = 0;
};

} // namespace Render

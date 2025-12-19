#pragma once

#include "simulation/SolarSystem.hpp"
#include "render/Camera.hpp"
#include "platform/WindowInterface.hpp"

namespace Core {

/// Handles raycasting and body picking logic
/// Extracted from App to follow Single Responsibility Principle
class BodyPicker {
public:
    BodyPicker(Platform::WindowInterface& window);
    
    /// Pick a body at the given screen coordinates
    /// Returns nullptr if no body was hit
    const Simulation::CelestialBody* pickBody(
        float mouseX, float mouseY,
        const Simulation::SolarSystem& solarSystem,
        const Render::Camera& camera,
        double simulationTime,
        bool showLabels
    );
    
private:
    Platform::WindowInterface& m_window;
};

} // namespace Core

#pragma once

#include "simulation/SolarSystem.hpp"
#include "simulation/SystemLoader.hpp"
#include "core/Time.hpp"
#include "Camera.hpp"
#include "platform/WindowInterface.hpp"
#include <functional>

namespace Render {

/// Callback types for UI actions
struct UICallbacks {
    std::function<void(const std::string&)> onSystemSelected;
    std::function<void(const Simulation::CelestialBody*)> onBodySelected;
    std::function<void()> onClearFocus;
    std::function<void(float distance, float duration)> onCameraTransition;
};

/// Manages all ImGui-based simulation UI rendering
/// Extracted from App to follow Single Responsibility Principle
class SimulationUI {
public:
    SimulationUI(Platform::WindowInterface& window);
    
    /// Render all UI panels
    /// Returns the body that was clicked in the celestial bodies panel (or nullptr)
    void render(
        Simulation::SolarSystem& solarSystem,
        Core::Time& time,
        const Camera& camera,
        const Simulation::CelestialBody* hoveredBody,
        const Simulation::CelestialBody* selectedBody,
        const Simulation::CelestialBody* lockedBody,
        const UICallbacks& callbacks
    );
    
    /// Render planet labels as background overlay
    void renderLabels(
        const Simulation::SolarSystem& solarSystem,
        const Camera& camera,
        double simulationTime,
        const Simulation::CelestialBody* hoveredBody,
        const Simulation::CelestialBody* selectedBody,
        const Simulation::CelestialBody* lockedBody,
        bool showLabels
    );
    
    /// Render body info tooltip
    void renderBodyTooltip(
        const Simulation::CelestialBody* body,
        const Simulation::CelestialBody* hoveredBody,
        const Simulation::CelestialBody* selectedBody,
        float mouseX, float mouseY,
        bool uiWantsMouse
    );
    
    // Settings accessors for render options
    void setShowOrbits(bool show) { m_showOrbits = show; }
    bool isShowOrbits() const { return m_showOrbits; }
    void setShowLabels(bool show) { m_showLabels = show; }
    bool isShowLabels() const { return m_showLabels; }

private:
    void renderStatsOverlay(const Core::Time& time);
    void renderControlPanel(
        Simulation::SolarSystem& solarSystem,
        Core::Time& time,
        const UICallbacks& callbacks
    );
    void renderBodiesPanel(
        Simulation::SolarSystem& solarSystem,
        const Core::Time& time,
        const Simulation::CelestialBody* lockedBody,
        const UICallbacks& callbacks
    );
    void renderHelpButton();
    
    Platform::WindowInterface& m_window;
    bool m_showOrbits = true;
    bool m_showLabels = true;
    bool m_showHelp = false;
};

} // namespace Render

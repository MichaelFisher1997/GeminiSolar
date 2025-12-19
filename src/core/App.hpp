#pragma once

#include <memory>
#include "platform/SDLWindow.hpp"
#include "core/InputManager.hpp"
#include "core/Time.hpp"
#include "core/BodyPicker.hpp"
#include "simulation/SolarSystem.hpp"
#include "render/Camera.hpp"
#include "render/GLRenderer.hpp"
#include "render/SimulationUI.hpp"

namespace Core {

/// Main application class - orchestrates all subsystems
/// Refactored to delegate UI rendering and body picking to separate classes (SRP)
class App {
public:
    App();
    ~App();

    /// Run the main application loop
    void run();
    
    /// Request graceful shutdown
    void requestShutdown() { m_isRunning = false; }
    
    /// Check if external shutdown was requested (e.g., signal)
    static bool isShutdownRequested();

private:
    void processInput(float deltaTime);
    void update(float deltaTime);
    void render();
    void shutdown();
    
    // Interaction helpers
    void updateHover();
    void handleBodySelection(const Simulation::CelestialBody* body);
    void handleSystemChange(const std::string& systemName);

    // Core systems - using SDLWindow concrete type for now due to GL context needs
    // Note: Ideally would use WindowInterface* but GLRenderer needs SDL-specific features
    std::unique_ptr<Platform::SDLWindow> m_window;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<Time> m_time;
    std::unique_ptr<BodyPicker> m_bodyPicker;
    
    // Simulation
    std::unique_ptr<Simulation::SolarSystem> m_solarSystem;
    
    // Rendering
    std::unique_ptr<Render::Camera> m_camera;
    std::unique_ptr<Render::GLRenderer> m_renderer;
    std::unique_ptr<Render::SimulationUI> m_simulationUI;
    
    // State
    const Simulation::CelestialBody* m_lockedBody = nullptr;
    const Simulation::CelestialBody* m_hoveredBody = nullptr;
    const Simulation::CelestialBody* m_selectedBody = nullptr;
    
    float m_clickTime = 0.0f;
    const float DOUBLE_CLICK_TIME = 0.3f;
    
    bool m_isRunning = false;
};

} // namespace Core

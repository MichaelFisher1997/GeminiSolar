#pragma once

#include <memory>
#include "platform/SDLWindow.hpp"
#include "core/InputManager.hpp"
#include "core/Time.hpp"
#include "simulation/SolarSystem.hpp"
#include "render/Camera.hpp"
#include "render/GLRenderer.hpp"

namespace Core {

/// Main application class - orchestrates all subsystems
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
    void renderUI();
    void shutdown();
    
    // Interaction
    void updateHover();
    const Simulation::CelestialBody* pickBody(float mouseX, float mouseY);

    // Core systems
    std::unique_ptr<Platform::SDLWindow> m_window;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<Time> m_time;
    
    // Simulation
    std::unique_ptr<Simulation::SolarSystem> m_solarSystem;
    
    // Rendering
    std::unique_ptr<Render::Camera> m_camera;
    std::unique_ptr<Render::GLRenderer> m_renderer;
    
    // State
    const Simulation::CelestialBody* m_lockedBody = nullptr;
    const Simulation::CelestialBody* m_hoveredBody = nullptr;
    const Simulation::CelestialBody* m_selectedBody = nullptr;
    
    float m_clickTime = 0.0f;
    const float DOUBLE_CLICK_TIME = 0.3f;
    
    bool m_isRunning = false;
};

} // namespace Core

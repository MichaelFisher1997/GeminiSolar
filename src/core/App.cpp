#include "App.hpp"
#include "render/GLRenderer.hpp"
#include "Logger.hpp"
#include "imgui.h"
#include <SDL3/SDL.h>
#include <chrono>
#include <atomic>

namespace {
    // Global shutdown flag for signal handling
    std::atomic<bool> g_shutdownRequested{false};
}

namespace Core {

// Static method to check shutdown request
bool App::isShutdownRequested() {
    return g_shutdownRequested.load(std::memory_order_relaxed);
}

// Allow external code to request shutdown (e.g., signal handlers)
void requestGlobalShutdown() {
    g_shutdownRequested.store(true, std::memory_order_relaxed);
}

App::App() {
    LOG_INFO("App", "Initializing application...");
    
    // Create window
    m_window = std::make_unique<Platform::SDLWindow>("Solar System Simulator", 1280, 720);
    m_window->createGLContext();
    
    // Create input manager
    m_inputManager = std::make_unique<InputManager>();
    
    // Create body picker (SRP - extracted from App)
    m_bodyPicker = std::make_unique<BodyPicker>(*m_window);
    
    // Get initial window size
    int width, height;
    m_window->getSize(width, height);
    
    // Create camera
    m_camera = std::make_unique<Render::Camera>(static_cast<float>(width), static_cast<float>(height));
    
    // Create renderer
    m_renderer = std::make_unique<Render::GLRenderer>(*m_window);
    
    // Create simulation UI (SRP - extracted from App)
    m_simulationUI = std::make_unique<Render::SimulationUI>(*m_window);
    
    // Set up raw event forwarding to ImGui
    m_window->setRawEventCallback([this](const SDL_Event& event) {
        m_renderer->processEvent(event);
    });
    
    // Create time manager
    m_time = std::make_unique<Time>();
    m_time->setTimeScale(1.0 / 365.25);  // 1 real second = 1 simulated day
    
    // Create solar system
    m_solarSystem = std::make_unique<Simulation::SolarSystem>();
    m_solarSystem->init();
    
    LOG_INFO("App", "Application initialized successfully");
}

App::~App() {
    shutdown();
}

void App::shutdown() {
    if (!m_isRunning && !m_renderer) {
        return;  // Already shut down
    }
    
    LOG_INFO("App", "Shutting down application...");
    
    // Explicit cleanup in reverse order of creation
    m_lockedBody = nullptr;
    m_solarSystem.reset();
    m_simulationUI.reset();
    m_renderer.reset();
    m_camera.reset();
    m_bodyPicker.reset();
    m_time.reset();
    m_inputManager.reset();
    // Window is destroyed last (destructor handles it)
    
    LOG_INFO("App", "Application shutdown complete");
}

void App::run() {
    m_isRunning = true;
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (m_isRunning && !m_window->shouldClose() && !isShutdownRequested()) {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Process events
        m_window->pollEvents([this](const Platform::WindowEvent& event) {
            // Forward to input manager
            m_inputManager->processEvent(event);
            
            // Handle window-specific events
            if (event.type == Platform::WindowEventType::Quit) {
                m_isRunning = false;
            }
            else if (event.type == Platform::WindowEventType::Resized) {
                m_camera->setAspectRatio(static_cast<float>(event.width), 
                                         static_cast<float>(event.height));
                m_renderer->resize(event.width, event.height);
            }
        });
        
        // Check for external shutdown request
        if (isShutdownRequested()) {
            LOG_INFO("App", "External shutdown requested");
            m_isRunning = false;
            break;
        }
        
        // Update UI state
        m_inputManager->setUIWantsKeyboard(ImGui::GetIO().WantCaptureKeyboard);
        m_inputManager->setUIWantsMouse(ImGui::GetIO().WantCaptureMouse);
        
        // Process input
        processInput(deltaTime);
        
        // Update simulation
        update(deltaTime);
        
        // Render
        render();
        
        // End input frame
        m_inputManager->endFrame();
    }
    
    m_isRunning = false;
}

void App::processInput(float deltaTime) {
    // Handle toggle actions first
    if (m_inputManager->wasActionTriggered(InputAction::TogglePause)) {
        m_time->setPaused(!m_time->isPaused());
    }
    if (m_inputManager->wasActionTriggered(InputAction::ResetCamera)) {
        m_camera->transitionToTarget(glm::vec3(0.0f), 30.0f, 0.5f);
        m_lockedBody = nullptr;
        m_selectedBody = nullptr;
    }
    if (m_inputManager->wasActionTriggered(InputAction::Quit)) {
        m_isRunning = false;
    }
    if (m_inputManager->wasActionTriggered(InputAction::ToggleCameraMode)) {
        m_camera->toggleMode();
        if (m_camera->getMode() == Render::CameraMode::FreeFly) {
            m_lockedBody = nullptr;  // Unlock when switching to free-fly
        }
    }
    
    // Handle picking on click - using BodyPicker (SRP)
    if (!m_inputManager->uiWantsMouse() && m_inputManager->wasActionTriggered("left_click")) {
        float mx, my;
        m_inputManager->getMousePosition(mx, my);
        const Simulation::CelestialBody* clicked = m_bodyPicker->pickBody(
            mx, my, *m_solarSystem, *m_camera, 
            m_time->getSimulationTime(), m_simulationUI->isShowLabels()
        );
        
        float currentTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
        if (clicked && (currentTime - m_clickTime) < DOUBLE_CLICK_TIME && clicked == m_selectedBody) {
            // Double click - Focus
            handleBodySelection(clicked);
        } else {
            // Single click - Select
            m_selectedBody = clicked;
        }
        m_clickTime = currentTime;
    }
    
    // Keyboard movement
    float forward = 0.0f, right = 0.0f, up = 0.0f;
    
    if (m_inputManager->isActionActive(InputAction::MoveForward)) forward += 1.0f;
    if (m_inputManager->isActionActive(InputAction::MoveBackward)) forward -= 1.0f;
    if (m_inputManager->isActionActive(InputAction::MoveRight)) right += 1.0f;
    if (m_inputManager->isActionActive(InputAction::MoveLeft)) right -= 1.0f;
    if (m_inputManager->isActionActive(InputAction::MoveUp)) up += 1.0f;
    if (m_inputManager->isActionActive(InputAction::MoveDown)) up -= 1.0f;
    
    if (forward != 0.0f || right != 0.0f || up != 0.0f) {
        m_camera->handleKeyMovement(forward, right, up, deltaTime);
    }
    
    // Keyboard rotation (arrow keys) - for orbit mode
    float rotYaw = 0.0f, rotPitch = 0.0f;
    const float keyRotateSpeed = 120.0f;  // Degrees per second
    
    if (m_inputManager->isActionActive(InputAction::RotateLeft)) rotYaw -= keyRotateSpeed * deltaTime;
    if (m_inputManager->isActionActive(InputAction::RotateRight)) rotYaw += keyRotateSpeed * deltaTime;
    if (m_inputManager->isActionActive(InputAction::RotateUp)) rotPitch += keyRotateSpeed * deltaTime;
    if (m_inputManager->isActionActive(InputAction::RotateDown)) rotPitch -= keyRotateSpeed * deltaTime;
    
    if (rotYaw != 0.0f || rotPitch != 0.0f) {
        // Directly modify target angles for keyboard input (already scaled by deltaTime)
        m_camera->handleMouseDrag(rotYaw / m_camera->getMouseSensitivity(), 
                                  rotPitch / m_camera->getMouseSensitivity());
    }
    
    // Mouse drag rotation (right mouse button)
    if (m_inputManager->isMouseButtonDown(3)) {
        float dx, dy;
        m_inputManager->getMouseDelta(dx, dy);
        if (dx != 0.0f || dy != 0.0f) {
            m_camera->handleMouseDrag(dx, dy);
        }
    }
    
    // Mouse pan (middle mouse button)
    if (m_inputManager->isMouseButtonDown(2)) {
        float dx, dy;
        m_inputManager->getMouseDelta(dx, dy);
        if (dx != 0.0f || dy != 0.0f) {
            m_camera->handleMousePan(dx, dy);
            m_lockedBody = nullptr;  // Unlock when panning manually
        }
    }
    
    // Mouse wheel zoom
    float scroll = m_inputManager->getScrollDelta();
    if (scroll != 0.0f) {
        m_camera->handleScroll(scroll);
    }
}

void App::update(float deltaTime) {
    if (m_solarSystem->isPhysicsEnabled()) {
        // Run physics multiple steps per frame for stability
        const int steps = 4;
        float subDt = deltaTime / steps;
        for (int i = 0; i < steps; ++i) {
            m_solarSystem->updatePhysics(subDt);
        }
    } else {
        m_time->update(deltaTime);
    }
    
    m_camera->update(deltaTime);
    
    updateHover();
    
    // Update orbit target if locked to a body
    if (m_lockedBody && m_camera->getMode() == Render::CameraMode::Orbit) {
        glm::vec3 worldPos;
        if (m_solarSystem->isPhysicsEnabled()) {
            worldPos = m_lockedBody->getPhysicsPosition();
        } else {
            worldPos = m_lockedBody->getWorldPosition(m_time->getSimulationTime());
        }
        worldPos *= m_solarSystem->getSystemScale();
        
        // Update target smoothly
        glm::vec3 currentTarget = m_camera->getOrbitTarget();
        glm::vec3 newTarget = glm::mix(currentTarget, worldPos, 1.0f - std::exp(-10.0f * deltaTime));
        m_camera->setOrbitTarget(newTarget);
    }
}

void App::render() {
    // Sync render options from SimulationUI to GLRenderer
    m_renderer->setShowOrbits(m_simulationUI->isShowOrbits());
    m_renderer->setShowLabels(m_simulationUI->isShowLabels());
    
    m_renderer->render(*m_solarSystem, *m_camera, m_time->getSimulationTime(), m_hoveredBody, [this]() {
        // Set up callbacks for UI actions
        Render::UICallbacks callbacks;
        callbacks.onSystemSelected = [this](const std::string& name) {
            handleSystemChange(name);
        };
        callbacks.onBodySelected = [this](const Simulation::CelestialBody* body) {
            handleBodySelection(body);
        };
        callbacks.onClearFocus = [this]() {
            m_lockedBody = nullptr;
        };
        
        // Render main UI panels (SRP - delegated to SimulationUI)
        m_simulationUI->render(
            *m_solarSystem, *m_time, *m_camera,
            m_hoveredBody, m_selectedBody, m_lockedBody,
            callbacks
        );
        
        // Render planet labels
        m_simulationUI->renderLabels(
            *m_solarSystem, *m_camera, m_time->getSimulationTime(),
            m_hoveredBody, m_selectedBody, m_lockedBody,
            m_simulationUI->isShowLabels()
        );
        
        // Render body tooltip
        float mx, my;
        m_inputManager->getMousePosition(mx, my);
        const Simulation::CelestialBody* infoBody = m_hoveredBody ? m_hoveredBody : m_selectedBody;
        m_simulationUI->renderBodyTooltip(
            infoBody, m_hoveredBody, m_selectedBody,
            mx, my, m_inputManager->uiWantsMouse()
        );
    });
}

void App::updateHover() {
    if (m_inputManager->uiWantsMouse()) {
        m_hoveredBody = nullptr;
        return;
    }
    
    float mx, my;
    m_inputManager->getMousePosition(mx, my);
    m_hoveredBody = m_bodyPicker->pickBody(
        mx, my, *m_solarSystem, *m_camera,
        m_time->getSimulationTime(), m_simulationUI->isShowLabels()
    );
}

void App::handleBodySelection(const Simulation::CelestialBody* body) {
    if (!body) return;
    
    m_lockedBody = body;
    float bodyRadius = static_cast<float>(body->getRadius()) * m_solarSystem->getPlanetScale();
    float orbitDist = std::max(bodyRadius * 5.0f, 2.0f);
    
    glm::vec3 worldPos;
    if (m_solarSystem->isPhysicsEnabled()) {
        worldPos = body->getPhysicsPosition();
    } else {
        worldPos = body->getWorldPosition(m_time->getSimulationTime());
    }
    worldPos *= m_solarSystem->getSystemScale();
    
    m_camera->transitionToTarget(worldPos, orbitDist, 0.8f);
}

void App::handleSystemChange(const std::string& systemName) {
    m_solarSystem->loadSystem(systemName);
    m_lockedBody = nullptr;
    m_selectedBody = nullptr;
    m_hoveredBody = nullptr;
    
    // Use different zoom for special events
    float distance = (systemName == "Black Hole" || systemName == "Binary Star") ? 40.0f : 30.0f;
    m_camera->transitionToTarget(glm::vec3(0.0f), distance, 0.5f);
}

} // namespace Core

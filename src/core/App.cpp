#include "App.hpp"
#include "render/GLRenderer.hpp"
#include "Logger.hpp"
#include "imgui.h"
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
    
    // Get initial window size
    int width, height;
    m_window->getSize(width, height);
    
    // Create camera
    m_camera = std::make_unique<Render::Camera>(static_cast<float>(width), static_cast<float>(height));
    
    // Create renderer
    m_renderer = std::make_unique<Render::GLRenderer>(*m_window);
    
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
    m_renderer.reset();
    m_camera.reset();
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
    
    // Mouse drag rotation (right mouse button or middle button)
    if (m_inputManager->isMouseButtonDown(3) || m_inputManager->isMouseButtonDown(2)) {
        float dx, dy;
        m_inputManager->getMouseDelta(dx, dy);
        if (dx != 0.0f || dy != 0.0f) {
            m_camera->handleMouseDrag(dx, dy);
        }
    }
    
    // Mouse wheel zoom
    float scroll = m_inputManager->getScrollDelta();
    if (scroll != 0.0f) {
        m_camera->handleScroll(scroll);
    }
}

void App::update(float deltaTime) {
    m_time->update(deltaTime);
    m_camera->update(deltaTime);
    
    // Update orbit target if locked to a body
    if (m_lockedBody && m_camera->getMode() == Render::CameraMode::Orbit) {
        glm::vec3 worldPos = m_lockedBody->getWorldPosition(m_time->getSimulationTime());
        worldPos *= m_solarSystem->getSystemScale();
        
        // Update target smoothly
        glm::vec3 currentTarget = m_camera->getOrbitTarget();
        glm::vec3 newTarget = glm::mix(currentTarget, worldPos, 1.0f - std::exp(-10.0f * deltaTime));
        m_camera->setOrbitTarget(newTarget);
    }
}

void App::render() {
    m_renderer->render(*m_solarSystem, *m_camera, m_time->getSimulationTime(), [this]() {
        renderUI();
    });
}

void App::renderUI() {
    ImGui::Begin("Controls");
    
    // System selector
    const char* systems[] = { "Solar System", "TRAPPIST-1", "Kepler-90", "HR 8799", "Kepler-11", "55 Cancri" };
    static int currentSystemIdx = 0;
    if (ImGui::Combo("System", &currentSystemIdx, systems, IM_ARRAYSIZE(systems))) {
        m_solarSystem->loadSystem(systems[currentSystemIdx]);
        m_lockedBody = nullptr;
        m_camera->transitionToTarget(glm::vec3(0.0f), 30.0f, 0.5f);
    }
    
    // Time controls
    ImGui::Separator();
    ImGui::Text("Time");
    
    float timeScale = static_cast<float>(m_time->getTimeScale());
    if (ImGui::SliderFloat("Speed", &timeScale, 0.0f, 1.0f, "%.4f")) {
        m_time->setTimeScale(timeScale);
    }
    
    if (ImGui::Button(m_time->isPaused() ? "Resume (P)" : "Pause (P)")) {
        m_time->setPaused(!m_time->isPaused());
    }
    
    // Camera controls
    ImGui::Separator();
    ImGui::Text("Camera");
    
    const char* modeNames[] = { "Orbit", "Free Fly" };
    int currentMode = static_cast<int>(m_camera->getMode());
    if (ImGui::Combo("Mode (Tab)", &currentMode, modeNames, 2)) {
        m_camera->setMode(static_cast<Render::CameraMode>(currentMode));
        if (m_camera->getMode() == Render::CameraMode::FreeFly) {
            m_lockedBody = nullptr;
        }
    }
    
    float sensitivity = m_camera->getMouseSensitivity();
    if (ImGui::SliderFloat("Sensitivity", &sensitivity, 0.05f, 1.0f)) {
        m_camera->setMouseSensitivity(sensitivity);
    }
    
    bool invertY = m_camera->isInvertY();
    if (ImGui::Checkbox("Invert Y", &invertY)) {
        m_camera->setInvertY(invertY);
    }
    
    if (ImGui::Button("Reset Camera (O)")) {
        m_camera->transitionToTarget(glm::vec3(0.0f), 30.0f, 0.5f);
        m_lockedBody = nullptr;
    }
    
    // Focus target selector
    ImGui::Separator();
    ImGui::Text("Focus Target");
    
    std::string currentFocus = m_lockedBody ? m_lockedBody->getName() : "None";
    if (ImGui::BeginCombo("##Focus", currentFocus.c_str())) {
        // None option
        if (ImGui::Selectable("None", m_lockedBody == nullptr)) {
            m_lockedBody = nullptr;
        }
        
        // Sun/Star option
        const auto* sun = m_solarSystem->getSun();
        if (sun && ImGui::Selectable(sun->getName().c_str(), m_lockedBody == sun)) {
            m_lockedBody = sun;
            m_camera->transitionToTarget(glm::vec3(0.0f), 30.0f, 0.5f);
        }
        
        float systemScale = m_solarSystem->getSystemScale();
        float planetScale = m_solarSystem->getPlanetScale();
        
        // Other bodies
        auto& bodies = m_solarSystem->getBodies();
        for (size_t i = 1; i < bodies.size(); ++i) {  // Skip sun at index 0
            const auto& body = bodies[i];
            
            // Calculate appropriate orbit distance based on body size
            float bodyRadius = static_cast<float>(body->getRadius()) * planetScale;
            float orbitDist = std::max(bodyRadius * 4.0f, 3.0f);
            
            bool isSelected = (m_lockedBody == body.get());
            if (ImGui::Selectable(body->getName().c_str(), isSelected)) {
                m_lockedBody = body.get();
                glm::vec3 worldPos = body->getWorldPosition(m_time->getSimulationTime()) * systemScale;
                m_camera->transitionToTarget(worldPos, orbitDist, 0.5f);
            }
            
            // Children (moons) - indented
            for (const auto& child : body->getChildren()) {
                std::string label = "  " + child->getName();
                float childRadius = static_cast<float>(child->getRadius()) * planetScale;
                float childOrbitDist = std::max(childRadius * 5.0f, 1.5f);
                
                isSelected = (m_lockedBody == child.get());
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    m_lockedBody = child.get();
                    glm::vec3 worldPos = child->getWorldPosition(m_time->getSimulationTime()) * systemScale;
                    m_camera->transitionToTarget(worldPos, childOrbitDist, 0.5f);
                }
            }
        }
        ImGui::EndCombo();
    }
    
    // Info
    ImGui::Separator();
    ImGui::Text("Simulation: %.2f years", m_time->getSimulationTime());
    ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
    
    // Help
    if (ImGui::CollapsingHeader("Controls Help")) {
        ImGui::BulletText("Right-click + drag: Rotate camera");
        ImGui::BulletText("Scroll wheel: Zoom in/out");
        ImGui::BulletText("Arrow keys: Rotate camera");
        ImGui::BulletText("W/S: Zoom in/out (orbit) or move (free)");
        ImGui::BulletText("A/D: Strafe (free fly only)");
        ImGui::BulletText("Q/E: Move down/up (free fly only)");
        ImGui::BulletText("Tab: Toggle camera mode");
        ImGui::BulletText("P: Pause/Resume");
        ImGui::BulletText("O: Reset camera");
        ImGui::BulletText("Esc: Quit");
    }
    
    ImGui::End();
}

} // namespace Core

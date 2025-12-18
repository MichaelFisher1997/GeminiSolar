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
    const float moveSpeed = 10.0f * deltaTime;
    const float rotateSpeed = 90.0f * deltaTime;
    
    // Movement
    if (m_inputManager->isActionActive(InputAction::MoveForward)) {
        m_camera->moveForward(moveSpeed);
    }
    if (m_inputManager->isActionActive(InputAction::MoveBackward)) {
        m_camera->moveForward(-moveSpeed);
    }
    if (m_inputManager->isActionActive(InputAction::MoveLeft)) {
        m_camera->moveRight(-moveSpeed);
    }
    if (m_inputManager->isActionActive(InputAction::MoveRight)) {
        m_camera->moveRight(moveSpeed);
    }
    if (m_inputManager->isActionActive(InputAction::MoveUp)) {
        m_camera->moveUp(moveSpeed);
    }
    if (m_inputManager->isActionActive(InputAction::MoveDown)) {
        m_camera->moveUp(-moveSpeed);
    }
    
    // Rotation (keyboard)
    if (m_inputManager->isActionActive(InputAction::RotateLeft)) {
        m_camera->rotate(-rotateSpeed, 0.0f);
    }
    if (m_inputManager->isActionActive(InputAction::RotateRight)) {
        m_camera->rotate(rotateSpeed, 0.0f);
    }
    if (m_inputManager->isActionActive(InputAction::RotateUp)) {
        m_camera->rotate(0.0f, rotateSpeed);
    }
    if (m_inputManager->isActionActive(InputAction::RotateDown)) {
        m_camera->rotate(0.0f, -rotateSpeed);
    }
    
    // Toggle actions
    if (m_inputManager->wasActionTriggered(InputAction::TogglePause)) {
        m_time->setPaused(!m_time->isPaused());
    }
    if (m_inputManager->wasActionTriggered(InputAction::ResetCamera)) {
        m_camera->setOrbitTarget(glm::vec3(0.0f), 20.0f);
    }
    if (m_inputManager->wasActionTriggered(InputAction::Quit)) {
        m_isRunning = false;
    }
    
    // Mouse look (right button)
    if (m_inputManager->isMouseButtonDown(3)) {  // SDL_BUTTON_RIGHT = 3
        float dx, dy;
        m_inputManager->getMouseDelta(dx, dy);
        m_camera->rotate(dx * 0.2f, dy * 0.2f);
    }
    
    // Mouse wheel zoom
    float scroll = m_inputManager->getScrollDelta();
    if (scroll != 0.0f) {
        m_camera->zoom(scroll * 2.0f);
    }
}

void App::update(float deltaTime) {
    m_time->update(deltaTime);
    m_camera->update(deltaTime);
    
    // Update orbit target if locked to a body
    if (m_lockedBody) {
        glm::vec3 worldPos = m_lockedBody->getWorldPosition(m_time->getSimulationTime());
        worldPos *= m_solarSystem->getSystemScale();
        m_camera->updateOrbitTarget(worldPos);
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
        m_camera->setOrbitTarget(glm::vec3(0.0f), 20.0f);
    }
    
    // Time scale
    float timeScale = static_cast<float>(m_time->getTimeScale());
    if (ImGui::SliderFloat("Time Scale", &timeScale, 0.0f, 1.0f, "%.4f")) {
        m_time->setTimeScale(timeScale);
    }
    
    // Pause button
    if (ImGui::Button(m_time->isPaused() ? "Resume" : "Pause")) {
        m_time->setPaused(!m_time->isPaused());
    }
    
    // Focus target selector
    if (ImGui::BeginCombo("Focus", m_lockedBody ? m_lockedBody->getName().c_str() : "Select Body")) {
        // Sun option
        if (ImGui::Selectable("Sun", m_lockedBody == m_solarSystem->getSun())) {
            m_camera->setOrbitTarget(glm::vec3(0.0f), 20.0f);
            m_lockedBody = m_solarSystem->getSun();
        }
        
        float systemScale = m_solarSystem->getSystemScale();
        
        // Other bodies
        auto& bodies = m_solarSystem->getBodies();
        for (const auto& body : bodies) {
            if (body->getName() == "Sun") continue;
            
            // Root bodies (planets)
            bool isSelected = (m_lockedBody == body.get());
            if (ImGui::Selectable(body->getName().c_str(), isSelected)) {
                glm::vec3 worldPos = body->getWorldPosition(m_time->getSimulationTime()) * systemScale;
                m_camera->setOrbitTarget(worldPos, 5.0f);
                m_lockedBody = body.get();
            }
            
            // Children (moons)
            for (const auto& child : body->getChildren()) {
                std::string label = "  " + child->getName();
                isSelected = (m_lockedBody == child.get());
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    glm::vec3 worldPos = child->getWorldPosition(m_time->getSimulationTime()) * systemScale;
                    m_camera->setOrbitTarget(worldPos, 2.0f);
                    m_lockedBody = child.get();
                }
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::Separator();
    ImGui::Text("Simulation Time: %.2f years", m_time->getSimulationTime());
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    
    ImGui::End();
}

} // namespace Core

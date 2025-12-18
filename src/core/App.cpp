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
    
    // Get initial window size
    int width, height;
    m_window->getSize(width, height);
    
    // Create camera
    m_camera = std::make_unique<Render::Camera>(static_cast<float>(width), static_cast<float>(height));
    
    // Create renderer
    m_renderer = std::make_unique<Render::GLRenderer>(*m_window);
    
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
    
    // Handle picking on click
    if (!m_inputManager->uiWantsMouse() && m_inputManager->wasActionTriggered("left_click")) {
        float mx, my;
        m_inputManager->getMousePosition(mx, my);
        const Simulation::CelestialBody* clicked = pickBody(mx, my);
        
        float currentTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
        if (clicked && (currentTime - m_clickTime) < DOUBLE_CLICK_TIME && clicked == m_selectedBody) {
            // Double click - Focus
            m_lockedBody = clicked;
            float bodyRadius = static_cast<float>(clicked->getRadius()) * m_solarSystem->getPlanetScale();
            float orbitDist = std::max(bodyRadius * 5.0f, 2.0f);
            glm::vec3 worldPos = clicked->getWorldPosition(m_time->getSimulationTime()) * m_solarSystem->getSystemScale();
            m_camera->transitionToTarget(worldPos, orbitDist, 0.8f);
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
    m_time->update(deltaTime);
    m_camera->update(deltaTime);
    
    updateHover();
    
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
    m_renderer->render(*m_solarSystem, *m_camera, m_time->getSimulationTime(), m_hoveredBody, [this]() {
        renderUI();
    });
}

void App::updateHover() {
    if (m_inputManager->uiWantsMouse()) {
        m_hoveredBody = nullptr;
        return;
    }
    
    float mx, my;
    m_inputManager->getMousePosition(mx, my);
    m_hoveredBody = pickBody(mx, my);
}

const Simulation::CelestialBody* App::pickBody(float mouseX, float mouseY) {
    int w, h;
    m_window->getSize(w, h);
    glm::vec3 rayDir = m_camera->getRayDirection(mouseX, mouseY, static_cast<float>(w), static_cast<float>(h));
    glm::vec3 rayOrigin = m_camera->getPosition();
    
    const Simulation::CelestialBody* bestBody = nullptr;
    float minDist = 1e10f;
    
    float sysScale = m_solarSystem->getSystemScale();
    float planetScale = m_solarSystem->getPlanetScale();
    double time = m_time->getSimulationTime();
    
    std::function<void(const Simulation::CelestialBody&, glm::vec3)> checkBody;
    checkBody = [&](const Simulation::CelestialBody& body, glm::vec3 parentPos) {
        glm::vec3 pos = parentPos + body.getPosition(time) * sysScale;
        float radius = static_cast<float>(body.getRadius());
        
        if (body.getName() == "Sun") {
            radius = 1.5f; // Visual sun size is hardcoded in renderer
        } else {
            radius *= planetScale;
        }
        
        // Ray-sphere intersection
        glm::vec3 L = pos - rayOrigin;
        float tca = glm::dot(L, rayDir);
        if (tca >= 0) {
            float d2 = glm::dot(L, L) - tca * tca;
            if (d2 <= radius * radius) {
                float t0 = tca - sqrt(radius * radius - d2);
                if (t0 < minDist) {
                    minDist = t0;
                    bestBody = &body;
                }
            }
        }
        
        for (const auto& child : body.getChildren()) {
            checkBody(*child, pos);
        }
    };
    
    for (const auto& body : m_solarSystem->getBodies()) {
        checkBody(*body, glm::vec3(0.0f));
    }
    
    return bestBody;
}

void App::renderUI() {
    // Labels Toggle
    if (ImGui::Begin("Visual Settings", nullptr)) {
        bool showOrbits = m_renderer->isShowOrbits();
        if (ImGui::Checkbox("Show Orbital Rings", &showOrbits)) {
            m_renderer->setShowOrbits(showOrbits);
        }
        
        bool showLabels = m_renderer->isShowLabels();
        if (ImGui::Checkbox("Show Planet Labels", &showLabels)) {
            m_renderer->setShowLabels(showLabels);
        }
    }
    ImGui::End();

    // 1. Top bar / Stats Overlay
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove)) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Solar System Simulator");
        ImGui::Separator();
        ImGui::Text("Time: %.2f years", m_time->getSimulationTime());
        ImGui::Text("FPS:  %.0f", ImGui::GetIO().Framerate);
        if (m_time->isPaused()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "[PAUSED]");
        }
    }
    ImGui::End();

    // Planet Labels
    if (m_renderer->isShowLabels()) {
        int w, h;
        m_window->getSize(w, h);
        float sw = static_cast<float>(w);
        float sh = static_cast<float>(h);
        
        glm::mat4 viewProj = m_camera->getProjectionMatrix() * m_camera->getViewMatrix();
        float sysScale = m_solarSystem->getSystemScale();
        double time = m_time->getSimulationTime();
        
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        
        std::function<void(const Simulation::CelestialBody&, glm::vec3)> drawLabel;
        drawLabel = [&](const Simulation::CelestialBody& body, glm::vec3 parentPos) {
            glm::vec3 worldPos = parentPos + body.getPosition(time) * sysScale;
            glm::vec4 clipPos = viewProj * glm::vec4(worldPos, 1.0f);
            
            if (clipPos.z > 0 && clipPos.w > 0) {
                glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
                float sx = (ndc.x + 1.0f) * 0.5f * sw;
                float sy = (1.0f - ndc.y) * 0.5f * sh;
                
                ImU32 col = (m_hoveredBody == &body || m_selectedBody == &body) ? IM_COL32(255, 255, 255, 255) : IM_COL32(200, 200, 200, 150);
                drawList->AddText(ImVec2(sx + 10, sy - 10), col, body.getName().c_str());
            }
            
            for (const auto& child : body.getChildren()) {
                drawLabel(*child, worldPos);
            }
        };
        
        for (const auto& body : m_solarSystem->getBodies()) {
            drawLabel(*body, glm::vec3(0.0f));
        }
    }

    // Info Pop-up (on hover or selection)
    const Simulation::CelestialBody* infoBody = m_hoveredBody ? m_hoveredBody : m_selectedBody;
    if (infoBody) {
        ImGui::SetNextWindowBgAlpha(0.85f);
        if (m_hoveredBody) {
            float mx, my;
            m_inputManager->getMousePosition(mx, my);
            ImGui::SetNextWindowPos(ImVec2(mx + 20, my + 20), ImGuiCond_Always);
        } else {
            ImGui::SetNextWindowPos(ImVec2(10, 100), ImGuiCond_FirstUseEver);
        }
        
        ImGui::Begin("Body Info", nullptr, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::TextColored(ImVec4(0.4, 0.8, 1.0, 1.0), "%s", infoBody->getName().c_str());
        ImGui::Separator();
        
        if (infoBody->getName() == "Sun" || infoBody->getName() == "Kepler-90" || infoBody->getName() == "TRAPPIST-1") {
            ImGui::Text("Type: Star");
            ImGui::Text("Radius: %.1f km (Relative)", infoBody->getRadius());
        } else {
            ImGui::Text("Type: Planet/Moon");
            const auto& op = infoBody->getOrbitalParams();
            ImGui::Text("Distance: %.3f AU", op.semiMajorAxis);
            ImGui::Text("Period: %.3f Years", op.orbitalPeriod);
            ImGui::Text("Eccentricity: %.3f", op.eccentricity);
        }
        
        if (m_hoveredBody && m_hoveredBody != m_selectedBody) {
            ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "(Click for more info)");
        }
        if (infoBody == m_selectedBody) {
             ImGui::TextColored(ImVec4(0.7, 0.7, 1.0, 1.0), "(Double-click to focus)");
        }
        
        ImGui::End();
    }

    // 2. Main Control Panel
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 310, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
    if (ImGui::Begin("Simulation Controls", nullptr)) {
        if (ImGui::CollapsingHeader("System Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char* systems[] = { "Solar System", "TRAPPIST-1", "Kepler-90", "HR 8799", "Kepler-11", "55 Cancri" };
            static int currentSystemIdx = 0;
            if (ImGui::Combo("System", &currentSystemIdx, systems, IM_ARRAYSIZE(systems))) {
                m_solarSystem->loadSystem(systems[currentSystemIdx]);
                m_lockedBody = nullptr;
                m_camera->transitionToTarget(glm::vec3(0.0f), 30.0f, 0.5f);
            }
        }

        if (ImGui::CollapsingHeader("Time Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Internal scale where 1.0 = 1 day per real second
            const double DAY_IN_YEARS = 1.0 / 365.25;
            const double SECOND_IN_YEARS = 1.0 / (365.25 * 86400.0);
            
            float multiplier = static_cast<float>(m_time->getTimeScale() / DAY_IN_YEARS);
            
            // Logarithmic-like slider for better sensitivity at low speeds
            if (ImGui::SliderFloat("Speed (Days/s)", &multiplier, 0.0f, 400.0f, "%.4f", ImGuiSliderFlags_Logarithmic)) {
                m_time->setTimeScale(static_cast<double>(multiplier) * DAY_IN_YEARS);
            }
            
            ImGui::Text("Presets:");
            if (ImGui::Button("IRL (Real-time)")) m_time->setTimeScale(SECOND_IN_YEARS);
            ImGui::SameLine();
            if (ImGui::Button("1x")) m_time->setTimeScale(1.0 * DAY_IN_YEARS);
            ImGui::SameLine();
            if (ImGui::Button("2x")) m_time->setTimeScale(2.0 * DAY_IN_YEARS);
            ImGui::SameLine();
            if (ImGui::Button("4x")) m_time->setTimeScale(4.0 * DAY_IN_YEARS);
            ImGui::SameLine();
            if (ImGui::Button("100x")) m_time->setTimeScale(100.0 * DAY_IN_YEARS);
            
            if (ImGui::Button(m_time->isPaused() ? "Resume (P)" : "Pause (P)", ImVec2(-1, 0))) {
                m_time->setPaused(!m_time->isPaused());
            }
        }

        if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
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
            
            if (ImGui::Button("Reset View (O)", ImVec2(-1, 0))) {
                m_camera->transitionToTarget(glm::vec3(0.0f), 30.0f, 0.8f);
                m_lockedBody = nullptr;
            }
        }

        if (ImGui::CollapsingHeader("Visual Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            bool showOrbits = m_renderer->isShowOrbits();
            if (ImGui::Checkbox("Show Orbital Rings", &showOrbits)) {
                m_renderer->setShowOrbits(showOrbits);
            }
        }
    }
    ImGui::End();

    // 3. Navigation / Focus Panel
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 310, ImGui::GetIO().DisplaySize.y - 260), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always);
    if (ImGui::Begin("Celestial Bodies", nullptr)) {
        if (ImGui::Button("Clear Focus", ImVec2(-1, 0))) {
            m_lockedBody = nullptr;
        }
        ImGui::Separator();

        ImGui::BeginChild("BodyList");
        
        // Sun
        const auto* sun = m_solarSystem->getSun();
        if (sun) {
            bool isSelected = (m_lockedBody == sun);
            if (ImGui::Selectable(sun->getName().c_str(), isSelected)) {
                m_lockedBody = sun;
                m_camera->transitionToTarget(glm::vec3(0.0f), 25.0f, 0.8f);
            }
        }

        float systemScale = m_solarSystem->getSystemScale();
        float planetScale = m_solarSystem->getPlanetScale();
        
        // Planets
        auto& bodies = m_solarSystem->getBodies();
        for (size_t i = 1; i < bodies.size(); ++i) {
            const auto& body = bodies[i];
            float bodyRadius = static_cast<float>(body->getRadius()) * planetScale;
            float orbitDist = std::max(bodyRadius * 5.0f, 2.0f);
            
            bool isSelected = (m_lockedBody == body.get());
            if (ImGui::Selectable(body->getName().c_str(), isSelected)) {
                m_lockedBody = body.get();
                glm::vec3 worldPos = body->getWorldPosition(m_time->getSimulationTime()) * systemScale;
                m_camera->transitionToTarget(worldPos, orbitDist, 0.8f);
            }
            
            // Moons
            for (const auto& child : body->getChildren()) {
                std::string label = "  > " + child->getName();
                float childRadius = static_cast<float>(child->getRadius()) * planetScale;
                float childOrbitDist = std::max(childRadius * 6.0f, 1.2f);
                
                isSelected = (m_lockedBody == child.get());
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    m_lockedBody = child.get();
                    glm::vec3 worldPos = child->getWorldPosition(m_time->getSimulationTime()) * systemScale;
                    m_camera->transitionToTarget(worldPos, childOrbitDist, 0.8f);
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();

    // 4. Help Overlay
    static bool showHelp = false;
    ImGui::SetNextWindowPos(ImVec2(10, ImGui::GetIO().DisplaySize.y - 40), ImGuiCond_Always);
    if (ImGui::Begin("HelpToggle", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground)) {
        if (ImGui::Button("? Controls Help")) {
            showHelp = !showHelp;
        }
    }
    ImGui::End();

    if (showHelp) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Controls Help", &showHelp, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Mouse Controls:");
            ImGui::BulletText("Right-Click + Drag: Rotate Camera");
            ImGui::BulletText("Scroll Wheel: Zoom In/Out");
            ImGui::BulletText("Middle-Click + Drag: Move Orbit Target");
            
            ImGui::Separator();
            ImGui::Text("Keyboard Controls:");
            ImGui::BulletText("Tab: Toggle Camera Mode (Orbit / Free Fly)");
            ImGui::BulletText("W / S: Zoom / Move Forward & Backward");
            ImGui::BulletText("A / D: Strafe Left & Right (Free Fly)");
            ImGui::BulletText("Q / E: Move Up & Down (Free Fly)");
            ImGui::BulletText("P: Toggle Pause Simulation");
            ImGui::BulletText("O: Reset Camera View");
            ImGui::BulletText("Esc: Exit Application");
            
            if (ImGui::Button("Close", ImVec2(-1, 0))) {
                showHelp = false;
            }
        }
        ImGui::End();
    }
}

} // namespace Core

#include "App.hpp"
#include "render/GLRenderer.hpp"
#include <iostream>
#include <chrono>
#include <cmath>

namespace Core {

App::App() {
    try {
        // Request OpenGL context in SDL
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        m_window = std::make_unique<Platform::SDLWindow>("Solar System Simulator (OpenGL)", 1280, 720);
        
        // Skip Vulkan Init
        // m_vulkanContext = std::make_unique<Render::VulkanContext>();
        // m_vulkanContext->init(*m_window);
        
        int width, height;
        SDL_GetWindowSize(m_window->getHandle(), &width, &height);
        m_camera = std::make_unique<Render::Camera>(static_cast<float>(width), static_cast<float>(height));
        
        // Use OpenGL Renderer
        m_renderer = std::make_unique<Render::GLRenderer>(*m_window);

        m_time = std::make_unique<Time>();
        // 1 real second = 1 simulated day (approx)
        m_time->setTimeScale(1.0 / 365.25); 

        m_solarSystem = std::make_unique<Simulation::SolarSystem>();
        m_solarSystem->init();
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize app: " << e.what() << std::endl;
        exit(1);
    }
}

App::~App() {
    // Cleanup handled by unique_ptr
    m_renderer.reset();
    // m_vulkanContext.reset();
}

void App::run() {
    m_isRunning = true;
    
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (m_isRunning && !m_window->shouldClose()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Poll events properly
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                m_isRunning = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    m_isRunning = false;
                }
                // Toggles
                if (event.key.key == SDLK_P) {
                    m_time->setPaused(!m_time->isPaused());
                }
                if (event.key.key == SDLK_O) {
                    m_camera->setOrbitTarget(glm::vec3(0.0f), 20.0f);
                }
            }
            // Mouse Wheel
            if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                m_camera->zoom(event.wheel.y * 2.0f);
            }
        }
        
        update(deltaTime);
        render();
    }
}

void App::update(double deltaTime) {
    m_time->update(deltaTime);
    m_camera->update(deltaTime);
    
    // Basic Camera Controls (Polling for now, should use event loop in run() but this is cleaner for v1)
    const float moveSpeed = 10.0f * deltaTime;
    const float rotateSpeed = 90.0f * deltaTime;
    
    const bool* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_W]) m_camera->moveForward(moveSpeed);
    if (state[SDL_SCANCODE_S]) m_camera->moveForward(-moveSpeed);
    if (state[SDL_SCANCODE_A]) m_camera->moveRight(-moveSpeed);
    if (state[SDL_SCANCODE_D]) m_camera->moveRight(moveSpeed);
    if (state[SDL_SCANCODE_Q]) m_camera->moveUp(-moveSpeed);
    if (state[SDL_SCANCODE_E]) m_camera->moveUp(moveSpeed);
    
    // Mouse look
    float mouseX, mouseY;
    Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
    
    static float lastMouseX = 0.0f;
    static float lastMouseY = 0.0f;
    static bool firstMouse = true;
    
    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }
    
    float xoffset = mouseX - lastMouseX;
    float yoffset = mouseY - lastMouseY; // Y is typically inverted in screen coords
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    if (buttons & SDL_BUTTON_RMASK) {
         m_camera->rotate(xoffset * 0.2f, yoffset * 0.2f); // Sensitivity 0.2
    }
    
    // Time scale
    if (state[SDL_SCANCODE_EQUALS] || state[SDL_SCANCODE_KP_PLUS]) m_time->setTimeScale(m_time->getTimeScale() * 1.1);
    if (state[SDL_SCANCODE_MINUS] || state[SDL_SCANCODE_KP_MINUS]) m_time->setTimeScale(m_time->getTimeScale() * 0.9);
    
    // Keys for rotation (Legacy)
    if (state[SDL_SCANCODE_LEFT]) m_camera->rotate(-rotateSpeed, 0.0f);
    if (state[SDL_SCANCODE_RIGHT]) m_camera->rotate(rotateSpeed, 0.0f);
    if (state[SDL_SCANCODE_UP]) m_camera->rotate(0.0f, rotateSpeed);
    if (state[SDL_SCANCODE_DOWN]) m_camera->rotate(0.0f, -rotateSpeed);

    // Print positions occasionally for debugging
    static double printTimer = 0.0;
    printTimer += deltaTime;
    if (printTimer > 5.0) {
        printTimer = 0.0;
        double simTime = m_time->getSimulationTime();
        std::cout << "Sim Time (Years): " << simTime << std::endl;
        
        if (m_solarSystem) {
            auto& bodies = m_solarSystem->getBodies();
            for (const auto& body : bodies) {
                if (body->getName() == "Earth") {
                     glm::vec3 pos = body->getPosition(simTime);
                     std::cout << "Earth Pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
                }
            }
        }
    }
}

void App::render() {
    if (m_renderer && m_solarSystem && m_camera) {
        m_renderer->render(*m_solarSystem, *m_camera, m_time->getSimulationTime());
    }
}

} // namespace Core

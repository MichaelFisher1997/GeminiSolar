#include "SDLWindow.hpp"
#include <SDL3/SDL_vulkan.h>
#include <stdexcept>
#include <iostream>
#include <vulkan/vulkan.h>

namespace Platform {

SDLWindow::SDLWindow(const std::string& title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
    }

    // Set window flags (OpenGL by default now, was Vulkan)
    SDL_WindowFlags window_flags = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    m_window = SDL_CreateWindow(
        title.c_str(),
        width,
        height,
        window_flags
    );

    if (!m_window) {
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }
}

SDLWindow::~SDLWindow() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
    SDL_Quit();
}

void SDLWindow::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_shouldClose = true;
        }
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_ESCAPE) {
                m_shouldClose = true;
            }
        }
    }
}

std::vector<const char*> SDLWindow::getVulkanInstanceExtensions() const {
    Uint32 count = 0;
    const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&count);
    if (!extensions) {
        throw std::runtime_error("Failed to get SDL Vulkan extensions: " + std::string(SDL_GetError()));
    }
    
    std::vector<const char*> extVector;
    for (Uint32 i = 0; i < count; i++) {
        extVector.push_back(extensions[i]);
    }
    return extVector;
}

bool SDLWindow::createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) const {
    return SDL_Vulkan_CreateSurface(m_window, instance, nullptr, surface);
}

} // namespace Platform

#pragma once

#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Platform {

class SDLWindow {
public:
    SDLWindow(const std::string& title, int width, int height);
    ~SDLWindow();

    // Delete copy constructors to prevent double-freeing resources
    SDLWindow(const SDLWindow&) = delete;
    SDLWindow& operator=(const SDLWindow&) = delete;

    bool shouldClose() const { return m_shouldClose; }
    void pollEvents();
    
    SDL_Window* getHandle() const { return m_window; }
    
    // Get Vulkan instance extensions required by SDL
    std::vector<const char*> getVulkanInstanceExtensions() const;
    
    // Create a Vulkan surface for this window
    bool createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) const;

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;
};

} // namespace Platform

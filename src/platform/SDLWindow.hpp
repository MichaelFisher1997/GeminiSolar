#pragma once

#include "WindowInterface.hpp"
#include <SDL3/SDL.h>
#include <string>

namespace Platform {

/// SDL3-based window implementation with OpenGL support
class SDLWindow : public WindowInterface {
public:
    SDLWindow(const std::string& title, int width, int height);
    ~SDLWindow() override;

    // Delete copy constructors to prevent double-freeing resources
    SDLWindow(const SDLWindow&) = delete;
    SDLWindow& operator=(const SDLWindow&) = delete;

    // WindowInterface implementation
    bool shouldClose() const override { return m_shouldClose; }
    void getSize(int& width, int& height) const override;
    void* getNativeHandle() const override { return m_window; }
    void setTitle(const std::string& title) override;
    void swapBuffers() override;
    void makeContextCurrent() override;
    void pollEvents(EventCallback callback) override;
    
    /// Get the OpenGL context
    SDL_GLContext getGLContext() const { return m_glContext; }
    
    /// Create OpenGL context (call after construction)
    void createGLContext();

private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;
    bool m_shouldClose = false;
};

} // namespace Platform

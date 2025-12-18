#pragma once

#include "platform/WindowInterface.hpp"
#include <functional>
#include <SDL3/SDL.h>

struct SDL_Window;

namespace Render {

/// Manages ImGui initialization and rendering
class UIManager {
public:
    UIManager(Platform::WindowInterface& window, void* glContext);
    ~UIManager();
    
    // Non-copyable
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    
    /// Start a new frame
    void beginFrame();
    
    /// End the frame and render
    void endFrame();
    
    /// Check if UI wants keyboard input
    bool wantsKeyboard() const;
    
    /// Check if UI wants mouse input
    bool wantsMouse() const;
    
    /// Process SDL event for ImGui
    void processEvent(const SDL_Event& event);

private:
    Platform::WindowInterface& m_window;
};

} // namespace Render

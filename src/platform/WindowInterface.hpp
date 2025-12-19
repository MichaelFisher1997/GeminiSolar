#pragma once

#include <string>
#include <functional>

namespace Platform {

/// Event types for window callbacks
enum class WindowEventType {
    Quit,
    Resized,
    KeyDown,
    KeyUp,
    MouseButtonDown,
    MouseButtonUp,
    MouseMotion,
    MouseWheel
};

/// Window event data
struct WindowEvent {
    WindowEventType type;
    
    // Key data
    int keyCode = 0;
    int scanCode = 0;
    
    // Mouse data
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    float wheelDelta = 0.0f;
    int mouseButton = 0;
    
    // Window data
    int width = 0;
    int height = 0;
};

/// Abstract window interface - allows for different windowing backends
class WindowInterface {
public:
    virtual ~WindowInterface() = default;
    
    /// Check if window should close
    virtual bool shouldClose() const = 0;
    
    /// Get window dimensions
    virtual void getSize(int& width, int& height) const = 0;
    
    /// Get native window handle (platform-specific)
    virtual void* getNativeHandle() const = 0;
    
    /// Set window title
    virtual void setTitle(const std::string& title) = 0;
    
    /// Swap buffers (for double buffering)
    virtual void swapBuffers() = 0;
    
    /// Make OpenGL context current (if applicable)
    virtual void makeContextCurrent() = 0;
    
    /// Process window events and return them via callback
    using EventCallback = std::function<void(const WindowEvent&)>;
    virtual void pollEvents(EventCallback callback) = 0;
};

} // namespace Platform

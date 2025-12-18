#pragma once

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>
#include <vector>
#include "platform/WindowInterface.hpp"

namespace Core {

/// Input action names for decoupled input handling
namespace InputAction {
    constexpr const char* MoveForward = "move_forward";
    constexpr const char* MoveBackward = "move_backward";
    constexpr const char* MoveLeft = "move_left";
    constexpr const char* MoveRight = "move_right";
    constexpr const char* MoveUp = "move_up";
    constexpr const char* MoveDown = "move_down";
    constexpr const char* RotateLeft = "rotate_left";
    constexpr const char* RotateRight = "rotate_right";
    constexpr const char* RotateUp = "rotate_up";
    constexpr const char* RotateDown = "rotate_down";
    constexpr const char* TogglePause = "toggle_pause";
    constexpr const char* ResetCamera = "reset_camera";
    constexpr const char* Quit = "quit";
}

/// Manages input state and action bindings
class InputManager {
public:
    InputManager();
    
    /// Process a window event
    void processEvent(const Platform::WindowEvent& event);
    
    /// Check if an action is currently active (key held down)
    bool isActionActive(const std::string& action) const;
    
    /// Check if an action was just triggered this frame
    bool wasActionTriggered(const std::string& action) const;
    
    /// Get mouse position
    void getMousePosition(float& x, float& y) const;
    
    /// Get mouse delta since last frame
    void getMouseDelta(float& dx, float& dy) const;
    
    /// Check if mouse button is down
    bool isMouseButtonDown(int button) const;
    
    /// Get scroll wheel delta
    float getScrollDelta() const;
    
    /// Bind a scancode to an action
    void bindKey(int scanCode, const std::string& action);
    
    /// Clear triggered actions (call at end of frame)
    void endFrame();
    
    /// Check if UI wants keyboard focus (for ImGui integration)
    void setUIWantsKeyboard(bool wants) { m_uiWantsKeyboard = wants; }
    void setUIWantsMouse(bool wants) { m_uiWantsMouse = wants; }
    bool uiWantsKeyboard() const { return m_uiWantsKeyboard; }
    bool uiWantsMouse() const { return m_uiWantsMouse; }

private:
    void setupDefaultBindings();
    
    // Key bindings: scancode -> action name
    std::unordered_map<int, std::string> m_keyBindings;
    
    // Currently active actions (keys held down)
    std::unordered_set<std::string> m_activeActions;
    
    // Actions triggered this frame (key just pressed)
    std::unordered_set<std::string> m_triggeredActions;
    
    // Mouse state
    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
    float m_lastMouseX = 0.0f;
    float m_lastMouseY = 0.0f;
    float m_mouseDeltaX = 0.0f;
    float m_mouseDeltaY = 0.0f;
    float m_scrollDelta = 0.0f;
    std::unordered_set<int> m_mouseButtonsDown;
    
    bool m_uiWantsKeyboard = false;
    bool m_uiWantsMouse = false;
    bool m_firstMouseMove = true;
};

} // namespace Core

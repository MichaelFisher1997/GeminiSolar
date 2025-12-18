#include "InputManager.hpp"
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_keycode.h>

namespace Core {

InputManager::InputManager() {
    setupDefaultBindings();
}

void InputManager::setupDefaultBindings() {
    // Movement
    bindKey(SDL_SCANCODE_W, InputAction::MoveForward);
    bindKey(SDL_SCANCODE_S, InputAction::MoveBackward);
    bindKey(SDL_SCANCODE_A, InputAction::MoveLeft);
    bindKey(SDL_SCANCODE_D, InputAction::MoveRight);
    bindKey(SDL_SCANCODE_E, InputAction::MoveUp);
    bindKey(SDL_SCANCODE_Q, InputAction::MoveDown);
    
    // Rotation (arrow keys)
    bindKey(SDL_SCANCODE_LEFT, InputAction::RotateLeft);
    bindKey(SDL_SCANCODE_RIGHT, InputAction::RotateRight);
    bindKey(SDL_SCANCODE_UP, InputAction::RotateUp);
    bindKey(SDL_SCANCODE_DOWN, InputAction::RotateDown);
    
    // Actions
    bindKey(SDL_SCANCODE_P, InputAction::TogglePause);
    bindKey(SDL_SCANCODE_TAB, InputAction::ToggleCameraMode);
    bindKey(SDL_SCANCODE_O, InputAction::ResetCamera);
    bindKey(SDL_SCANCODE_ESCAPE, InputAction::Quit);
}

void InputManager::processEvent(const Platform::WindowEvent& event) {
    switch (event.type) {
        case Platform::WindowEventType::KeyDown: {
            if (m_uiWantsKeyboard) break;
            
            auto it = m_keyBindings.find(event.scanCode);
            if (it != m_keyBindings.end()) {
                const std::string& action = it->second;
                if (m_activeActions.find(action) == m_activeActions.end()) {
                    m_triggeredActions.insert(action);
                }
                m_activeActions.insert(action);
            }
            break;
        }
        
        case Platform::WindowEventType::KeyUp: {
            auto it = m_keyBindings.find(event.scanCode);
            if (it != m_keyBindings.end()) {
                m_activeActions.erase(it->second);
            }
            break;
        }
        
        case Platform::WindowEventType::MouseMotion: {
            if (m_firstMouseMove) {
                m_lastMouseX = event.mouseX;
                m_lastMouseY = event.mouseY;
                m_firstMouseMove = false;
            }
            
            m_mouseX = event.mouseX;
            m_mouseY = event.mouseY;
            m_mouseDeltaX = event.mouseX - m_lastMouseX;
            m_mouseDeltaY = event.mouseY - m_lastMouseY;
            m_lastMouseX = event.mouseX;
            m_lastMouseY = event.mouseY;
            break;
        }
        
        case Platform::WindowEventType::MouseButtonDown: {
            m_mouseButtonsDown.insert(event.mouseButton);
            break;
        }
        
        case Platform::WindowEventType::MouseButtonUp: {
            m_mouseButtonsDown.erase(event.mouseButton);
            break;
        }
        
        case Platform::WindowEventType::MouseWheel: {
            m_scrollDelta = event.wheelDelta;
            break;
        }
        
        default:
            break;
    }
}

bool InputManager::isActionActive(const std::string& action) const {
    if (m_uiWantsKeyboard) return false;
    return m_activeActions.find(action) != m_activeActions.end();
}

bool InputManager::wasActionTriggered(const std::string& action) const {
    if (m_uiWantsKeyboard) return false;
    return m_triggeredActions.find(action) != m_triggeredActions.end();
}

void InputManager::getMousePosition(float& x, float& y) const {
    x = m_mouseX;
    y = m_mouseY;
}

void InputManager::getMouseDelta(float& dx, float& dy) const {
    dx = m_mouseDeltaX;
    dy = m_mouseDeltaY;
}

bool InputManager::isMouseButtonDown(int button) const {
    if (m_uiWantsMouse) return false;
    return m_mouseButtonsDown.find(button) != m_mouseButtonsDown.end();
}

float InputManager::getScrollDelta() const {
    if (m_uiWantsMouse) return 0.0f;
    return m_scrollDelta;
}

void InputManager::bindKey(int scanCode, const std::string& action) {
    m_keyBindings[scanCode] = action;
}

void InputManager::endFrame() {
    m_triggeredActions.clear();
    m_mouseDeltaX = 0.0f;
    m_mouseDeltaY = 0.0f;
    m_scrollDelta = 0.0f;
}

} // namespace Core

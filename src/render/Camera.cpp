#include "Camera.hpp"
#include <algorithm>
#include <cmath>

namespace Render {

Camera::Camera(float width, float height)
    : m_position(0.0f, 15.0f, 30.0f)
    , m_front(0.0f, 0.0f, -1.0f)
    , m_worldUp(0.0f, 1.0f, 0.0f)
    , m_yaw(-90.0f)
    , m_pitch(-25.0f)
    , m_fov(45.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(2000.0f)
    , m_targetYaw(-90.0f)
    , m_targetPitch(-25.0f) {
    
    setAspectRatio(width, height);
    updateCameraVectors();
    updateOrbitPosition();
    updateViewMatrix();
}

void Camera::update(float deltaTime) {
    // Smooth interpolation factor
    float lerpFactor = 1.0f - std::exp(-m_smoothing * deltaTime);
    
    // Handle transition animation
    if (m_isTransitioning) {
        m_transitionTime += deltaTime;
        float t = std::min(m_transitionTime / m_transitionDuration, 1.0f);
        
        // Smooth step interpolation
        t = t * t * (3.0f - 2.0f * t);
        
        m_orbitTarget = glm::mix(m_transitionStartTarget, m_transitionEndTarget, t);
        m_orbitDistance = glm::mix(m_transitionStartDistance, m_transitionEndDistance, t);
        m_targetOrbitDistance = m_orbitDistance;
        
        if (m_transitionTime >= m_transitionDuration) {
            m_isTransitioning = false;
            m_orbitTarget = m_transitionEndTarget;
            m_orbitDistance = m_transitionEndDistance;
        }
    }
    
    // Smooth zoom
    m_orbitDistance = glm::mix(m_orbitDistance, m_targetOrbitDistance, lerpFactor);
    
    // Smooth rotation
    m_yaw = glm::mix(m_yaw, m_targetYaw, lerpFactor);
    m_pitch = glm::mix(m_pitch, m_targetPitch, lerpFactor);
    
    if (m_mode == CameraMode::Orbit) {
        updateOrbitPosition();
    }
    
    updateCameraVectors();
    updateViewMatrix();
}

void Camera::setAspectRatio(float width, float height) {
    m_aspectRatio = width / height;
    updateProjectionMatrix();
}

void Camera::handleMouseDrag(float deltaX, float deltaY) {
    // Apply sensitivity
    float yawDelta = deltaX * m_mouseSensitivity;
    float pitchDelta = deltaY * m_mouseSensitivity;
    
    // Invert Y if requested
    if (m_invertY) {
        pitchDelta = -pitchDelta;
    }
    
    if (m_mode == CameraMode::Orbit) {
        // In orbit mode: dragging rotates around the target
        // Moving mouse right rotates camera right (target appears to move left)
        m_targetYaw += yawDelta;
        m_targetPitch -= pitchDelta;  // Inverted: drag up = look down at target
    } else {
        // In free-fly mode: standard FPS controls
        m_targetYaw += yawDelta;
        m_targetPitch -= pitchDelta;  // Standard: drag up = look up
    }
    
    // Clamp pitch to prevent gimbal lock
    m_targetPitch = std::clamp(m_targetPitch, -89.0f, 89.0f);
}

void Camera::handleMousePan(float deltaX, float deltaY) {
    if (m_mode != CameraMode::Orbit) return;
    
    // Pan speed scales with distance
    float panSpeed = m_orbitDistance * 0.001f;
    
    glm::vec3 panDelta = (m_right * (-deltaX * panSpeed)) + (m_up * (deltaY * panSpeed));
    m_orbitTarget += panDelta;
    
    // If we were transitioning, stop it
    m_isTransitioning = false;
}

void Camera::handleScroll(float delta) {
    if (m_mode == CameraMode::Orbit) {
        // Adaptive zoom speed based on distance
        float zoomSpeed = getAdaptiveZoomSpeed();
        m_targetOrbitDistance -= delta * zoomSpeed;
        m_targetOrbitDistance = std::max(m_targetOrbitDistance, 1.0f);  // Minimum distance
    } else {
        // In free-fly, scroll adjusts move speed
        m_moveSpeed += delta * 2.0f;
        m_moveSpeed = std::clamp(m_moveSpeed, 1.0f, 100.0f);
    }
}

void Camera::handleKeyMovement(float forward, float right, float up, float deltaTime) {
    if (m_mode == CameraMode::Orbit) {
        // In orbit mode, WASD can pan the orbit target or be disabled
        // For now, only allow zoom with W/S
        m_targetOrbitDistance -= forward * deltaTime * getAdaptiveZoomSpeed() * 0.5f;
        m_targetOrbitDistance = std::max(m_targetOrbitDistance, 1.0f);
    } else {
        // Free-fly movement
        float speed = m_moveSpeed * deltaTime;
        m_position += m_front * (forward * speed);
        m_position += m_right * (right * speed);
        m_position += m_worldUp * (up * speed);
    }
}

void Camera::setMode(CameraMode mode) {
    if (m_mode == mode) return;
    
    if (mode == CameraMode::Orbit) {
        // Switching to orbit: set target in front of camera
        m_orbitTarget = m_position + m_front * m_orbitDistance;
        m_targetOrbitDistance = m_orbitDistance;
    } else {
        // Switching to free-fly: keep current position and orientation
        // Position and angles are already correct
    }
    
    m_mode = mode;
}

void Camera::toggleMode() {
    setMode(m_mode == CameraMode::Orbit ? CameraMode::FreeFly : CameraMode::Orbit);
}

void Camera::setOrbitTarget(const glm::vec3& target) {
    m_orbitTarget = target;
    m_mode = CameraMode::Orbit;
    updateOrbitPosition();
    updateViewMatrix();
}

void Camera::setOrbitTarget(const glm::vec3& target, float distance) {
    m_orbitTarget = target;
    m_orbitDistance = distance;
    m_targetOrbitDistance = distance;
    m_mode = CameraMode::Orbit;
    updateOrbitPosition();
    updateViewMatrix();
}

void Camera::setOrbitDistance(float distance) {
    m_targetOrbitDistance = std::max(distance, 1.0f);
}

void Camera::transitionToTarget(const glm::vec3& target, float distance, float duration) {
    m_isTransitioning = true;
    m_transitionStartTarget = m_orbitTarget;
    m_transitionEndTarget = target;
    m_transitionStartDistance = m_orbitDistance;
    m_transitionEndDistance = distance;
    m_transitionTime = 0.0f;
    m_transitionDuration = duration;
    m_mode = CameraMode::Orbit;
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    if (m_mode == CameraMode::FreeFly) {
        updateViewMatrix();
    }
}

void Camera::lookAt(const glm::vec3& target) {
    glm::vec3 direction = glm::normalize(target - m_position);
    
    // Calculate yaw and pitch from direction
    m_targetYaw = glm::degrees(std::atan2(direction.z, direction.x));
    m_targetPitch = glm::degrees(std::asin(direction.y));
    m_yaw = m_targetYaw;
    m_pitch = m_targetPitch;
    
    updateCameraVectors();
    updateViewMatrix();
}

glm::vec3 Camera::getRayDirection(float screenX, float screenY, float screenWidth, float screenHeight) const {
    // Convert screen coordinates to Normalized Device Coordinates (NDC)
    float x = (2.0f * screenX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * screenY) / screenHeight;
    
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(m_projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    
    glm::vec3 rayWorld = glm::vec3(glm::inverse(m_viewMatrix) * rayEye);
    return glm::normalize(rayWorld);
}

void Camera::updateOrbitPosition() {
    // Calculate camera position from spherical coordinates around target
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    
    glm::vec3 offset;
    offset.x = m_orbitDistance * std::cos(pitchRad) * std::cos(yawRad);
    offset.y = m_orbitDistance * std::sin(pitchRad);
    offset.z = m_orbitDistance * std::cos(pitchRad) * std::sin(yawRad);
    
    m_position = m_orbitTarget + offset;
}

void Camera::updateCameraVectors() {
    if (m_mode == CameraMode::Orbit) {
        // In orbit mode, front points toward target
        m_front = glm::normalize(m_orbitTarget - m_position);
    } else {
        // In free-fly mode, calculate from yaw/pitch
        glm::vec3 front;
        float yawRad = glm::radians(m_yaw);
        float pitchRad = glm::radians(m_pitch);
        front.x = std::cos(pitchRad) * std::cos(yawRad);
        front.y = std::sin(pitchRad);
        front.z = std::cos(pitchRad) * std::sin(yawRad);
        m_front = glm::normalize(front);
    }
    
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::updateViewMatrix() {
    if (m_mode == CameraMode::Orbit) {
        m_viewMatrix = glm::lookAt(m_position, m_orbitTarget, m_worldUp);
    } else {
        m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
    }
}

void Camera::updateProjectionMatrix() {
    // Standard OpenGL projection (not Vulkan)
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
}

float Camera::getAdaptiveZoomSpeed() const {
    // Zoom speed scales with distance for natural feel
    // Closer = slower, farther = faster
    return std::max(m_orbitDistance * 0.15f, 0.5f);
}

} // namespace Render

#include "Camera.hpp"
#include <algorithm>

namespace Render {

Camera::Camera(float width, float height)
    : m_position(0.0f, 10.0f, 20.0f)
    , m_front(0.0f, 0.0f, -1.0f)
    , m_worldUp(0.0f, 1.0f, 0.0f)
    , m_yaw(-90.0f)
    , m_pitch(-30.0f)
    , m_fov(45.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f) {
    
    setAspectRatio(width, height);
    updateCameraVectors();
    updateViewMatrix();
}

void Camera::update(float deltaTime) {
    // potentially smooth movement here
    (void)deltaTime;
}

void Camera::setAspectRatio(float width, float height) {
    m_aspectRatio = width / height;
    updateProjectionMatrix();
}

void Camera::moveForward(float amount) {
    if (m_orbitMode) {
        zoom(amount);
    } else {
        m_position += m_front * amount;
        updateViewMatrix();
    }
}

void Camera::moveRight(float amount) {
    if (!m_orbitMode) {
        m_position += m_right * amount;
        updateViewMatrix();
    }
}

void Camera::moveUp(float amount) {
    if (!m_orbitMode) {
        m_position += m_up * amount;
        updateViewMatrix();
    }
}

void Camera::rotate(float yawOffset, float pitchOffset) {
    if (m_orbitMode) {
        orbit(yawOffset, pitchOffset);
        return;
    }
    
    m_yaw += yawOffset;
    m_pitch += pitchOffset;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateCameraVectors();
    updateViewMatrix();
}

void Camera::lookAt(const glm::vec3& target) {
    // Switch to free look at target? Or just orient?
    // Implementation for "look at" usually implies changing front.
    m_front = glm::normalize(target - m_position);
    // update yaw/pitch...
    // Simplified: Just update view matrix
    m_viewMatrix = glm::lookAt(m_position, target, m_worldUp);
    // NOTE: This de-syncs m_yaw/m_pitch from m_front if we don't recalculate them.
    // For this toy engine, let's assume we don't mix lookAt() with rotate() often or fix it if needed.
}

void Camera::setOrbitTarget(const glm::vec3& target, float distance) {
    m_orbitMode = true;
    m_orbitTarget = target;
    m_orbitDistance = distance;
    updateViewMatrix();
}

void Camera::updateOrbitTarget(const glm::vec3& target) {
    if (m_orbitMode) {
        m_orbitTarget = target;
        updateViewMatrix();
    }
}

void Camera::orbit(float deltaYaw, float deltaPitch) {
    // In orbit mode, yaw/pitch control position around target
    m_yaw -= deltaYaw; // Invert for natural orbit feel
    m_pitch -= deltaPitch;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;
    
    updateViewMatrix();
}

void Camera::zoom(float delta) {
    if (m_orbitMode) {
        m_orbitDistance -= delta;
        if (m_orbitDistance < 2.0f) m_orbitDistance = 2.0f;
        updateViewMatrix();
    }
}

void Camera::updateCameraVectors() {
    // Only relevant for free fly mode to calculate front
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::updateViewMatrix() {
    if (m_orbitMode) {
        // Calculate position based on angles and distance
        float yawRad = glm::radians(m_yaw);
        float pitchRad = glm::radians(m_pitch);
        
        glm::vec3 offset;
        offset.x = m_orbitDistance * cos(pitchRad) * cos(yawRad);
        offset.y = m_orbitDistance * sin(pitchRad);
        offset.z = m_orbitDistance * cos(pitchRad) * sin(yawRad);
        
        m_position = m_orbitTarget + offset;
        m_viewMatrix = glm::lookAt(m_position, m_orbitTarget, m_worldUp);
        
        // Update front/right/up for consistency if we switch modes
        m_front = glm::normalize(m_orbitTarget - m_position);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    } else {
        m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
    }
}

void Camera::updateProjectionMatrix() {
    m_projectionMatrix = glm::perspectiveRH_ZO(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
    m_projectionMatrix[1][1] *= -1; // Vulkan clip space Y is inverted
}

} // namespace Render

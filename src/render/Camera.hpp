#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Render {

/// Camera modes
enum class CameraMode {
    Orbit,      // Orbit around a target point
    FreeFly     // Free-flying FPS-style camera
};

/// Camera controller with smooth movement and multiple modes
class Camera {
public:
    Camera(float width, float height);

    void update(float deltaTime);
    void setAspectRatio(float width, float height);

    const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    glm::vec3 getPosition() const { return m_position; }
    CameraMode getMode() const { return m_mode; }

    // Input handling - call these based on user input
    void handleMouseDrag(float deltaX, float deltaY);
    void handleMousePan(float deltaX, float deltaY);
    void handleScroll(float delta);
    void handleKeyMovement(float forward, float right, float up, float deltaTime);
    
    // Mode switching
    void setMode(CameraMode mode);
    void toggleMode();
    
    // Orbit mode controls
    void setOrbitTarget(const glm::vec3& target);
    void setOrbitTarget(const glm::vec3& target, float distance);
    void setOrbitDistance(float distance);
    float getOrbitDistance() const { return m_targetOrbitDistance; }
    glm::vec3 getOrbitTarget() const { return m_orbitTarget; }
    
    // Smooth transition to new target
    void transitionToTarget(const glm::vec3& target, float distance, float duration = 0.5f);
    
    // Free-fly mode
    void setPosition(const glm::vec3& position);
    void lookAt(const glm::vec3& target);
    
    // Raycasting
    glm::vec3 getRayDirection(float screenX, float screenY, float screenWidth, float screenHeight) const;

    // Settings
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    void setSmoothing(float smoothing) { m_smoothing = smoothing; }
    void setInvertY(bool invert) { m_invertY = invert; }
    
    float getMouseSensitivity() const { return m_mouseSensitivity; }
    float getMoveSpeed() const { return m_moveSpeed; }
    bool isInvertY() const { return m_invertY; }

private:
    void updateViewMatrix();
    void updateProjectionMatrix();
    void updateOrbitPosition();
    void updateCameraVectors();
    float getAdaptiveZoomSpeed() const;

    // Current state
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // Rotation angles (degrees)
    float m_yaw;    // Horizontal rotation
    float m_pitch;  // Vertical rotation

    // Projection
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    
    // Camera mode
    CameraMode m_mode = CameraMode::Orbit;
    
    // Orbit mode state
    glm::vec3 m_orbitTarget = glm::vec3(0.0f);
    float m_orbitDistance = 30.0f;
    float m_targetOrbitDistance = 30.0f;  // For smooth zoom
    
    // Transition animation
    bool m_isTransitioning = false;
    glm::vec3 m_transitionStartTarget;
    glm::vec3 m_transitionEndTarget;
    float m_transitionStartDistance;
    float m_transitionEndDistance;
    float m_transitionTime = 0.0f;
    float m_transitionDuration = 0.5f;
    
    // Smoothing
    float m_smoothing = 8.0f;  // Higher = snappier, lower = smoother
    glm::vec3 m_velocityPosition = glm::vec3(0.0f);
    float m_velocityYaw = 0.0f;
    float m_velocityPitch = 0.0f;
    
    // Target values for smoothing
    float m_targetYaw;
    float m_targetPitch;
    
    // Input settings
    float m_mouseSensitivity = 0.3f;
    float m_moveSpeed = 15.0f;
    bool m_invertY = false;

    // Matrices
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};

} // namespace Render

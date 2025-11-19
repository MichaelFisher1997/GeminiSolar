#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Render {

class Camera {
public:
    Camera(float width, float height);

    void update(float deltaTime);
    void setAspectRatio(float width, float height);

    const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    glm::vec3 getPosition() const { return m_position; }

    // Controls
    void moveForward(float amount);
    void moveRight(float amount);
    void moveUp(float amount);
    void rotate(float yaw, float pitch);
    
    void setPosition(const glm::vec3& position) { m_position = position; updateViewMatrix(); }
    void lookAt(const glm::vec3& target);
    
    void setOrbitTarget(const glm::vec3& target, float distance);
    void orbit(float deltaYaw, float deltaPitch);
    void zoom(float delta);

private:
    void updateViewMatrix();
    void updateProjectionMatrix();
    void updateCameraVectors();

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;

    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    
    // Orbit mode
    bool m_orbitMode = false;
    glm::vec3 m_orbitTarget = glm::vec3(0.0f);
    float m_orbitDistance = 20.0f;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};

} // namespace Render

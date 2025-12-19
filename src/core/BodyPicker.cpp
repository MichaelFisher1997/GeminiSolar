#include "BodyPicker.hpp"
#include <functional>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace Core {

BodyPicker::BodyPicker(Platform::WindowInterface& window)
    : m_window(window) {
}

const Simulation::CelestialBody* BodyPicker::pickBody(
    float mouseX, float mouseY,
    const Simulation::SolarSystem& solarSystem,
    const Render::Camera& camera,
    double simulationTime,
    bool showLabels
) {
    int w, h;
    m_window.getSize(w, h);
    float sw = static_cast<float>(w);
    float sh = static_cast<float>(h);
    
    glm::vec3 rayDir = camera.getRayDirection(mouseX, mouseY, sw, sh);
    glm::vec3 rayOrigin = camera.getPosition();
    
    const Simulation::CelestialBody* bestBody = nullptr;
    float minDist = 1e10f;
    
    float sysScale = solarSystem.getSystemScale();
    float planetScale = solarSystem.getPlanetScale();
    
    glm::mat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
    
    std::function<void(const Simulation::CelestialBody&, glm::vec3)> checkBody;
    checkBody = [&](const Simulation::CelestialBody& body, glm::vec3 parentPos) {
        glm::vec3 worldPos;
        if (solarSystem.isPhysicsEnabled()) {
            worldPos = body.getPhysicsPosition() * sysScale;
        } else {
            worldPos = parentPos + body.getPosition(simulationTime) * sysScale;
        }
        
        float radius = static_cast<float>(body.getRadius());
        
        // Use body type instead of string matching (OCP compliance)
        if (body.isStar()) {
            radius = 1.5f; 
        } else {
            radius *= planetScale;
        }
        
        float clickRadius = std::max(radius, 0.15f); 
        glm::vec3 L = worldPos - rayOrigin;
        float tca = glm::dot(L, rayDir);
        if (tca >= 0) {
            float d2 = glm::dot(L, L) - tca * tca;
            if (d2 <= clickRadius * clickRadius) {
                float t0 = tca - std::sqrt(clickRadius * clickRadius - d2);
                if (t0 < minDist) {
                    minDist = t0;
                    bestBody = &body;
                }
            }
        }
        
        // Label-based picking
        if (showLabels) {
            glm::vec4 clipPos = viewProj * glm::vec4(worldPos, 1.0f);
            if (clipPos.z > 0 && clipPos.w > 0) {
                glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
                float sx = (ndc.x + 1.0f) * 0.5f * sw;
                float sy = (1.0f - ndc.y) * 0.5f * sh;
                
                float dx = mouseX - sx;
                float dy = mouseY - sy;
                if (dx*dx + dy*dy < 900.0f) { // 30px radius
                    if (minDist > 100.0f) { 
                        minDist = 0.0f; 
                        bestBody = &body;
                    }
                }
            }
        }
        
        for (const auto& child : body.getChildren()) {
            checkBody(*child, worldPos);
        }
    };
    
    for (const auto& body : solarSystem.getBodies()) {
        checkBody(*body, glm::vec3(0.0f));
    }
    
    return bestBody;
}

} // namespace Core

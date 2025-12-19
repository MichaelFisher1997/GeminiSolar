#include "PhysicsSimulator.hpp"
#include <cmath>
#include <functional>

namespace Simulation {

PhysicsSimulator::PhysicsSimulator() {
}

void PhysicsSimulator::collectBodies(std::vector<std::unique_ptr<CelestialBody>>& bodies, 
                                      std::vector<CelestialBody*>& outBodies) {
    std::function<void(CelestialBody*)> collect;
    collect = [&](CelestialBody* b) {
        outBodies.push_back(b);
        for (auto& child : b->getChildren()) {
            collect(child.get());
        }
    };
    
    for (auto& b : bodies) {
        collect(b.get());
    }
}

void PhysicsSimulator::initializeFromOrbits(std::vector<std::unique_ptr<CelestialBody>>& bodies, double time) {
    std::function<void(CelestialBody&, glm::vec3, glm::vec3)> initBody;
    initBody = [&](CelestialBody& body, glm::vec3 parentPos, glm::vec3 parentVel) {
        glm::vec3 localPos = body.getPosition(time);
        glm::vec3 worldPos = parentPos + localPos;
        
        // Calculate orbital velocity from position change over small time step
        double dt = 0.0001;
        glm::vec3 nextPos = body.getPosition(time + dt);
        glm::vec3 localVel = (nextPos - localPos) / static_cast<float>(dt);
        glm::vec3 worldVel = parentVel + localVel;
        
        body.setPosition(worldPos);
        body.setVelocity(worldVel);
        
        // Set mass based on radius cubed (approximate uniform density)
        double r = body.getRadius();
        body.setMass(r * r * r);
        
        for (auto& child : body.getChildren()) {
            initBody(*child, worldPos, worldVel);
        }
    };
    
    for (auto& body : bodies) {
        initBody(*body, glm::vec3(0.0f), glm::vec3(0.0f));
    }
    
    // Special handling for the star mass (Sun is massive)
    if (!bodies.empty()) {
        bodies[0]->setMass(1000000.0);
    }
}

void PhysicsSimulator::update(std::vector<std::unique_ptr<CelestialBody>>& bodies, double dt) {
    // Collect all bodies into a flat vector
    std::vector<CelestialBody*> allBodies;
    collectBodies(bodies, allBodies);

    // 1. Compute accelerations (gravitational forces)
    std::vector<glm::vec3> accelerations(allBodies.size(), glm::vec3(0.0f));
    for (size_t i = 0; i < allBodies.size(); ++i) {
        for (size_t j = i + 1; j < allBodies.size(); ++j) {
            glm::vec3 rVec = allBodies[j]->getPhysicsPosition() - allBodies[i]->getPhysicsPosition();
            float distSq = glm::dot(rVec, rVec);
            
            // Softening factor prevents infinite forces at near-zero distances
            const float softeningSq = 0.001f;
            float forceMag = static_cast<float>(m_gravityConstant / (distSq + softeningSq));
            
            float dist = std::sqrt(distSq);
            if (dist < 0.0001f) continue;
            
            glm::vec3 dir = rVec / dist;
            
            accelerations[i] += dir * forceMag * static_cast<float>(allBodies[j]->getMass());
            accelerations[j] -= dir * forceMag * static_cast<float>(allBodies[i]->getMass());
        }
    }

    // 2. Update positions and velocities (Semi-Implicit Euler)
    for (size_t i = 0; i < allBodies.size(); ++i) {
        glm::vec3 v = allBodies[i]->getPhysicsVelocity();
        v += accelerations[i] * static_cast<float>(dt);
        allBodies[i]->setVelocity(v);
        
        glm::vec3 p = allBodies[i]->getPhysicsPosition();
        p += v * static_cast<float>(dt);
        allBodies[i]->setPosition(p);
    }
}

} // namespace Simulation

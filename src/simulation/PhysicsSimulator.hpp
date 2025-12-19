#pragma once

#include "CelestialBody.hpp"
#include <vector>

namespace Simulation {

/// Handles N-body gravitational physics simulation
/// Extracted from SolarSystem to follow Single Responsibility Principle
class PhysicsSimulator {
public:
    PhysicsSimulator();
    
    /// Initialize physics state from orbital positions
    void initializeFromOrbits(std::vector<std::unique_ptr<CelestialBody>>& bodies, double time);
    
    /// Update physics simulation (one step)
    void update(std::vector<std::unique_ptr<CelestialBody>>& bodies, double dt);
    
    /// Set the gravitational constant
    void setGravityConstant(double g) { m_gravityConstant = g; }
    double getGravityConstant() const { return m_gravityConstant; }

private:
    /// Collect all bodies (including children) into a flat list
    void collectBodies(std::vector<std::unique_ptr<CelestialBody>>& bodies, 
                       std::vector<CelestialBody*>& outBodies);
    
    double m_gravityConstant = 0.0001;  // Tuned for the visual scale
};

} // namespace Simulation

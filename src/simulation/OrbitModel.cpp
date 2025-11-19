#include "OrbitModel.hpp"
#include <cmath>

namespace Simulation {

glm::vec3 OrbitModel::calculatePosition(const OrbitalParams& params, double time) {
    if (params.semiMajorAxis == 0.0) {
        return glm::vec3(0.0f); // The Sun (or central body)
    }

    // Mean anomaly: M = M0 + n * t
    // n = 2 * pi / Period
    double n = (2.0 * M_PI) / params.orbitalPeriod;
    double M = params.meanAnomaly0 + n * time;

    // Solve Kepler's Equation for Eccentric Anomaly (E): M = E - e * sin(E)
    // Using Newton-Raphson iteration
    double E = M; // Initial guess
    for (int i = 0; i < 10; ++i) {
        double dM = M - (E - params.eccentricity * std::sin(E));
        double dE = dM / (1.0 - params.eccentricity * std::cos(E));
        E += dE;
        if (std::abs(dE) < 1e-6) break;
    }

    // Calculate true anomaly (v)
    // tan(v/2) = sqrt((1+e)/(1-e)) * tan(E/2)
    double sqrtTerm = std::sqrt((1.0 + params.eccentricity) / (1.0 - params.eccentricity));
    double v = 2.0 * std::atan(sqrtTerm * std::tan(E / 2.0));

    // Distance (r)
    double r = params.semiMajorAxis * (1.0 - params.eccentricity * std::cos(E));

    // Position in orbital plane (x', y', 0)
    // We assume periapsis is at angle 0 for simplicity unless argument of periapsis is added
    double x_orb = r * std::cos(v);
    double y_orb = r * std::sin(v);

    // Rotate by inclination (i) around x-axis (simple model, usually involves Longitude of Ascending Node too)
    // For full orbital elements we need: Longitude of Ascending Node (Omega), Argument of Periapsis (w), Inclination (i)
    // Let's assume Omega=0 and w=0 for simplicity as per spec "inclinations simplified"
    
    // Rotate around X axis by inclination
    double x_inc = x_orb;
    double y_inc = y_orb * std::cos(params.inclination);
    double z_inc = y_orb * std::sin(params.inclination);

    return glm::vec3(static_cast<float>(x_inc), static_cast<float>(z_inc), static_cast<float>(y_inc)); 
    // Note: We map z_inc to y (up) and y_inc to z (depth) usually in 3D graphics, 
    // but standard orbital plane is XY. Let's assume Y is up in our engine.
    // So orbit is in XZ plane primarily.
    // Standard physics: XY plane. 
    // OpenGL/Vulkan: Y up, -Z forward.
    
    // Let's map:
    // Orbit X -> World X
    // Orbit Y -> World Z
    // Orbit Z (inclination lift) -> World Y
    
    return glm::vec3(static_cast<float>(x_inc), static_cast<float>(z_inc), static_cast<float>(y_inc));
}

} // namespace Simulation

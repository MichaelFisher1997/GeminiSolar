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
    // Calculate position with full elements: argument of periapsis (w)
    double u = v + params.argumentPeriapsis; // Argument of Latitude

    double x_orb = r * std::cos(u);
    double y_orb = r * std::sin(u);

    // Rotate by Longitude of Ascending Node (Omega) and Inclination (i)
    // Standard 3D Rotation:
    // x = r * ( cos(Omega) * cos(u) - sin(Omega) * sin(u) * cos(i) )
    // y = r * ( sin(Omega) * cos(u) + cos(Omega) * sin(u) * cos(i) )
    // z = r * ( sin(i) * sin(u) )

    double x_space = r * (std::cos(params.longitudeAscendingNode) * std::cos(u) - std::sin(params.longitudeAscendingNode) * std::sin(u) * std::cos(params.inclination));
    double y_space = r * (std::sin(params.longitudeAscendingNode) * std::cos(u) + std::cos(params.longitudeAscendingNode) * std::sin(u) * std::cos(params.inclination));
    double z_space = r * (std::sin(params.inclination) * std::sin(u));

    // Map to World Coordinates (Y-up)
    // Standard physics is Z-up usually. Here we map z_space -> Y (up).
    return glm::vec3(static_cast<float>(x_space), static_cast<float>(z_space), static_cast<float>(y_space));
}

} // namespace Simulation

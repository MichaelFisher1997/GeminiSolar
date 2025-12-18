#include "OrbitModel.hpp"
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace Simulation {

glm::vec3 OrbitModel::calculatePosition(const OrbitalParams& params, double time) {
    if (params.semiMajorAxis == 0.0 || params.orbitalPeriod == 0.0) {
        return glm::vec3(0.0f); // The Sun (or central body)
    }

    const double PI = glm::pi<double>();

    // Mean anomaly: M = M0 + n * t
    // n = 2 * pi / Period
    double n = (2.0 * PI) / params.orbitalPeriod;
    double M = params.meanAnomaly0 + n * time;

    // Wrap M to [0, 2pi]
    M = std::fmod(M, 2.0 * PI);
    if (M < 0) M += 2.0 * PI;

    // Solve Kepler's Equation for Eccentric Anomaly (E): M = E - e * sin(E)
    // Using Newton-Raphson iteration
    double E = M; // Initial guess
    for (int i = 0; i < 15; ++i) {
        double dM = M - (E - params.eccentricity * std::sin(E));
        double dE = dM / (1.0 - params.eccentricity * std::cos(E));
        E += dE;
        if (std::abs(dE) < 1e-8) break;
    }

    // Calculate true anomaly (v)
    // tan(v/2) = sqrt((1+e)/(1-e)) * tan(E/2)
    double sqrtTerm = std::sqrt((1.0 + params.eccentricity) / (1.0 - params.eccentricity));
    double v = 2.0 * std::atan(sqrtTerm * std::tan(E / 2.0));

    // Distance (r)
    double r = params.semiMajorAxis * (1.0 - params.eccentricity * std::cos(E));

    // Position in orbital plane (x', y', 0)
    double u = v + params.argumentPeriapsis; // Argument of Latitude

    // Rotate by Longitude of Ascending Node (Omega) and Inclination (i)
    // Standard 3D Rotation:
    double cosU = std::cos(u);
    double sinU = std::sin(u);
    double cosOmega = std::cos(params.longitudeAscendingNode);
    double sinOmega = std::sin(params.longitudeAscendingNode);
    double cosI = std::cos(params.inclination);
    double sinI = std::sin(params.inclination);

    double x_space = r * (cosOmega * cosU - sinOmega * sinU * cosI);
    double y_space = r * (sinOmega * cosU + cosOmega * sinU * cosI);
    double z_space = r * (sinI * sinU);

    // Map to World Coordinates (Y-up)
    return glm::vec3(static_cast<float>(x_space), static_cast<float>(z_space), static_cast<float>(y_space));
}

} // namespace Simulation

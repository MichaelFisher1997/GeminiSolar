#pragma once

#include <vector>
#include <memory>
#include "CelestialBody.hpp"

namespace Simulation {

class SolarSystem {
public:
    SolarSystem();

    void init();

    const std::vector<std::unique_ptr<CelestialBody>>& getBodies() const { return m_bodies; }
    const CelestialBody* getSun() const;

private:
    std::vector<std::unique_ptr<CelestialBody>> m_bodies;
};

} // namespace Simulation

#include "Time.hpp"

namespace Core {

Time::Time() 
    : m_simulationTime(0.0)
    , m_timeScale(1.0)
    , m_paused(false) {
}

void Time::update(double deltaRealSeconds) {
    if (!m_paused) {
        m_simulationTime += deltaRealSeconds * m_timeScale;
    }
}

} // namespace Core

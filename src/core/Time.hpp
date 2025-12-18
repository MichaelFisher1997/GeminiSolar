#pragma once

namespace Core {

class Time {
public:
    Time();

    void update(double deltaRealSeconds);

    void setPaused(bool paused) { m_paused = paused; }
    bool isPaused() const { return m_paused; }

    void setTimeScale(double scale) { m_timeScale = scale; }
    double getTimeScale() const { return m_timeScale; }

    void setSimulationTime(double time) { m_simulationTime = time; }
    double getSimulationTime() const { return m_simulationTime; }

private:
    double m_simulationTime = 0.0; // Days or relevant unit since epoch
    double m_timeScale = 1.0;      // 1 real second = X simulation units
    bool m_paused = false;
};

} // namespace Core

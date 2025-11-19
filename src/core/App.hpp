#include <memory>
#include "platform/SDLWindow.hpp"
// #include "render/VulkanContext.hpp" // Hide Vulkan for now
#include "Time.hpp"
#include "simulation/SolarSystem.hpp"
#include "render/Camera.hpp"
#include "render/RenderInterface.hpp"

namespace Core {

class App {
public:
    App();
    ~App();

    void run();

private:
    void update(double deltaTime);
    void render();

    std::unique_ptr<Platform::SDLWindow> m_window;
    // std::unique_ptr<Render::VulkanContext> m_vulkanContext;
    std::unique_ptr<Time> m_time;
    std::unique_ptr<Simulation::SolarSystem> m_solarSystem;
    std::unique_ptr<Render::Camera> m_camera;
    std::unique_ptr<Render::RenderInterface> m_renderer;
    
    bool m_isRunning = false;
};

} // namespace Core

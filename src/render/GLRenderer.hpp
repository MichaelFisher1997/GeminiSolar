#pragma once

#include "RenderInterface.hpp"
#include "ShaderManager.hpp"
#include "MeshFactory.hpp"
#include "UIManager.hpp"
#include "platform/SDLWindow.hpp"
#include <memory>
#include <glm/glm.hpp>

namespace Render {

/// OpenGL-based renderer implementation
class GLRenderer : public RenderInterface {
public:
    explicit GLRenderer(Platform::SDLWindow& window);
    ~GLRenderer() override;

    void render(const Simulation::SolarSystem& solarSystem, 
                const Camera& camera, 
                double simulationTime, 
                const Simulation::CelestialBody* hoveredBody = nullptr,
                std::function<void()> uiCallback = nullptr);
    void resize(int width, int height) override;
    
    /// Forward SDL event to ImGui
    void processEvent(const SDL_Event& event);

    void setShowOrbits(bool show) { m_showOrbits = show; }
    bool isShowOrbits() const { return m_showOrbits; }
    
    void setShowLabels(bool show) { m_showLabels = show; }
    bool isShowLabels() const { return m_showLabels; }

private:
    void initGL();
    void loadShaders();
    void createMeshes();

    Platform::SDLWindow& m_window;
    
    // Managers
    std::unique_ptr<ShaderManager> m_shaderManager;
    std::unique_ptr<UIManager> m_uiManager;
    
    // Meshes
    std::unique_ptr<GLMesh> m_sphereMesh;
    std::unique_ptr<GLMesh> m_orbitMesh;
    
    bool m_showOrbits = true;
    bool m_showLabels = true;
    
    // Shader names
    static constexpr const char* SHADER_PLANET = "planet";
    static constexpr const char* SHADER_ORBIT = "orbit";
};

} // namespace Render

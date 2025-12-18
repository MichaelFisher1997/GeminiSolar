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
                std::function<void()> uiCallback = nullptr) override;
    void resize(int width, int height) override;
    
    /// Forward SDL event to ImGui
    void processEvent(const SDL_Event& event);

    void setShowOrbits(bool show) { m_showOrbits = show; }
    bool isShowOrbits() const { return m_showOrbits; }

private:
    void initGL();
    void loadShaders();
    void createMeshes();
    void renderStarfield(const Camera& camera);

    Platform::SDLWindow& m_window;
    
    // Managers
    std::unique_ptr<ShaderManager> m_shaderManager;
    std::unique_ptr<UIManager> m_uiManager;
    
    // Meshes
    std::unique_ptr<GLMesh> m_sphereMesh;
    std::unique_ptr<GLMesh> m_orbitMesh;
    std::unique_ptr<GLMesh> m_starfieldMesh;
    
    bool m_showOrbits = true;
    
    // Shader names
    static constexpr const char* SHADER_PLANET = "planet";
    static constexpr const char* SHADER_ORBIT = "orbit";
    static constexpr const char* SHADER_STARFIELD = "starfield";
};

} // namespace Render

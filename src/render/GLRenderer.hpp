#pragma once

#include "RenderInterface.hpp"
#include "platform/SDLWindow.hpp"
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <vector>
#include <glm/glm.hpp>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

namespace Render {

class GLRenderer : public RenderInterface {
public:
    GLRenderer(Platform::SDLWindow& window);
    ~GLRenderer() override;

    void render(const Simulation::SolarSystem& solarSystem, const Camera& camera, double simulationTime, std::function<void()> uiCallback = nullptr) override;
    void resize(int width, int height) override;

private:
    void initGL();
    void initImGui();
    void createShaders();
    void createSphereMesh();

    Platform::SDLWindow& m_window;
    SDL_GLContext m_glContext;

    GLuint m_shaderProgram;
    GLuint m_vao, m_vbo, m_ebo;
    uint32_t m_indexCount;

    GLint m_modelLoc, m_viewLoc, m_projLoc, m_colorLoc, m_isSunLoc;

    // Orbit Lines
    GLuint m_orbitVao, m_orbitVbo;
    void createOrbitMesh();
    void drawOrbit(float radius, const glm::vec3& color, const glm::mat4& view, const glm::mat4& proj);
};

} // namespace Render

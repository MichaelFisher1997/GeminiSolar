#include "GLRenderer.hpp"
#include "core/Logger.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>

namespace Render {

GLRenderer::GLRenderer(Platform::SDLWindow& window) 
    : m_window(window) {
    
    initGL();
    
    // Create managers
    m_shaderManager = std::make_unique<ShaderManager>();
    m_uiManager = std::make_unique<UIManager>(window, window.getGLContext());
    
    loadShaders();
    createMeshes();
    
    LOG_INFO("GLRenderer", "OpenGL renderer initialized");
}

GLRenderer::~GLRenderer() {
    m_sphereMesh.reset();
    m_orbitMesh.reset();
    m_uiManager.reset();
    m_shaderManager.reset();
    LOG_INFO("GLRenderer", "OpenGL renderer destroyed");
}

void GLRenderer::initGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
    LOG_INFO("GLRenderer", "OpenGL state initialized");
}

void GLRenderer::loadShaders() {
    // Try to load from files first, fall back to embedded shaders
    try {
        m_shaderManager->loadFromFiles(SHADER_PLANET, 
                                        "assets/shaders/planet.vert",
                                        "assets/shaders/planet.frag");
        m_shaderManager->loadFromFiles(SHADER_ORBIT, 
                                        "assets/shaders/orbit.vert",
                                        "assets/shaders/orbit.frag");
        m_shaderManager->loadFromFiles(SHADER_STARFIELD, 
                                        "assets/shaders/starfield.vert",
                                        "assets/shaders/starfield.frag");
    } catch (const std::exception& e) {
        LOG_WARN("GLRenderer", "Failed to load some shader files, some features may be missing: ", e.what());
    }
}

void GLRenderer::createMeshes() {
    m_sphereMesh = MeshFactory::createSphere(48, 48);
    m_orbitMesh = MeshFactory::createCircle(256);
    m_starfieldMesh = MeshFactory::createSphere(16, 16); // Lower detail for starfield
    LOG_INFO("GLRenderer", "Meshes created");
}

void GLRenderer::render(const Simulation::SolarSystem& solarSystem, 
                        const Camera& camera, 
                        double simulationTime, 
                        std::function<void()> uiCallback) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 1. Render Starfield
    renderStarfield(camera);
    
    // 2. Begin UI frame (to collect layout before drawing objects)
    m_uiManager->beginFrame();
    if (uiCallback) uiCallback();
    
    // 3. Render Solar System
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    GLuint planetShader = m_shaderManager->getShader(SHADER_PLANET);
    GLuint orbitShader = m_shaderManager->getShader(SHADER_ORBIT);
    
    m_shaderManager->useShader(planetShader);
    GLint modelLoc = m_shaderManager->getUniformLocation(planetShader, "model");
    GLint viewLoc = m_shaderManager->getUniformLocation(planetShader, "view");
    GLint projLoc = m_shaderManager->getUniformLocation(planetShader, "projection");
    GLint colorLoc = m_shaderManager->getUniformLocation(planetShader, "objectColor");
    GLint isSunLoc = m_shaderManager->getUniformLocation(planetShader, "isSun");
    GLint timeLoc = m_shaderManager->getUniformLocation(planetShader, "time");
    GLint viewPosLoc = m_shaderManager->getUniformLocation(planetShader, "viewPos");
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();
    glm::vec3 viewPos = camera.getPosition();
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
    glUniform1f(timeLoc, static_cast<float>(simulationTime));
    
    const auto& bodies = solarSystem.getBodies();
    float visualDistanceScale = solarSystem.getSystemScale();
    float visualPlanetScale = solarSystem.getPlanetScale();
    
    // Recursive body rendering function
    std::function<void(const Simulation::CelestialBody&, glm::mat4)> drawBodyRecursive;
    drawBodyRecursive = [&](const Simulation::CelestialBody& body, glm::mat4 parentTransform) {
        glm::vec3 localPos = body.getPosition(simulationTime);
        float visualRadiusScale = visualPlanetScale;
        bool isSun = false;
        
        if (body.getName() == solarSystem.getSun()->getName()) {
            visualRadiusScale = 1.5f / static_cast<float>(body.getRadius());
            isSun = true;
        }
        
        glm::mat4 posMatrix = glm::translate(parentTransform, localPos * visualDistanceScale);
        
        // Draw Orbits
        if (m_showOrbits) {
            glUseProgram(orbitShader);
            GLint oModelLoc = m_shaderManager->getUniformLocation(orbitShader, "model");
            GLint oViewLoc = m_shaderManager->getUniformLocation(orbitShader, "view");
            GLint oProjLoc = m_shaderManager->getUniformLocation(orbitShader, "projection");
            GLint oColorLoc = m_shaderManager->getUniformLocation(orbitShader, "orbitColor");
            GLint oOpacityLoc = m_shaderManager->getUniformLocation(orbitShader, "opacity");
            
            glUniformMatrix4fv(oViewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(oProjLoc, 1, GL_FALSE, glm::value_ptr(proj));
            
            for (const auto& child : body.getChildren()) {
                float orbitRadius = static_cast<float>(child->getOrbitalParams().semiMajorAxis) * visualDistanceScale;
                glm::mat4 orbitModel = glm::scale(posMatrix, glm::vec3(orbitRadius));
                glUniformMatrix4fv(oModelLoc, 1, GL_FALSE, glm::value_ptr(orbitModel));
                glUniform3f(oColorLoc, 0.4f, 0.4f, 0.5f);
                glUniform1f(oOpacityLoc, 0.2f);
                
                m_orbitMesh->draw(GL_LINE_LOOP);
            }
            glUseProgram(planetShader);
        }

        // Draw Body
        {
            float scale = static_cast<float>(body.getRadius()) * visualRadiusScale;
            glm::mat4 model = glm::scale(posMatrix, glm::vec3(scale));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(isSunLoc, isSun ? 1 : 0);
            glUniform3fv(colorLoc, 1, glm::value_ptr(body.getColor()));
            m_sphereMesh->draw();
        }
        
        for (const auto& child : body.getChildren()) {
            drawBodyRecursive(*child, posMatrix);
        }
    };
    
    glm::mat4 identity = glm::mat4(1.0f);
    for (const auto& body : bodies) {
        if (m_showOrbits && body.get() != solarSystem.getSun()) {
            glUseProgram(orbitShader);
            float orbitRadius = static_cast<float>(body->getOrbitalParams().semiMajorAxis) * visualDistanceScale;
            glm::mat4 orbitModel = glm::scale(identity, glm::vec3(orbitRadius));
            glUniformMatrix4fv(m_shaderManager->getUniformLocation(orbitShader, "model"), 1, GL_FALSE, glm::value_ptr(orbitModel));
            glUniformMatrix4fv(m_shaderManager->getUniformLocation(orbitShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(m_shaderManager->getUniformLocation(orbitShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
            glUniform3f(m_shaderManager->getUniformLocation(orbitShader, "orbitColor"), 0.3f, 0.3f, 0.4f);
            glUniform1f(m_shaderManager->getUniformLocation(orbitShader, "opacity"), 0.3f);
            m_orbitMesh->draw(GL_LINE_LOOP);
            glUseProgram(planetShader);
        }
        drawBodyRecursive(*body, identity);
    }
    
    glDisable(GL_BLEND);
    m_uiManager->endFrame();
    m_window.swapBuffers();
}

void GLRenderer::renderStarfield(const Camera& camera) {
    glDepthMask(GL_FALSE);
    GLuint shader = m_shaderManager->getShader(SHADER_STARFIELD);
    m_shaderManager->useShader(shader);
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f)); // Scale doesn't matter much for infinite background
    
    glUniformMatrix4fv(m_shaderManager->getUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(m_shaderManager->getUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_shaderManager->getUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    
    m_starfieldMesh->draw();
    glDepthMask(GL_TRUE);
}

void GLRenderer::resize(int width, int height) {
    glViewport(0, 0, width, height);
    LOG_DEBUG("GLRenderer", "Viewport resized to ", width, "x", height);
}

void GLRenderer::processEvent(const SDL_Event& event) {
    m_uiManager->processEvent(event);
}

} // namespace Render

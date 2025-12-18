#include "GLRenderer.hpp"
#include "core/Logger.hpp"
#include "simulation/OrbitModel.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Pitch black
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
    } catch (const std::exception& e) {
        LOG_WARN("GLRenderer", "Failed to load some shader files, some features may be missing: ", e.what());
    }
}

void GLRenderer::createMeshes() {
    m_sphereMesh = MeshFactory::createSphere(48, 48);
    m_orbitMesh = MeshFactory::createCircle(256);
    LOG_INFO("GLRenderer", "Meshes created");
}

void GLRenderer::render(const Simulation::SolarSystem& solarSystem, 
                        const Camera& camera, 
                        double simulationTime, 
                        const Simulation::CelestialBody* hoveredBody,
                        std::function<void()> uiCallback) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 1. Begin UI frame
    m_uiManager->beginFrame();
    if (uiCallback) uiCallback();
    
    // 2. Render Solar System
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
    GLint highlightLoc = m_shaderManager->getUniformLocation(planetShader, "highlight");
    
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
        glm::vec3 localPos;
        if (solarSystem.isPhysicsEnabled()) {
            localPos = body.getPhysicsPosition();
            // In physics mode, position is already in world space, but we follow the hierarchy in the loop.
            // Actually, my physics integrator updates world space positions.
            // So I should pass identity as parent transform or just use the physics position directly.
        } else {
            localPos = body.getPosition(simulationTime);
        }
        
        float visualRadiusScale = visualPlanetScale;
        bool isSun = false;
        
        if (body.getName() == solarSystem.getSun()->getName()) {
            visualRadiusScale = 1.5f / static_cast<float>(body.getRadius());
            isSun = true;
        }
        
        glm::mat4 posMatrix;
        if (solarSystem.isPhysicsEnabled()) {
             posMatrix = glm::translate(glm::mat4(1.0f), body.getPhysicsPosition() * visualDistanceScale);
        } else {
             posMatrix = glm::translate(parentTransform, localPos * visualDistanceScale);
        }
        
        // Draw Orbits
        if (m_showOrbits && !solarSystem.isPhysicsEnabled()) {
            glUseProgram(orbitShader);
            GLint oModelLoc = m_shaderManager->getUniformLocation(orbitShader, "model");
            GLint oViewLoc = m_shaderManager->getUniformLocation(orbitShader, "view");
            GLint oProjLoc = m_shaderManager->getUniformLocation(orbitShader, "projection");
            GLint oColorLoc = m_shaderManager->getUniformLocation(orbitShader, "orbitColor");
            GLint oOpacityLoc = m_shaderManager->getUniformLocation(orbitShader, "opacity");
            
            glUniformMatrix4fv(oViewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(oProjLoc, 1, GL_FALSE, glm::value_ptr(proj));
            
            for (const auto& child : body.getChildren()) {
                const int segments = 128;
                std::vector<glm::vec3> points;
                points.reserve(segments);
                
                Simulation::OrbitalParams params = child->getOrbitalParams();
                for (int i = 0; i < segments; ++i) {
                    double meanAnomaly = (static_cast<double>(i) / segments) * 2.0 * glm::pi<double>();
                    Simulation::OrbitalParams p = params;
                    p.meanAnomaly0 = meanAnomaly;
                    // Calculate position relative to parent
                    points.push_back(Simulation::OrbitModel::calculatePosition(p, 0.0) * visualDistanceScale);
                }
                
                static GLuint vao_moon = 0, vbo_moon = 0;
                if (vao_moon == 0) {
                    glGenVertexArrays(1, &vao_moon);
                    glGenBuffers(1, &vbo_moon);
                }
                
                glUniformMatrix4fv(oModelLoc, 1, GL_FALSE, glm::value_ptr(posMatrix));
                glUniform3f(oColorLoc, 0.4f, 0.4f, 0.5f);
                glUniform1f(oOpacityLoc, 0.2f);
                
                glBindVertexArray(vao_moon);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_moon);
                glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STREAM_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
                glEnableVertexAttribArray(0);
                glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(points.size()));
            }
            glUseProgram(planetShader);
        }

        // Draw Body
        {
            float scale = static_cast<float>(body.getRadius()) * visualRadiusScale;
            glm::mat4 model = glm::scale(posMatrix, glm::vec3(scale));
            
            float highlight = (hoveredBody == &body) ? 1.0f : 0.0f;
            glUniform1f(highlightLoc, highlight);
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(isSunLoc, isSun ? 1 : 0);
            glUniform3fv(colorLoc, 1, glm::value_ptr(body.getColor()));
            m_sphereMesh->draw();
        }
        
        for (const auto& child : body.getChildren()) {
            drawBodyRecursive(*child, posMatrix);
        }
    };
    
    for (const auto& body : bodies) {
        if (m_showOrbits && body.get() != solarSystem.getSun()) {
            glUseProgram(orbitShader);
            GLint oModelLoc = m_shaderManager->getUniformLocation(orbitShader, "model");
            GLint oViewLoc = m_shaderManager->getUniformLocation(orbitShader, "view");
            GLint oProjLoc = m_shaderManager->getUniformLocation(orbitShader, "projection");
            GLint oColorLoc = m_shaderManager->getUniformLocation(orbitShader, "orbitColor");
            GLint oOpacityLoc = m_shaderManager->getUniformLocation(orbitShader, "opacity");
            
            glUniformMatrix4fv(oViewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(oProjLoc, 1, GL_FALSE, glm::value_ptr(proj));
            glUniformMatrix4fv(oModelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
            glUniform3f(oColorLoc, 0.3f, 0.3f, 0.4f);
            glUniform1f(oOpacityLoc, 0.3f);
            
            // Draw elliptical orbit path
            const int segments = 256;
            std::vector<glm::vec3> points;
            points.reserve(segments);
            
            Simulation::OrbitalParams params = body->getOrbitalParams();
            for (int i = 0; i < segments; ++i) {
                double meanAnomaly = (static_cast<double>(i) / segments) * 2.0 * glm::pi<double>();
                Simulation::OrbitalParams p = params;
                p.meanAnomaly0 = meanAnomaly;
                points.push_back(Simulation::OrbitModel::calculatePosition(p, 0.0) * visualDistanceScale);
            }
            
            static GLuint vao = 0, vbo = 0;
            if (vao == 0) {
                glGenVertexArrays(1, &vao);
                glGenBuffers(1, &vbo);
            }
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STREAM_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
            glEnableVertexAttribArray(0);
            glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(points.size()));
            
            glUseProgram(planetShader);
        }
        
        drawBodyRecursive(*body, glm::mat4(1.0f));
    }
    
    glDisable(GL_BLEND);
    m_uiManager->endFrame();
    m_window.swapBuffers();
}

void GLRenderer::resize(int width, int height) {
    glViewport(0, 0, width, height);
    LOG_DEBUG("GLRenderer", "Viewport resized to ", width, "x", height);
}

void GLRenderer::processEvent(const SDL_Event& event) {
    m_uiManager->processEvent(event);
}

} // namespace Render

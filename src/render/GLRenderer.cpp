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
    } catch (const std::exception& e) {
        LOG_WARN("GLRenderer", "Failed to load shader files, using embedded shaders: ", e.what());
        
        // Embedded fallback shaders
        const char* vertexShader = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec3 aColor; 
            layout (location = 2) in vec3 aNormal;

            out vec3 FragPos;
            out vec3 Normal;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;

            void main() {
                FragPos = vec3(model * vec4(aPos, 1.0));
                Normal = mat3(transpose(inverse(model))) * aNormal;  
                gl_Position = projection * view * vec4(FragPos, 1.0);
            }
        )";

        const char* fragmentShader = R"(
            #version 330 core
            out vec4 FragColor;

            in vec3 FragPos;
            in vec3 Normal;

            uniform vec3 objectColor;
            uniform int isSun;

            void main() {
                if (isSun == 1) {
                    FragColor = vec4(objectColor, 1.0);
                    return;
                }

                float ambientStrength = 0.2;
                vec3 lightColor = vec3(1.0, 1.0, 1.0);
                vec3 ambient = ambientStrength * lightColor;

                vec3 norm = normalize(Normal);
                vec3 lightPos = vec3(0.0, 0.0, 0.0); 
                vec3 lightDir = normalize(lightPos - FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * lightColor;
                
                vec3 result = (ambient + diffuse) * objectColor;
                FragColor = vec4(result, 1.0);
            }
        )";
        
        m_shaderManager->loadFromSource(SHADER_PLANET, vertexShader, fragmentShader);
    }
}

void GLRenderer::createMeshes() {
    m_sphereMesh = MeshFactory::createSphere(32, 32);
    m_orbitMesh = MeshFactory::createCircle(128);
    LOG_INFO("GLRenderer", "Meshes created");
}

void GLRenderer::render(const Simulation::SolarSystem& solarSystem, 
                        const Camera& camera, 
                        double simulationTime, 
                        std::function<void()> uiCallback) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Begin UI frame
    m_uiManager->beginFrame();
    
    // Run UI callback
    if (uiCallback) {
        uiCallback();
    }
    
    // Get shader and uniforms
    GLuint shader = m_shaderManager->getShader(SHADER_PLANET);
    m_shaderManager->useShader(shader);
    
    GLint modelLoc = m_shaderManager->getUniformLocation(shader, "model");
    GLint viewLoc = m_shaderManager->getUniformLocation(shader, "view");
    GLint projLoc = m_shaderManager->getUniformLocation(shader, "projection");
    GLint colorLoc = m_shaderManager->getUniformLocation(shader, "objectColor");
    GLint isSunLoc = m_shaderManager->getUniformLocation(shader, "isSun");
    
    // Set view and projection matrices
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();
    proj[1][1] *= -1;  // Flip Y for OpenGL
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
    
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
        
        // Draw body
        {
            float scale = static_cast<float>(body.getRadius()) * visualRadiusScale;
            glm::mat4 model = glm::scale(posMatrix, glm::vec3(scale));
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(isSunLoc, isSun ? 1 : 0);
            glUniform3fv(colorLoc, 1, glm::value_ptr(body.getColor()));
            
            m_sphereMesh->draw();
        }
        
        // Draw orbit lines for children
        for (const auto& child : body.getChildren()) {
            float orbitRadius = static_cast<float>(child->getOrbitalParams().semiMajorAxis) * visualDistanceScale;
            
            glUniform1i(isSunLoc, 1);  // Use unlit for orbit lines
            glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.2f)));
            
            glm::mat4 orbitModel = glm::scale(posMatrix, glm::vec3(orbitRadius));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(orbitModel));
            
            m_orbitMesh->bind();
            glDrawArrays(GL_LINE_LOOP, 0, 129);  // 128 segments + 1
        }
        
        // Recurse for children
        for (const auto& child : body.getChildren()) {
            drawBodyRecursive(*child, posMatrix);
        }
    };
    
    glm::mat4 identity = glm::mat4(1.0f);
    
    // Draw all root bodies and their orbit lines
    for (const auto& body : bodies) {
        // Draw orbit line for planets (not for sun)
        if (body.get() != solarSystem.getSun()) {
            float orbitRadius = static_cast<float>(body->getOrbitalParams().semiMajorAxis) * visualDistanceScale;
            
            glUniform1i(isSunLoc, 1);
            glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.2f)));
            
            glm::mat4 orbitModel = glm::scale(identity, glm::vec3(orbitRadius));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(orbitModel));
            
            m_orbitMesh->bind();
            glDrawArrays(GL_LINE_LOOP, 0, 129);
        }
        
        drawBodyRecursive(*body, identity);
    }
    
    m_sphereMesh->unbind();
    
    // End UI frame and render
    m_uiManager->endFrame();
    
    // Swap buffers
    m_window.swapBuffers();
}

void GLRenderer::resize(int width, int height) {
    glViewport(0, 0, width, height);
    LOG_DEBUG("GLRenderer", "Viewport resized to ", width, "x", height);
}

} // namespace Render

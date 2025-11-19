#include "GLRenderer.hpp"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <functional>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Render {

GLRenderer::GLRenderer(Platform::SDLWindow& window) 
    : m_window(window) {
    
    // Set GL attributes BEFORE context creation
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_glContext = SDL_GL_CreateContext(window.getHandle());
    if (!m_glContext) {
        throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
    }

    SDL_GL_MakeCurrent(window.getHandle(), m_glContext);
    
    // Init GLEW
    glewExperimental = GL_TRUE; 
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("Failed to initialize GLEW: " + std::string((const char*)glewGetErrorString(err)));
    }

    initGL();
    initImGui();
    createShaders();
    createSphereMesh();
    createOrbitMesh();
}

GLRenderer::~GLRenderer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    
    glDeleteVertexArrays(1, &m_orbitVao);
    glDeleteBuffers(1, &m_orbitVbo);
    
    glDeleteProgram(m_shaderProgram);
    SDL_GL_DestroyContext(m_glContext);
}

void GLRenderer::initGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void GLRenderer::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(m_window.getHandle(), m_glContext);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void GLRenderer::createShaders() {
    const char* vertexShaderSource = R"(
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

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 FragPos;
        in vec3 Normal;

        uniform vec3 objectColor;
        uniform int isSun; // 1 if Sun, 0 otherwise

        void main() {
            // Special case for Sun: Unlit bright color
            if (isSun == 1) {
                FragColor = vec4(objectColor, 1.0);
                return;
            }

            // Ambient
            float ambientStrength = 0.2;
            vec3 lightColor = vec3(1.0, 1.0, 1.0);
            vec3 ambient = ambientStrength * lightColor;
  
            // Diffuse 
            vec3 norm = normalize(Normal);
            vec3 lightPos = vec3(0.0, 0.0, 0.0); // Sun at origin
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            vec3 result = (ambient + diffuse) * objectColor;
            FragColor = vec4(result, 1.0);
        } 
    )";

    auto compileShader = [](GLenum type, const char* source) -> GLuint {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        return shader;
    };

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_modelLoc = glGetUniformLocation(m_shaderProgram, "model");
    m_viewLoc = glGetUniformLocation(m_shaderProgram, "view");
    m_projLoc = glGetUniformLocation(m_shaderProgram, "projection");
    m_colorLoc = glGetUniformLocation(m_shaderProgram, "objectColor");
    m_isSunLoc = glGetUniformLocation(m_shaderProgram, "isSun");
}

void GLRenderer::createOrbitMesh() {
    std::vector<float> vertices;
    const int segments = 128;
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = cosf(theta);
        float z = sinf(theta);
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
        // Dummies
        vertices.push_back(0.3f); vertices.push_back(0.3f); vertices.push_back(0.3f);
        vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
    }

    glGenVertexArrays(1, &m_orbitVao);
    glGenBuffers(1, &m_orbitVbo);
    
    glBindVertexArray(m_orbitVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_orbitVbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void GLRenderer::drawOrbit(float radius, const glm::vec3& color, const glm::mat4& view, const glm::mat4& proj) {
    (void)view; 
    (void)proj;

    glUniform1i(m_isSunLoc, 1); 
    glUniform3fv(m_colorLoc, 1, glm::value_ptr(color));
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(radius));
    
    glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    glBindVertexArray(m_orbitVao);
    glDrawArrays(GL_LINE_LOOP, 0, 128);
    glBindVertexArray(0);
}

void GLRenderer::createSphereMesh() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const int sectorCount = 32;
    const int stackCount = 32;
    float radius = 1.0f;

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        
        xy = radius * cosf(stackAngle);             
        z = radius * sinf(stackAngle);              

        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           

            x = xy * cosf(sectorAngle);             
            y = xy * sinf(sectorAngle);             
            
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // Pos
            vertices.push_back(x);
            vertices.push_back(z);
            vertices.push_back(y);
            
            // Color
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);

            // Normal
            vertices.push_back(nx);
            vertices.push_back(nz);
            vertices.push_back(ny);
        }
    }

    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     
        k2 = k1 + sectorCount + 1;      

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if(i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if(i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    
    m_indexCount = static_cast<uint32_t>(indices.size());

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void GLRenderer::render(const Simulation::SolarSystem& solarSystem, const Camera& camera, double simulationTime, std::function<void()> uiCallback) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    // Execute UI Callback
    if (uiCallback) {
        uiCallback();
    }
    
    ImGui::Render();
    
    // Draw Scene
    glUseProgram(m_shaderProgram);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();
    
    proj[1][1] *= -1; // Vulkan fix flip

    glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    const auto& bodies = solarSystem.getBodies();
    float visualDistanceScale = 10.0f;

    // Helper for recursive body rendering
    std::function<void(const Simulation::CelestialBody&, glm::mat4)> drawBodyRecursive;
    drawBodyRecursive = [&](const Simulation::CelestialBody& body, glm::mat4 parentTransform) {
        glm::vec3 localPos = body.getPosition(simulationTime);
        
        // Visual scale logic
        float visualRadiusScale = 0.5f;
        bool isSun = false;
        if (body.getName() == "Sun") {
             visualRadiusScale = 1.5f / 109.0f; 
             isSun = true;
        }

        // Position matrix (Orbit position relative to parent)
        glm::mat4 posMatrix = glm::translate(parentTransform, localPos * visualDistanceScale);
        
        // Draw Body
        {
            double scale = body.getRadius();
            glm::mat4 model = glm::scale(posMatrix, glm::vec3(static_cast<float>(scale) * visualRadiusScale));

            glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(m_isSunLoc, isSun ? 1 : 0);
            
            glm::vec3 color = body.getColor(); 
            glUniform3fv(m_colorLoc, 1, glm::value_ptr(color));

            glBindVertexArray(m_vao);
            glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        }
        
        // Draw Orbits for children
        for (const auto& child : body.getChildren()) {
             float orbitRadius = child->getOrbitalParams().semiMajorAxis * visualDistanceScale;
             
             // Inline drawOrbit logic here for simplicity in recursion
             glUniform1i(m_isSunLoc, 1); 
             glUniform3fv(m_colorLoc, 1, glm::value_ptr(glm::vec3(0.2f))); 
             
             glm::mat4 orbitModel = glm::scale(posMatrix, glm::vec3(orbitRadius));
             glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr(orbitModel));
             
             glBindVertexArray(m_orbitVao);
             glDrawArrays(GL_LINE_LOOP, 0, 128);
        }
        
        // Recurse
        for (const auto& child : body.getChildren()) {
            drawBodyRecursive(*child, posMatrix);
        }
    };

    glm::mat4 identity = glm::mat4(1.0f);

    // Draw Sun and Primary Planets (Roots)
    for (const auto& body : bodies) {
        // If it's a root body (Planets/Sun), it orbits (0,0,0).
        // Draw its orbit first (if not Sun)
        if (body->getName() != "Sun") {
             float orbitRadius = body->getOrbitalParams().semiMajorAxis * visualDistanceScale;
             drawOrbit(orbitRadius, glm::vec3(0.2f, 0.2f, 0.2f), view, proj);
        }
        
        drawBodyRecursive(*body, identity);
    }
    
    glBindVertexArray(0);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(m_window.getHandle());
}

void GLRenderer::resize(int width, int height) {
    glViewport(0, 0, width, height);
}

} // namespace Render

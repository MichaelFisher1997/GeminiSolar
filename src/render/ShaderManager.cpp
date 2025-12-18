#include "ShaderManager.hpp"
#include "core/Logger.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Render {

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
    cleanup();
}

GLuint ShaderManager::loadFromSource(const std::string& name,
                                      const std::string& vertexSource,
                                      const std::string& fragmentSource) {
    // Check if already loaded
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        return it->second;
    }
    
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint program = linkProgram(vertexShader, fragmentShader);
    
    // Shaders are no longer needed after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    m_shaders[name] = program;
    LOG_INFO("ShaderManager", "Loaded shader: ", name);
    
    return program;
}

GLuint ShaderManager::loadFromFiles(const std::string& name,
                                     const std::string& vertexPath,
                                     const std::string& fragmentPath) {
    std::string vertexSource = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);
    return loadFromSource(name, vertexSource, fragmentSource);
}

GLuint ShaderManager::getShader(const std::string& name) const {
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        return it->second;
    }
    LOG_ERROR("ShaderManager", "Shader not found: ", name);
    return 0;
}

void ShaderManager::useShader(const std::string& name) {
    GLuint program = getShader(name);
    useShader(program);
}

void ShaderManager::useShader(GLuint program) {
    if (program != m_currentProgram) {
        glUseProgram(program);
        m_currentProgram = program;
    }
}

GLint ShaderManager::getUniformLocation(GLuint program, const std::string& name) {
    auto& cache = m_uniformCache[program];
    auto it = cache.find(name);
    if (it != cache.end()) {
        return it->second;
    }
    
    GLint location = glGetUniformLocation(program, name.c_str());
    cache[name] = location;
    return location;
}

void ShaderManager::cleanup() {
    for (auto& [name, program] : m_shaders) {
        glDeleteProgram(program);
    }
    m_shaders.clear();
    m_uniformCache.clear();
    m_currentProgram = 0;
}

GLuint ShaderManager::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::string shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        LOG_ERROR("ShaderManager", shaderType, " shader compilation failed: ", infoLog);
        throw std::runtime_error("Shader compilation failed");
    }
    
    return shader;
}

GLuint ShaderManager::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG_ERROR("ShaderManager", "Shader program linking failed: ", infoLog);
        throw std::runtime_error("Shader linking failed");
    }
    
    return program;
}

std::string ShaderManager::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("ShaderManager", "Failed to open shader file: ", path);
        throw std::runtime_error("Failed to open shader file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace Render

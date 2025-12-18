#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <GL/glew.h>

namespace Render {

/// Manages OpenGL shader compilation, linking, and caching
class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();
    
    /// Load shader from source strings
    GLuint loadFromSource(const std::string& name, 
                          const std::string& vertexSource, 
                          const std::string& fragmentSource);
    
    /// Load shader from files
    GLuint loadFromFiles(const std::string& name,
                         const std::string& vertexPath,
                         const std::string& fragmentPath);
    
    /// Get a loaded shader by name
    GLuint getShader(const std::string& name) const;
    
    /// Use a shader program
    void useShader(const std::string& name);
    void useShader(GLuint program);
    
    /// Get uniform location (cached)
    GLint getUniformLocation(GLuint program, const std::string& name);
    
    /// Delete all shaders
    void cleanup();

private:
    GLuint compileShader(GLenum type, const std::string& source);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
    std::string readFile(const std::string& path);
    
    std::unordered_map<std::string, GLuint> m_shaders;
    std::unordered_map<GLuint, std::unordered_map<std::string, GLint>> m_uniformCache;
    GLuint m_currentProgram = 0;
};

} // namespace Render

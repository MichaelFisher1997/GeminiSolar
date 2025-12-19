#pragma once

#include <vector>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Render {

/// Vertex data for OpenGL meshes
struct GLVertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
};

/// OpenGL mesh data - manages VAO, VBO, EBO
class GLMesh {
public:
    GLMesh(const std::vector<GLVertex>& vertices, const std::vector<uint32_t>& indices);
    ~GLMesh();
    
    // Non-copyable
    GLMesh(const GLMesh&) = delete;
    GLMesh& operator=(const GLMesh&) = delete;
    
    // Movable
    GLMesh(GLMesh&& other) noexcept;
    GLMesh& operator=(GLMesh&& other) noexcept;
    
    void bind() const;
    void unbind() const;
    void draw(GLenum mode = GL_TRIANGLES) const;
    
    uint32_t getIndexCount() const { return m_indexCount; }

private:
    void setupMesh(const std::vector<GLVertex>& vertices, const std::vector<uint32_t>& indices);
    void cleanup();
    
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    uint32_t m_indexCount = 0;
};

/// Factory for creating common mesh shapes
class MeshFactory {
public:
    /// Create a UV sphere
    static std::unique_ptr<GLMesh> createSphere(int sectorCount = 32, int stackCount = 32);
    
    /// Create a unit circle (for orbit lines)
    static std::unique_ptr<GLMesh> createCircle(int segments = 128);
    
    /// Create a simple cube
    static std::unique_ptr<GLMesh> createCube();

private:
    static constexpr double PI = 3.14159265358979323846;
};

} // namespace Render

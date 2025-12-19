#include "MeshFactory.hpp"
#include <cmath>

namespace Render {

// GLMesh implementation
GLMesh::GLMesh(const std::vector<GLVertex>& vertices, const std::vector<uint32_t>& indices) {
    setupMesh(vertices, indices);
}

GLMesh::~GLMesh() {
    cleanup();
}

GLMesh::GLMesh(GLMesh&& other) noexcept
    : m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ebo(other.m_ebo)
    , m_indexCount(other.m_indexCount) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_indexCount = 0;
}

GLMesh& GLMesh::operator=(GLMesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_indexCount = other.m_indexCount;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_indexCount = 0;
    }
    return *this;
}

void GLMesh::setupMesh(const std::vector<GLVertex>& vertices, const std::vector<uint32_t>& indices) {
    m_indexCount = static_cast<uint32_t>(indices.size());
    
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    
    glBindVertexArray(m_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLVertex), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)offsetof(GLVertex, position));
    glEnableVertexAttribArray(0);
    
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)offsetof(GLVertex, color));
    glEnableVertexAttribArray(1);
    
    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLVertex), (void*)offsetof(GLVertex, normal));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void GLMesh::cleanup() {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_ebo) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
}

void GLMesh::bind() const {
    glBindVertexArray(m_vao);
}

void GLMesh::unbind() const {
    glBindVertexArray(0);
}

void GLMesh::draw(GLenum mode) const {
    glBindVertexArray(m_vao);
    glDrawElements(mode, m_indexCount, GL_UNSIGNED_INT, 0);
}

// MeshFactory implementation
std::unique_ptr<GLMesh> MeshFactory::createSphere(int sectorCount, int stackCount) {
    std::vector<GLVertex> vertices;
    std::vector<uint32_t> indices;
    
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f;
    
    float sectorStep = 2.0f * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    
    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2.0f - i * stackStep;
        xy = std::cos(stackAngle);
        z = std::sin(stackAngle);
        
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;
            
            x = xy * std::cos(sectorAngle);
            y = xy * std::sin(sectorAngle);
            
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            
            GLVertex vertex;
            vertex.position = glm::vec3(x, z, y);  // Swap Y/Z for Y-up
            vertex.color = glm::vec3(1.0f);
            vertex.normal = glm::vec3(nx, nz, ny);
            vertices.push_back(vertex);
        }
    }
    
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;
        
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    
    return std::make_unique<GLMesh>(vertices, indices);
}

std::unique_ptr<GLMesh> MeshFactory::createCircle(int segments) {
    std::vector<GLVertex> vertices;
    std::vector<uint32_t> indices;
    
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * PI * float(i) / float(segments);
        float x = std::cos(theta);
        float z = std::sin(theta);
        
        GLVertex vertex;
        vertex.position = glm::vec3(x, 0.0f, z);
        vertex.color = glm::vec3(0.3f);
        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        vertices.push_back(vertex);
        
        if (i < segments) {
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }
    // Close the loop
    indices.push_back(segments);
    indices.push_back(0);
    
    return std::make_unique<GLMesh>(vertices, indices);
}

std::unique_ptr<GLMesh> MeshFactory::createCube() {
    std::vector<GLVertex> vertices = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, {1,1,1}, { 0, 0, 1}},
        {{ 0.5f, -0.5f,  0.5f}, {1,1,1}, { 0, 0, 1}},
        {{ 0.5f,  0.5f,  0.5f}, {1,1,1}, { 0, 0, 1}},
        {{-0.5f,  0.5f,  0.5f}, {1,1,1}, { 0, 0, 1}},
        // Back face
        {{-0.5f, -0.5f, -0.5f}, {1,1,1}, { 0, 0,-1}},
        {{ 0.5f, -0.5f, -0.5f}, {1,1,1}, { 0, 0,-1}},
        {{ 0.5f,  0.5f, -0.5f}, {1,1,1}, { 0, 0,-1}},
        {{-0.5f,  0.5f, -0.5f}, {1,1,1}, { 0, 0,-1}},
    };
    
    std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0,  // Front
        5, 4, 7, 7, 6, 5,  // Back
        4, 0, 3, 3, 7, 4,  // Left
        1, 5, 6, 6, 2, 1,  // Right
        3, 2, 6, 6, 7, 3,  // Top
        4, 5, 1, 1, 0, 4,  // Bottom
    };
    
    return std::make_unique<GLMesh>(vertices, indices);
}

} // namespace Render

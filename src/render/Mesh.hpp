#pragma once

#include <vector>
#include <memory>
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "VulkanContext.hpp"

namespace Render {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

class Mesh {
public:
    Mesh(VulkanContext& context, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();

    void draw(VkCommandBuffer commandBuffer);

    static std::unique_ptr<Mesh> createSphere(VulkanContext& context, int sectorCount, int stackCount, glm::vec3 color);

private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);
    
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VulkanContext& m_context;
    
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    uint32_t m_vertexCount;

    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;
    uint32_t m_indexCount;
};

} // namespace Render

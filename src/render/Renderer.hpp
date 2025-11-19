#pragma once

#include "VulkanContext.hpp"
#include "Swapchain.hpp"
#include "platform/SDLWindow.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Camera.hpp"
#include "Mesh.hpp"
#include "simulation/SolarSystem.hpp"
#include "RenderInterface.hpp"

namespace Render {

struct UniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class Renderer : public RenderInterface {
public:
    Renderer(VulkanContext& context, Platform::SDLWindow& window);
    ~Renderer() override;

    void render(const Simulation::SolarSystem& solarSystem, const Camera& camera, double simulationTime) override;

private:
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createDepthResources();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();
    
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const Simulation::SolarSystem& solarSystem, const Camera& camera, double simulationTime);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);

    VulkanContext& m_context;
    Platform::SDLWindow& m_window;
    std::unique_ptr<Swapchain> m_swapchain;

    VkRenderPass m_renderPass;
    VkDescriptorSetLayout m_descriptorSetLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    std::vector<VkFramebuffer> m_swapchainFramebuffers;
    
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers; // Changed to vector for multiple frames in flight ideally, but sticking to 1 for now, just cleaner.
    
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
    VkFence m_inFlightFence;

    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;
    std::vector<void*> m_uniformBuffersMapped;

    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;

    VkImage m_depthImage;
    VkDeviceMemory m_depthImageMemory;
    VkImageView m_depthImageView;

    std::unique_ptr<Mesh> m_sphereMesh;
};

} // namespace Render

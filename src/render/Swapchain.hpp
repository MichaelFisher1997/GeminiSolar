#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanContext.hpp"

namespace Render {

class Swapchain {
public:
    Swapchain(VulkanContext& context, int width, int height);
    ~Swapchain();

    void recreate(int width, int height);
    
    VkSwapchainKHR getHandle() const { return m_swapchain; }
    VkFormat getFormat() const { return m_swapchainImageFormat; }
    VkExtent2D getExtent() const { return m_swapchainExtent; }
    
    const std::vector<VkImageView>& getImageViews() const { return m_swapchainImageViews; }

private:
    void createSwapchain(int width, int height);
    void createImageViews();
    void cleanup();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);

    VulkanContext& m_context;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapchainImages;
    VkFormat m_swapchainImageFormat;
    VkExtent2D m_swapchainExtent;
    std::vector<VkImageView> m_swapchainImageViews;
};

} // namespace Render

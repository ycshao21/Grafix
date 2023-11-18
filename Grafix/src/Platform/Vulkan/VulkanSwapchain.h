#pragma once

#include "vulkan/vulkan.h"
struct GLFWwindow;

#include "Grafix/Core/Base.h"
#include "Platform/Vulkan/VulkanLogicalDevice.h"

namespace Grafix
{
    class VulkanSwapchain
    {
    public:
        VulkanSwapchain();
        ~VulkanSwapchain() = default;

        void InitSurface(GLFWwindow* windowHandle);
        // TODO: Remove this
        void BindLogicalDevice(const Shared<VulkanLogicalDevice>& logicalDevice);
        void Create(uint32_t width, uint32_t height);
        void Destroy();

        void Present() {}

        VkSurfaceKHR GetVkSurface() const { return m_Surface; }
    private:
        Shared<VulkanLogicalDevice> m_LogicalDevice = nullptr;

        VkSurfaceKHR m_Surface = nullptr;
        VkSurfaceFormatKHR m_SurfaceFormat;
        VkExtent2D m_Extent;

        int32_t m_GraphicsQueueFamilyIndex = -1;
        int32_t m_PresentQueueFamilyIndex = -1;

        VkSwapchainKHR m_Swapchain = nullptr;

        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
    };
}


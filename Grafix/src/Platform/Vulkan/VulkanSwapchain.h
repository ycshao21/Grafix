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
        VulkanSwapchain() = default;
        ~VulkanSwapchain() = default;

        void InitSurface(GLFWwindow* windowHandle);
        // TODO: Remove this
        void BindLogicalDevice(const Shared<VulkanLogicalDevice>& logicalDevice);
        void Create(uint32_t width, uint32_t height);
        void Destroy();

        void Resize(uint32_t width, uint32_t height);

        void BeginFrame();
        void Present();

        VkSwapchainKHR GetVkSwapchain() const { return m_Swapchain; }
        VkSurfaceKHR GetVkSurface() const { return m_Surface; }
        const VkExtent2D& GetExtent() const { return m_Extent; }
        ////const VkSurfaceFormatKHR& GetSurfaceFormat() const { return m_SurfaceFormat; }
        uint32_t GetGraphicsQueueFamilyIndex() const { return (uint32_t)m_GraphicsQueueFamilyIndex; }

        const std::vector<VkImageView>& GetImageViews() { return m_ImageViews; }

        VkRenderPass GetVkRenderPass() const { return m_RenderPass; }

        VkCommandBuffer GetCurrentCommandBuffer() const { return m_CommandBuffers[m_CurrentBufferIndex]; }
        VkSemaphore GetCurrentImageAvailableSemaphore() const { return m_ImageAvailableSemaphores[m_CurrentBufferIndex]; }
        VkSemaphore GetCurrentRenderFinishedSemaphore() const { return m_RenderFinishedSemaphores[m_CurrentBufferIndex]; }
        VkFence GetCurrentInFlightFence() const { return m_InFlightFences[m_CurrentBufferIndex]; }
        VkFramebuffer GetCurrentFramebuffer() const { return m_Framebuffers[m_CurrentImageIndex]; }
    private:
        Shared<VulkanLogicalDevice> m_LogicalDevice = nullptr;

        uint32_t m_Width, m_Height;
        VkSurfaceKHR m_Surface = nullptr;
        VkSurfaceFormatKHR m_SurfaceFormat;
        VkExtent2D m_Extent;

        int32_t m_GraphicsQueueFamilyIndex = -1;
        int32_t m_PresentQueueFamilyIndex = -1;

        VkSwapchainKHR m_Swapchain = nullptr;

        // Images and image views
        uint32_t m_ImageCount = 0;
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;

        VkRenderPass m_RenderPass;

        std::vector<VkFramebuffer> m_Framebuffers;

        VkCommandPool m_CommandPool;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        const uint32_t m_MaxFramesInFlight = 2;
        uint32_t m_CurrentBufferIndex = 0;
        uint32_t m_CurrentImageIndex = 0;  // Unused

        std::vector<VkSemaphore> m_ImageAvailableSemaphores, m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
    };
}


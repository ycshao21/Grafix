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
        VulkanSwapchain(const Shared<VulkanLogicalDevice>& logicalDevice);
        ~VulkanSwapchain() = default;

        void InitSurface(GLFWwindow* windowHandle);
        void Create(uint32_t* width, uint32_t* height, bool vsync);
        void Destroy();

        void Present() {}
    private:
        bool m_Vsync = true;
        Shared<VulkanLogicalDevice> m_LogicalDevice = nullptr;

        VkSurfaceKHR m_Surface = nullptr;
        uint32_t m_GraphicsQueueFamilyIndex, m_PresentQueueFamilyIndex;
        VkSurfaceFormatKHR m_SurfaceFormat;

        VkSwapchainKHR m_Swapchain = nullptr;
    };
}


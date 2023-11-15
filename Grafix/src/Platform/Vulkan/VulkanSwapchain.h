#pragma once

#include "vulkan/vulkan.h"

namespace Grafix
{
    class VulkanSwapchain
    {
    public:
        VulkanSwapchain();
        ~VulkanSwapchain();

        void Init();
        void InitSurface();
    private:
        VkSurfaceKHR m_Surface = nullptr;
    };
}


#pragma once

#include "vulkan/vulkan.h"

namespace Grafix
{
    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice();
        ~VulkanPhysicalDevice();

        VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

        static Shared<VulkanPhysicalDevice> Pick();
    private:
        VkPhysicalDevice m_PhysicalDevice = nullptr;
    };
}


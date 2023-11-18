#pragma once

#include "vulkan/vulkan.h"

namespace Grafix
{
    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice();
        ~VulkanPhysicalDevice();

        VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
        bool IsExtensionSupported(const std::string& extensionName) const;

        static Shared<VulkanPhysicalDevice> Pick();
    private:
        // Physical device
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        std::set<std::string> m_SupportedExtensions;
    };
}


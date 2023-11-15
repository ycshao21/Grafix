#pragma once

#include "vulkan/vulkan.h"

namespace Grafix
{
    struct QueueFamilyIndices
    {
        int32_t GraphicsFamily = -1;
        int32_t ComputeFamily = -1;
        int32_t TransferFamily = -1;
    };

    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice();
        ~VulkanPhysicalDevice();

        VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

        bool IsExtensionSupported(const std::string& extensionName) const;

        const std::vector<VkDeviceQueueCreateInfo>& GetQueueCreateInfos() const { return m_QueueCreateInfos; }
        const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }


        static Shared<VulkanPhysicalDevice> Pick();
    private:
        QueueFamilyIndices GetQueueFamilyIndices(int queueFlags) const;
    private:
        // Physical device
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        VkPhysicalDeviceProperties m_Properties;
        std::set<std::string> m_SupportedExtensions;

        // Queue family indices
        std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;
        std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
        QueueFamilyIndices m_QueueFamilyIndices;
    };
}


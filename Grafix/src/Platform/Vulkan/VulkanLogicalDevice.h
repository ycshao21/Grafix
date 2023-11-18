#pragma once

#include "Grafix/Core/Base.h"
#include "Platform/Vulkan/VulkanPhysicalDevice.h"

namespace Grafix
{
    struct QueueFamilyIndices
    {
        int32_t GraphicsFamily = -1;
        int32_t PresentFamily = -1;
    };

    class VulkanLogicalDevice
    {
    public:
        VulkanLogicalDevice(const Shared<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
        ~VulkanLogicalDevice();

        void Destroy();

        VkDevice GetVkDevice() const { return m_LogicalDevice; }
        Shared<VulkanPhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }

        static Shared<VulkanLogicalDevice> Create(const Shared<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
    private:
        // TODO: Remove this, this is already done in VulkanSwapchain::InitSurface();
        QueueFamilyIndices FindQueueFamilyIndices() const;
    private:
        VkDevice m_LogicalDevice = nullptr;
        Shared<VulkanPhysicalDevice> m_PhysicalDevice = nullptr;

        VkQueue m_GraphicsQueue = nullptr;
        VkQueue m_PresentQueue = nullptr;
    };
}


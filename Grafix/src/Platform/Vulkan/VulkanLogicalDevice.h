#pragma once

#include "Grafix/Core/Base.h"
#include "Platform/Vulkan/VulkanPhysicalDevice.h"

namespace Grafix
{
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
        VkDevice m_LogicalDevice = nullptr;
        Shared<VulkanPhysicalDevice> m_PhysicalDevice = nullptr;

        VkQueue m_GraphicsQueue = nullptr;
        VkQueue m_ComputeQueue = nullptr;
    };
}


#include "pch.h"
#include "VulkanLogicalDevice.h"

namespace Grafix
{
    VulkanLogicalDevice::VulkanLogicalDevice(const Shared<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
        : m_PhysicalDevice(physicalDevice)
    {
        GF_CORE_ASSERT(m_PhysicalDevice->IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "Swapchain extension is not supported!");
        std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkDeviceCreateInfo deviceCI{};
        deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCI.pEnabledFeatures = &enabledFeatures;
        deviceCI.enabledExtensionCount = (uint32_t)extensions.size();
        deviceCI.ppEnabledExtensionNames = extensions.data();
        deviceCI.queueCreateInfoCount = (uint32_t)m_PhysicalDevice->GetQueueCreateInfos().size();
        deviceCI.pQueueCreateInfos = m_PhysicalDevice->GetQueueCreateInfos().data();
        VK_CHECK(vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &deviceCI, nullptr, &m_LogicalDevice));

        vkGetDeviceQueue(m_LogicalDevice, m_PhysicalDevice->GetQueueFamilyIndices().GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicalDevice, m_PhysicalDevice->GetQueueFamilyIndices().ComputeFamily, 0, &m_ComputeQueue);
    }

    VulkanLogicalDevice::~VulkanLogicalDevice()
    {
        // Nothing to do here.
        // Destroy() must be called explicitly.
    }

    void VulkanLogicalDevice::Destroy()
    {
        vkDestroyDevice(m_LogicalDevice, nullptr);
    }

    Shared<VulkanLogicalDevice> VulkanLogicalDevice::Create(const Shared<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
    {
        return CreateShared<VulkanLogicalDevice>(physicalDevice, enabledFeatures);
    }
}

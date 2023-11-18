#include "pch.h"
#include "VulkanLogicalDevice.h"

#include "VulkanContext.h"

namespace Grafix
{
    VulkanLogicalDevice::VulkanLogicalDevice(const Shared<VulkanPhysicalDevice>& physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
        : m_PhysicalDevice(physicalDevice)
    {
        GF_CORE_ASSERT(m_PhysicalDevice->IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME), "Swapchain extension is not supported!");
        std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        // -----------------------------------------------------------------------------------------------------------------------------------------------------------
        // Queue families
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------

        QueueFamilyIndices queueFamilyIndices = FindQueueFamilyIndices();
        std::set<int32_t> uniqueQueueFamilies = { queueFamilyIndices.GraphicsFamily, queueFamilyIndices.PresentFamily };

        float queuePriority = 1.0f;

        std::vector<VkDeviceQueueCreateInfo> queueCIs;
        for (auto& queueFamilyIndex : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCI{};
            queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCI.queueFamilyIndex = queueFamilyIndex;
            queueCI.queueCount = 1;
            queueCI.pQueuePriorities = &queuePriority;
            queueCIs.push_back(queueCI);
        }

        VkDeviceCreateInfo deviceCI{};
        deviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCI.pEnabledFeatures = &enabledFeatures;
        deviceCI.pQueueCreateInfos = queueCIs.data();
        deviceCI.queueCreateInfoCount = (uint32_t)queueCIs.size();
        deviceCI.enabledExtensionCount = (uint32_t)extensions.size();
        deviceCI.ppEnabledExtensionNames = extensions.data();
        VK_CHECK(vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &deviceCI, nullptr, &m_LogicalDevice));

        vkGetDeviceQueue(m_LogicalDevice, queueFamilyIndices.GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicalDevice, queueFamilyIndices.PresentFamily, 0, &m_PresentQueue);
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

    QueueFamilyIndices VulkanLogicalDevice::FindQueueFamilyIndices() const
    {
        auto physicalDevice = m_PhysicalDevice->GetVkPhysicalDevice();

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        GF_CORE_ASSERT(queueFamilyCount > 0, "Physical device has no queue families!");
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        QueueFamilyIndices queueFamilyIndices;
        bool found = false;
        for (int i = 0; i < queueFamilyCount; i++)
        {
            if(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                queueFamilyIndices.GraphicsFamily = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, VulkanContext::Get().GetSwapchain()->GetVkSurface(), &presentSupport);
            if (presentSupport)
                queueFamilyIndices.PresentFamily = i;

            found = queueFamilyIndices.GraphicsFamily != -1 && queueFamilyIndices.PresentFamily != -1;
            if (found)
                break;
        }

        GF_CORE_ASSERT(found, "Physical device does not support required queue families!");
        return queueFamilyIndices;
    }
}

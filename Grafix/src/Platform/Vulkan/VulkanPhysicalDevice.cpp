#include "pch.h"
#include "VulkanPhysicalDevice.h"

#include "VulkanContext.h"

namespace Grafix
{
    VulkanPhysicalDevice::VulkanPhysicalDevice()
        : m_PhysicalDevice(nullptr)
    {
        auto instance = VulkanContext::GetInstance();

        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
        GF_CORE_ASSERT(physicalDeviceCount > 0, "Failed to find GPUs with Vulkan support!");
        GF_CORE_DEBUG("Found {0} physical device(s) with Vulkan support.", physicalDeviceCount);
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

        // Pick the first suitable device.
        for (auto physicalDevice : physicalDevices)
        {
            vkGetPhysicalDeviceProperties(physicalDevice, &m_Properties);
            if(m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                m_PhysicalDevice = physicalDevice;
                break;
            }
        }

        GF_CORE_ASSERT(m_PhysicalDevice, "Failed to find a suitable physical device!");
        GF_CORE_INFO("Picked physical device: {0}", m_Properties.deviceName);

        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);
        GF_CORE_ASSERT(extensionCount > 0, "Physical device has no supported extensions!");

        std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, supportedExtensions.data()));
        for (auto& extension : supportedExtensions)
            m_SupportedExtensions.emplace(extension.extensionName);

        // -----------------------------------------------------------------------------------------------------------------------------------------------------------
        // Queue families
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
        GF_CORE_ASSERT(queueFamilyCount > 0, "Physical device has no queue families!");
        m_QueueFamilyProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

        m_QueueFamilyIndices = GetQueueFamilyIndices(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);

        constexpr const float queuePriority = 0.0f;

        // Graphics queue
        VkDeviceQueueCreateInfo graphicsQueueCI = {};
        graphicsQueueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCI.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily;
        graphicsQueueCI.queueCount = 1;
        graphicsQueueCI.pQueuePriorities = &queuePriority;
        m_QueueCreateInfos.push_back(graphicsQueueCI);

        // Compute queue
        // If compute family is not the same as graphics family, then we need to create another queue.
        if (m_QueueFamilyIndices.ComputeFamily != m_QueueFamilyIndices.GraphicsFamily)
        {
            VkDeviceQueueCreateInfo computeQueueCI = {};
            computeQueueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            computeQueueCI.queueFamilyIndex = m_QueueFamilyIndices.ComputeFamily;
            computeQueueCI.queueCount = 1;
            computeQueueCI.pQueuePriorities = &queuePriority;
            m_QueueCreateInfos.push_back(computeQueueCI);
        }

        // Transfer queue
        // If transfer family is not the same as graphics family or compute family, then we need to create another queue.
        if (m_QueueFamilyIndices.TransferFamily != m_QueueFamilyIndices.GraphicsFamily
            && m_QueueFamilyIndices.TransferFamily != m_QueueFamilyIndices.ComputeFamily)
        {
            VkDeviceQueueCreateInfo transferQueueCI = {};
            transferQueueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            transferQueueCI.queueFamilyIndex = m_QueueFamilyIndices.TransferFamily;
            transferQueueCI.queueCount = 1;
            transferQueueCI.pQueuePriorities = &queuePriority;
            m_QueueCreateInfos.push_back(transferQueueCI);
        }
    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice()
    {
        // Nothing to do here.
        // Physical device is destroyed when the instance is destroyed.
    }

    bool VulkanPhysicalDevice::IsExtensionSupported(const std::string& extensionName) const
    {
        return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
    }

    Shared<VulkanPhysicalDevice> VulkanPhysicalDevice::Pick()
    {
        return CreateShared<VulkanPhysicalDevice>();
    }

    QueueFamilyIndices VulkanPhysicalDevice::GetQueueFamilyIndices(int queueFlags) const
    {
        QueueFamilyIndices indices;

        // Find a queue family that supports compute but not graphics.
        if (queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            for (uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++)
            {
                if ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                    && !(m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    indices.ComputeFamily = i;
                    break;
                }
            }
        }

        // Find a queue family that supports transfer but not graphics or compute.
        if (queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            for (uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++)
            {
                if ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                    && !(m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    && !(m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
                {
                    indices.TransferFamily = i;
                    break;
                }
            }
        }

        for (uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++)
        {
            if ((queueFlags & VK_QUEUE_COMPUTE_BIT) && indices.ComputeFamily == -1)
            {
                if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                    indices.ComputeFamily = i;
            }

            if ((queueFlags & VK_QUEUE_TRANSFER_BIT) && indices.TransferFamily == -1)
            {
                if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                    indices.TransferFamily = i;
            }

            if ((queueFlags & VK_QUEUE_GRAPHICS_BIT) && indices.GraphicsFamily == -1)
            {
                if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    indices.GraphicsFamily = i;
            }
        }

        return indices;
    }
}

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
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;

        m_PhysicalDevice = physicalDevices[0];
        for (auto physicalDevice : physicalDevices)
        {
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);
            vkGetPhysicalDeviceFeatures(physicalDevice, &features);
            if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader)
            {
                m_PhysicalDevice = physicalDevice;
                break;
            }
        }
        GF_CORE_ASSERT(m_PhysicalDevice, "Failed to find a suitable physical device!");
        GF_CORE_INFO("Picked physical device: {0}", properties.deviceName);

        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);
        GF_CORE_ASSERT(extensionCount > 0, "Physical device has no supported extensions!");

        std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, supportedExtensions.data()));
        for (auto& extension : supportedExtensions)
            m_SupportedExtensions.emplace(extension.extensionName);
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
}

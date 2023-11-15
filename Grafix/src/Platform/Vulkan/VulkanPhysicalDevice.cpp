#include "pch.h"
#include "VulkanPhysicalDevice.h"

#include "VulkanContext.h"

namespace Grafix
{
    constexpr static const char* GetVulkanPhysicalDeviceTypeString(VkPhysicalDeviceType type)
    {
        switch (type)
        {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:          return "Other"; 
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU"; 
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "Discrete GPU";
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "Virtual GPU";
            case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "CPU";
        }
    }

    static bool IsPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice)
    {
        std::set<std::string> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; 

        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        for (auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        // If the set is empty,
        // then all required extensions are in the available extensions list.
        return requiredExtensions.empty();
    }

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
            if (IsPhysicalDeviceSuitable(physicalDevice))
            {
                m_PhysicalDevice = physicalDevice;
                break;
            }
        }

        VkPhysicalDeviceProperties deviceProps;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProps);
        GF_CORE_INFO("Picked physical device: {0}", deviceProps.deviceName);
        GF_CORE_INFO("                  Type: {0}", GetVulkanPhysicalDeviceTypeString(deviceProps.deviceType));

    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice()
    {
    }

    Shared<VulkanPhysicalDevice> VulkanPhysicalDevice::Pick()
    {
        return CreateShared<VulkanPhysicalDevice>();
    }
}

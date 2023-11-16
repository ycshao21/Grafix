#include "pch.h"
#include "VulkanSwapchain.h"

#include "VulkanContext.h"
#include "Grafix/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Grafix
{
    VulkanSwapchain::VulkanSwapchain(const Shared<VulkanLogicalDevice>& logicalDevice)
        : m_LogicalDevice(logicalDevice)
    {
    }

    void VulkanSwapchain::InitSurface(GLFWwindow* windowHandle)
    {
        auto physicalDevice = m_LogicalDevice->GetPhysicalDevice()->GetVkPhysicalDevice();

        VK_CHECK(glfwCreateWindowSurface(VulkanContext::GetInstance(), windowHandle, nullptr, &m_Surface));

        // Get queue family indices
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        GF_CORE_ASSERT(queueFamilyCount > 0, "No queue families found!");
        std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProps.data());

        std::vector<VkBool32> presentSupports(queueFamilyCount);
        uint32_t graphicsQueueFamilyIndex = std::numeric_limits<uint32_t>::max();
        uint32_t presentQueueFamilyIndex = std::numeric_limits<uint32_t>::max();

        // Find a queue family that supports both graphics and present.
        for(uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &presentSupports[i]);
            if(queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                if (presentSupports[i] == VK_TRUE)  // Found
                {
                    graphicsQueueFamilyIndex = i;
                    presentQueueFamilyIndex = i;
                    break;
                }

                if (graphicsQueueFamilyIndex == std::numeric_limits<uint32_t>::max())
                    graphicsQueueFamilyIndex = i;
            }
        }

        // If no queue family supports both graphics and present,
        // then find a separate present queue family.
        if(presentQueueFamilyIndex == std::numeric_limits<uint32_t>::max())
        {
            for(uint32_t i = 0; i < queueFamilyCount; ++i)
            {
                if(presentSupports[i] == VK_TRUE)
                {
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }

        GF_CORE_ASSERT(graphicsQueueFamilyIndex != std::numeric_limits<uint32_t>::max(), "No graphics queue family found!");
        GF_CORE_ASSERT(presentQueueFamilyIndex != std::numeric_limits<uint32_t>::max(), "No present queue family found!");

        m_GraphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
        m_PresentQueueFamilyIndex = presentQueueFamilyIndex;

        // Get surface format
        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr);
        GF_CORE_ASSERT(formatCount > 0, "No surface formats found!");
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceFormats.data());

        m_SurfaceFormat = surfaceFormats[0];  // Default to first format
        for(auto& format : surfaceFormats)
        {
            if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                m_SurfaceFormat = format;
                break;
            }
        }
    }

    void VulkanSwapchain::Create(uint32_t* width, uint32_t* height, bool vsync)
    {
        m_Vsync = vsync;

        auto device = m_LogicalDevice->GetVkDevice();
        auto physicalDevice = m_LogicalDevice->GetPhysicalDevice()->GetVkPhysicalDevice();

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCapabilities);

        uint32_t swapchainImageCount = surfaceCapabilities.minImageCount + 1;
        swapchainImageCount = std::clamp(swapchainImageCount, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

        // Extent
        VkExtent2D swapchainExtent{};
        if(surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()
            || surfaceCapabilities.currentExtent.height == std::numeric_limits<uint32_t>::max())
        {
            // If current extent is at numeric limits,
            // then extent can be set to any value within the min/max bounds.
            swapchainExtent.width = *width;
            swapchainExtent.height = *height;
        }
        else
        {
            // Otherwise, the size is defined by the extent.
            swapchainExtent = surfaceCapabilities.currentExtent;
            *width = surfaceCapabilities.currentExtent.width;
            *height = surfaceCapabilities.currentExtent.height;
        }

        // Pre transform
        VkSurfaceTransformFlagBitsKHR preTransform = surfaceCapabilities.currentTransform;

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr);
        GF_CORE_ASSERT(presentModeCount > 0, "No present modes found!");
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data());

        VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;  // Default to FIFO
        if (!vsync)
        {
            for(auto& mode : presentModes)
            {
                if(mode == VK_PRESENT_MODE_MAILBOX_KHR)  // Best mode
                {
                    chosenPresentMode = mode;
                    break;
                }
            }
        }

        VkSwapchainKHR oldSwapchain = m_Swapchain;

        VkSwapchainCreateInfoKHR swapchainCI{};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.clipped = VK_TRUE;
        swapchainCI.imageArrayLayers = 1;
        swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCI.surface = m_Surface;
        swapchainCI.imageExtent = swapchainExtent;
        swapchainCI.minImageCount = swapchainImageCount;
        swapchainCI.imageFormat = m_SurfaceFormat.format;
        swapchainCI.imageColorSpace = m_SurfaceFormat.colorSpace;
        swapchainCI.preTransform = preTransform;
        swapchainCI.presentMode = chosenPresentMode;
        swapchainCI.oldSwapchain = oldSwapchain;
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.pQueueFamilyIndices = nullptr;
        swapchainCI.queueFamilyIndexCount = 0;
        VK_CHECK(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &m_Swapchain));
        GF_CORE_INFO("Swapchain created.");

        if(oldSwapchain)
            vkDestroySwapchainKHR(device, oldSwapchain, nullptr);
    }

    void VulkanSwapchain::Destroy()
    {
        auto device = m_LogicalDevice->GetVkDevice();
        vkDestroySwapchainKHR(device, m_Swapchain, nullptr);

        vkDestroySurfaceKHR(VulkanContext::GetInstance(), m_Surface, nullptr);
    }
}

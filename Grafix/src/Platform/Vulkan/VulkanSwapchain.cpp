#include "pch.h"
#include "VulkanSwapchain.h"

#include "VulkanContext.h"
#include "Grafix/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Grafix
{
    static std::string PresentModeString(VkPresentModeKHR presentMode)
    {
        switch (presentMode)
        {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:     return "Immediate";
            case VK_PRESENT_MODE_MAILBOX_KHR:       return "Mailbox";
            case VK_PRESENT_MODE_FIFO_KHR:          return "FIFO";
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:  return "FIFO Relaxed";
        }

        GF_CORE_ASSERT(false, "Unknown present mode!");
        return "";
    }

    VulkanSwapchain::VulkanSwapchain()
    {
    }

    void VulkanSwapchain::InitSurface(GLFWwindow* windowHandle)
    {
        // TODO: Make it like this
        ////auto physicalDevice = m_LogicalDevice->GetPhysicalDevice()->GetVkPhysicalDevice();
        auto physicalDevice = VulkanContext::Get().GetPhysicalDevice()->GetVkPhysicalDevice();
        VK_CHECK(glfwCreateWindowSurface(VulkanContext::GetInstance(), windowHandle, nullptr, &m_Surface));

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

        // -----------------------------------------------------------------------------------------------------------------------------------------------------------
        // Queue families
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        GF_CORE_ASSERT(queueFamilyCount > 0, "Physical device has no queue families!");
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        bool found = false;
        for (int i = 0; i < queueFamilyCount; i++)
        {
            if(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                m_GraphicsQueueFamilyIndex = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, VulkanContext::Get().GetSwapchain()->GetVkSurface(), &presentSupport);
            if (presentSupport)
                m_PresentQueueFamilyIndex = i;

            found = m_GraphicsQueueFamilyIndex  != -1 && m_PresentQueueFamilyIndex  != -1;
            if (found)
                break;
        }
        GF_CORE_ASSERT(found, "Physical device does not support required queue families!");
    }

    void VulkanSwapchain::BindLogicalDevice(const Shared<VulkanLogicalDevice>& logicalDevice)
    {
        m_LogicalDevice = logicalDevice;
    }

    void VulkanSwapchain::Create(uint32_t width, uint32_t height)
    {
        auto device = m_LogicalDevice->GetVkDevice();
        auto physicalDevice = m_LogicalDevice->GetPhysicalDevice()->GetVkPhysicalDevice();
        auto window = (GLFWwindow*)Application::Get().GetWindow().GetNative();

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCapabilities);

        // Present mode
        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr);
        GF_CORE_ASSERT(presentModeCount > 0, "No present modes found!");
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data());

        VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;  // Default to FIFO
        for(auto& mode : presentModes)
        {
            if(mode == VK_PRESENT_MODE_MAILBOX_KHR)  // Best mode
            {
                chosenPresentMode = mode;
                break;
            }
        }
        GF_CORE_INFO("Chosen present mode: {0}", PresentModeString(chosenPresentMode));

        // Extent
        VkExtent2D swapchainExtent{};
        if (surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            swapchainExtent.width = std::clamp((uint32_t)width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
            swapchainExtent.height = std::clamp((uint32_t)height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        }
        else
        {
            swapchainExtent = surfaceCapabilities.currentExtent;
        }
        m_Extent = std::move(swapchainExtent);

        // Image count
        uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
            imageCount = surfaceCapabilities.maxImageCount;

        // Pre transform
        VkSurfaceTransformFlagBitsKHR preTransform = surfaceCapabilities.currentTransform;

        VkSwapchainCreateInfoKHR swapchainCI{};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.surface = m_Surface;

        swapchainCI.minImageCount = imageCount;
        swapchainCI.imageFormat = m_SurfaceFormat.format;
        swapchainCI.imageColorSpace = m_SurfaceFormat.colorSpace;
        swapchainCI.imageExtent = m_Extent;
        swapchainCI.imageArrayLayers = 1;
        swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (m_GraphicsQueueFamilyIndex != m_PresentQueueFamilyIndex)
        {
            uint32_t queueFamilyIndices[] = { (uint32_t)m_GraphicsQueueFamilyIndex, (uint32_t)m_PresentQueueFamilyIndex };
            swapchainCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCI.queueFamilyIndexCount = 2;
            swapchainCI.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCI.queueFamilyIndexCount = 0;
            swapchainCI.pQueueFamilyIndices = nullptr;
        }

        swapchainCI.preTransform = preTransform;

        swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        swapchainCI.presentMode = chosenPresentMode;
        swapchainCI.clipped = VK_TRUE;

        swapchainCI.oldSwapchain = VK_NULL_HANDLE;  // This is for resizing, but we set it to null for now.

        VK_CHECK(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &m_Swapchain));

        // -----------------------------------------------------------------------------------------------------------------------------------------------------------
        // Images
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------

        vkGetSwapchainImagesKHR(device, m_Swapchain, &imageCount, nullptr);
        m_SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, m_Swapchain, &imageCount, m_SwapchainImages.data());

        // -----------------------------------------------------------------------------------------------------------------------------------------------------------
        // Image views
        // -----------------------------------------------------------------------------------------------------------------------------------------------------------

        m_SwapchainImageViews.resize(imageCount);
        for (uint32_t i = 0; i < imageCount; i++)
        {
            VkImageViewCreateInfo imageViewCI{};
            imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCI.image = m_SwapchainImages[i];

            imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCI.format = m_SurfaceFormat.format;

            imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCI.subresourceRange.baseMipLevel = 0;
            imageViewCI.subresourceRange.levelCount = 1;
            imageViewCI.subresourceRange.baseArrayLayer = 0;
            imageViewCI.subresourceRange.layerCount = 1;

            VK_CHECK(vkCreateImageView(device, &imageViewCI, nullptr, &m_SwapchainImageViews[i]));
        }
    }

    void VulkanSwapchain::Destroy()
    {
        auto device = m_LogicalDevice->GetVkDevice();

        for(auto imageView : m_SwapchainImageViews)
            vkDestroyImageView(device, imageView, nullptr);

        vkDestroySwapchainKHR(device, m_Swapchain, nullptr);

        vkDestroySurfaceKHR(VulkanContext::GetInstance(), m_Surface, nullptr);
    }
}

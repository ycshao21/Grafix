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

    void VulkanSwapchain::InitSurface(GLFWwindow* windowHandle)
    {
        // TODO: Make it like this. (Logical device has not been created yet.)
        //auto physicalDevice = m_LogicalDevice->GetPhysicalDevice()->GetVkPhysicalDevice();
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Queue families
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        GF_CORE_ASSERT(queueFamilyCount > 0, "Physical device has no queue families!");
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        bool found = false;
        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                m_GraphicsQueueFamilyIndex = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &presentSupport);
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
        auto physicalDevice = m_LogicalDevice->GetPhysicalDevice()->GetVkPhysicalDevice();
        auto device = m_LogicalDevice->GetVkDevice();

        // Destroy old resources
        if (m_Swapchain)
        {
            for(auto imageView : m_ImageViews)
                vkDestroyImageView(device, imageView, nullptr);

            for(auto framebuffer : m_Framebuffers)
                vkDestroyFramebuffer(device, framebuffer, nullptr);

            vkDestroySwapchainKHR(device, m_Swapchain, nullptr);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Swapchain
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

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
            auto window = (GLFWwindow*)Application::Get().GetWindow().GetNative();
            glfwGetFramebufferSize(window, (int*)&width, (int*)&height);
            swapchainExtent.width = std::clamp((uint32_t)width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
            swapchainExtent.height = std::clamp((uint32_t)height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        }
        else
        {
            swapchainExtent = surfaceCapabilities.currentExtent;
        }
        m_Extent = std::move(swapchainExtent);

        if(width == 0 || height == 0)
            return;

        // Image count
        m_ImageCount = surfaceCapabilities.minImageCount + 1;
        if (surfaceCapabilities.maxImageCount > 0 && m_ImageCount > surfaceCapabilities.maxImageCount)
            m_ImageCount = surfaceCapabilities.maxImageCount;

        // Pre transform
        VkSurfaceTransformFlagBitsKHR preTransform = surfaceCapabilities.currentTransform;

        VkSwapchainCreateInfoKHR swapchainCI{};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.surface = m_Surface;

        swapchainCI.minImageCount = m_ImageCount;
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Images
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        vkGetSwapchainImagesKHR(device, m_Swapchain, &m_ImageCount, nullptr);
        m_Images.resize(m_ImageCount);
        vkGetSwapchainImagesKHR(device, m_Swapchain, &m_ImageCount, m_Images.data());

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Image views
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        m_ImageViews.resize(m_ImageCount);
        for (uint32_t i = 0; i < m_ImageCount; i++)
        {
            VkImageViewCreateInfo imageViewCI{};
            imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCI.image = m_Images[i];

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

            VK_CHECK(vkCreateImageView(device, &imageViewCI, nullptr, &m_ImageViews[i]));
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Render pass
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_SurfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;  // Clear the values to a constant at the start
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;  // Rendered contents will be stored in memory and can be read later
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // We are not using it for now.
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;  // We are not using it for now.
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Subpass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;

        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;

        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Render pass
        VkRenderPassCreateInfo renderPassCI{};
        renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCI.attachmentCount = 1;
        renderPassCI.pAttachments = &colorAttachment;
        renderPassCI.subpassCount = 1;
        renderPassCI.pSubpasses = &subpass;
        renderPassCI.dependencyCount = 1;
        renderPassCI.pDependencies = &dependency;

        VK_CHECK(vkCreateRenderPass(device, &renderPassCI, nullptr, &m_RenderPass));

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Framebuffer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        m_Framebuffers.resize(m_ImageViews.size());
        for (int i = 0; i < m_ImageViews.size(); ++i)
        {
            std::vector<VkImageView> attachments = { m_ImageViews[i] };

            VkFramebufferCreateInfo framebufferCI{};
            framebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCI.renderPass = m_RenderPass;
            framebufferCI.attachmentCount = (uint32_t)attachments.size();
            framebufferCI.pAttachments = attachments.data();
            framebufferCI.width = m_Extent.width;
            framebufferCI.height = m_Extent.height;
            framebufferCI.layers = 1;
            VK_CHECK(vkCreateFramebuffer(device, &framebufferCI, nullptr, &m_Framebuffers[i]));
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Command pool
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        VkCommandPoolCreateInfo poolCI{};
        poolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  // Allow command buffers to be rerecorded individually
        poolCI.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
        VK_CHECK(vkCreateCommandPool(device, &poolCI, nullptr, &m_CommandPool));

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Command buffers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        m_CommandBuffers.resize(m_MaxFramesInFlight);

        VkCommandBufferAllocateInfo commandBufferAI{};
        commandBufferAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAI.commandPool = m_CommandPool;
        commandBufferAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;  // Can be submitted to a queue for execution, but cannot be called from other command buffers
        commandBufferAI.commandBufferCount = (uint32_t)m_CommandBuffers.size();
        VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAI, m_CommandBuffers.data()));

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Sync objects
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        m_ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
        m_RenderFinishedSemaphores.resize(m_MaxFramesInFlight);
        m_InFlightFences.resize(m_MaxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreCI{};
        semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCI{};
        fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < m_MaxFramesInFlight; ++i)
        {
            VK_CHECK(vkCreateSemaphore(device, &semaphoreCI, nullptr, &m_ImageAvailableSemaphores[i]));
            VK_CHECK(vkCreateSemaphore(device, &semaphoreCI, nullptr, &m_RenderFinishedSemaphores[i]));
            VK_CHECK(vkCreateFence(device, &fenceCI, nullptr, &m_InFlightFences[i]));
        }
    }

    void VulkanSwapchain::Destroy()
    {
        auto device = m_LogicalDevice->GetVkDevice();

        // Destroy sync objects
        for (int i = 0; i < m_MaxFramesInFlight; ++i)
        {
            vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, m_InFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device, m_CommandPool, nullptr);

        for(auto framebuffer : m_Framebuffers)
            vkDestroyFramebuffer(device, framebuffer, nullptr);

        for(auto imageView : m_ImageViews)
            vkDestroyImageView(device, imageView, nullptr);

        vkDestroyRenderPass(device, m_RenderPass, nullptr);

        vkDestroySwapchainKHR(device, m_Swapchain, nullptr);

        vkDestroySurfaceKHR(VulkanContext::GetInstance(), m_Surface, nullptr);
    }

    void VulkanSwapchain::BeginFrame()
    {
        auto device = m_LogicalDevice->GetVkDevice();
        VK_CHECK(vkWaitForFences(device, 1, &m_InFlightFences[m_CurrentBufferIndex], VK_TRUE, UINT64_MAX));

        VkResult result = vkAcquireNextImageKHR(device, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentBufferIndex], VK_NULL_HANDLE, &m_CurrentImageIndex);
        VK_CHECK(result);

        VK_CHECK(vkResetFences(device, 1, &m_InFlightFences[m_CurrentBufferIndex]));
        VK_CHECK(vkResetCommandBuffer(m_CommandBuffers[m_CurrentBufferIndex], 0));
    }

    void VulkanSwapchain::Present()
    {
        // Submit

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentBufferIndex] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        VkCommandBuffer commandBuffers[] = { m_CommandBuffers[m_CurrentBufferIndex]};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers;

        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentBufferIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VK_CHECK(vkQueueSubmit(m_LogicalDevice->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentBufferIndex]));

        // Present

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_CurrentImageIndex;
        presentInfo.pResults = nullptr;
        VkResult result = vkQueuePresentKHR(m_LogicalDevice->GetPresentQueue(), &presentInfo);
        VK_CHECK(result);

        m_CurrentBufferIndex = (m_CurrentBufferIndex + 1) % m_MaxFramesInFlight;
    }
}

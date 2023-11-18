#pragma once

#include "Grafix/Renderer/RendererContext.h"

#include "vulkan/vulkan.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"

// Put swapchain here?
#include "VulkanSwapchain.h"

struct GLFWwindow;

namespace Grafix
{
    class VulkanContext : public RendererContext
    {
    public:
        VulkanContext(GLFWwindow* windowHandle);
        virtual ~VulkanContext();

        virtual void Init() override;
        virtual void SwapBuffers() override;

        static VulkanContext& Get();
        static VkInstance GetInstance() { return s_Instance; }
        Shared<VulkanPhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }
        Shared<VulkanLogicalDevice> GetLogicalDevice() const { return m_LogicalDevice; }
        Shared<VulkanSwapchain> GetSwapchain() const { return m_Swapchain; }
    private:
        void CreateInstance();
        void SetupDebugMessenger();
    private:
        GLFWwindow* m_WindowHandle;

        // Instance
        static VkInstance s_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;

        // Devices
        Shared<VulkanPhysicalDevice> m_PhysicalDevice = nullptr;
        Shared<VulkanLogicalDevice> m_LogicalDevice = nullptr;

        // Swapchain
        Shared<VulkanSwapchain> m_Swapchain = nullptr;
    };
}

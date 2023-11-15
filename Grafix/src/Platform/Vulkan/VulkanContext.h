#pragma once

#include "Grafix/Renderer/RendererContext.h"

#include "vulkan/vulkan.h"
#include "VulkanPhysicalDevice.h"

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

        static VkInstance GetInstance() { return s_Instance; }
    private:
        void CreateInstance();
        void SetupDebugMessenger();
    private:
        GLFWwindow* m_WindowHandle;

        static VkInstance s_Instance;
        Shared<VulkanPhysicalDevice> m_PhysicalDevice = nullptr;

        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    };
}

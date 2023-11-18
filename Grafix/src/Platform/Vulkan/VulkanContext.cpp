#include "pch.h"
#include "VulkanContext.h"

#include "Grafix/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Grafix
{
#ifdef GF_CONFIG_DEBUG
    static bool s_EnableValidation = true;
#else
    static bool s_EnableValidation = false;
#endif

    static void CheckExtensionSupport(const std::vector<const char*>& extensions)
    {
        uint32_t supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

        GF_CORE_DEBUG("Found {0} supported extensions:", supportedExtensionCount);
        for (uint32_t i = 0; i < supportedExtensionCount; ++i)
            GF_CORE_DEBUG("      {0}: {1}", i + 1, supportedExtensions[i].extensionName);

        for (auto& extension: extensions)
        {
            bool found = false;
            for (const auto& supportedExtension : supportedExtensions)
            {
                if (strcmp(extension, supportedExtension.extensionName) == 0)
                {
                    found = true;
                    GF_CORE_TRACE("{0} is supported.", extension);
                    break;
                }
            }
            if (!found)
            {
                GF_CORE_ASSERT(false, "{0} is not supported.", extension);
                return;
            }
        }
    }

    static void CheckLayersSupport(const std::vector<const char*>& layers)
    {
        uint32_t supportedLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);
        std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());
        GF_CORE_DEBUG("Found {0} supported layers:", supportedLayerCount);
        for (uint32_t i = 0; i < supportedLayerCount; ++i)
            GF_CORE_DEBUG("      {0}: {1}", i + 1, supportedLayers[i].layerName);

        for (auto& layer: layers)
        {
            bool found = false;
            for (const auto& supportedLayer : supportedLayers)
            {
                if (strcmp(layer, supportedLayer.layerName) == 0)
                {
                    found = true;
                    GF_CORE_TRACE("{0} is supported.", layer);
                    break;
                }
            }
            if (!found)
            {
                GF_CORE_ASSERT(false, "{0} is not supported.", layer);
                return;
            }
        }
    }

    // TODO: Improve this
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: { GF_CORE_TRACE(pCallbackData->pMessage); break; }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    { GF_CORE_INFO(pCallbackData->pMessage);  break; }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: { GF_CORE_WARN(pCallbackData->pMessage);  break; }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   { GF_CORE_ERROR(pCallbackData->pMessage); break; }
        }
        return VK_FALSE;
    }

    VkInstance VulkanContext::s_Instance = nullptr;

    static VulkanContext* s_Context = nullptr;

    VulkanContext::VulkanContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        s_Context = this;
    }

    VulkanContext::~VulkanContext()
    {
        m_Swapchain->Destroy();

        m_LogicalDevice->Destroy();

        if (s_EnableValidation)
        {
            // Destroy debug messenger
            auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Instance, "vkDestroyDebugUtilsMessengerEXT");
            GF_CORE_ASSERT(vkDestroyDebugUtilsMessengerEXT, "Could not load vkDestroyDebugUtilsMessengerEXT");
            vkDestroyDebugUtilsMessengerEXT(s_Instance, m_DebugMessenger, nullptr);
        }
        vkDestroyInstance(s_Instance, nullptr);

        s_Context = nullptr;
    }

    void VulkanContext::Init()
    {
        CreateInstance();

        if(s_EnableValidation)
            SetupDebugMessenger();
        
        m_PhysicalDevice = VulkanPhysicalDevice::Pick();

        m_Swapchain = CreateShared<VulkanSwapchain>();
        m_Swapchain->InitSurface(m_WindowHandle);

        // No features for now.
        VkPhysicalDeviceFeatures features{};
        m_LogicalDevice = VulkanLogicalDevice::Create(m_PhysicalDevice, features);

        m_Swapchain->BindLogicalDevice(m_LogicalDevice);

        auto& windowData = Application::Get().GetWindow().m_Data;
        m_Swapchain->Create(windowData.Width, windowData.Height);
    }

    void VulkanContext::SwapBuffers()
    {
        m_Swapchain->Present();
    }

    VulkanContext& VulkanContext::Get()
    {
        return *s_Context;
    }

    void VulkanContext::CreateInstance()
    {
        VkInstanceCreateInfo instanceCI{};
        instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // Application information
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Grafix";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Grafix";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;
        instanceCI.pApplicationInfo = &appInfo;

        // Extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (s_EnableValidation)
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        CheckExtensionSupport(requiredExtensions);
        instanceCI.enabledExtensionCount = (uint32_t)requiredExtensions.size();
        instanceCI.ppEnabledExtensionNames = requiredExtensions.data();

        // Validation layer
        if (s_EnableValidation)
        {
            const char* validationLayer = "VK_LAYER_KHRONOS_validation";
            CheckLayersSupport({ validationLayer });
            instanceCI.enabledLayerCount = 1;
            instanceCI.ppEnabledLayerNames = &validationLayer;
        }

        VK_CHECK(vkCreateInstance(&instanceCI, nullptr, &s_Instance));
    }

    void VulkanContext::SetupDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT debugMsgCI{};
        debugMsgCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMsgCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMsgCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMsgCI.pfnUserCallback = DebugUtilsMessengerCallback;
        debugMsgCI.pUserData = nullptr;

        // NOTE: vkCreateDebugUtilsMessengerEXT is an extension function, so we need to load it manually.
        auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Instance, "vkCreateDebugUtilsMessengerEXT");
        GF_CORE_ASSERT(vkCreateDebugUtilsMessengerEXT, "Could not load vkCreateDebugUtilsMessengerEXT.");
        VK_CHECK(vkCreateDebugUtilsMessengerEXT(s_Instance, &debugMsgCI, nullptr, &m_DebugMessenger));
    }
}


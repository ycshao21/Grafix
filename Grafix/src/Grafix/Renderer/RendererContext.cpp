#include "pch.h"
#include "RendererContext.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

#include "RendererAPI.h"

namespace Grafix
{
    Shared<RendererContext> RendererContext::Create(void* windowHandle)
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:   { GF_CORE_ASSERT(false, "Renderer API has not been selected!");  break; }
            case RendererAPIType::OpenGL: { return CreateShared<OpenGLContext>((GLFWwindow*)windowHandle); break; }
            case RendererAPIType::Vulkan: { return CreateShared<VulkanContext>((GLFWwindow*)windowHandle); break; }
        }
        return nullptr;
    }
}


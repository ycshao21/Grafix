#include "pch.h"
#include "RendererContext.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include "Grafix/Renderer/RendererAPI.h"

namespace Grafix
{
    Shared<RendererContext> RendererContext::Create(void* windowHandle)
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:   { GF_CORE_ASSERT(false, "Renderer API has not been selected!");  break; }
            case RendererAPIType::OpenGL: { return CreateShared<OpenGLContext>((GLFWwindow*)windowHandle); break; }
        }
        return nullptr;
    }
}


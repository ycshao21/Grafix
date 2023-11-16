#include "pch.h"
#include "Renderer.h"

#include "RendererAPI.h"

namespace Grafix
{
    static RendererAPI* s_RendererAPI = nullptr;

    void Renderer::Init()
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:   { GF_CORE_ASSERT(false, "RendererAPI has not been selected!"); break; }
            case RendererAPIType::OpenGL: { GF_CORE_INFO("RendererAPI: OpenGL");                         break; }
            case RendererAPIType::Vulkan: { GF_CORE_INFO("RendererAPI: Vulkan");                         break; }
        }
        
        ////s_RendererAPI->Init();
    }

    void Renderer::Shutdown()
    {
        ////s_RendererAPI->Shutdown();
    }
}


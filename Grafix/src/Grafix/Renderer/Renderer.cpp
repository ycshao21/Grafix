#include "pch.h"
#include "Renderer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Grafix
{
    static RendererAPI* s_RendererAPI = nullptr;

    void Renderer::Init()
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:
            {
                GF_CORE_ASSERT(false, "Renderer API has not been selected!");
                break;
            }
            case RendererAPIType::OpenGL:
            {
                GF_CORE_INFO("Renderer API: OpenGL");
                s_RendererAPI = new OpenGLRenderer();
                break;
            }
            case RendererAPIType::Vulkan:
            {
                GF_CORE_INFO("Renderer API: Vulkan");
                break;
            }
        }
        
        s_RendererAPI->Init();
    }

    void Renderer::Shutdown()
    {
        s_RendererAPI->Shutdown();
    }

    void Renderer::BeginScene()
    {
        s_RendererAPI->SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        s_RendererAPI->Clear();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Shared<VertexArray>& vertexArray)
    {
        vertexArray->Bind();
        s_RendererAPI->DrawIndexed(vertexArray);
    }
}


#include "pch.h"
#include "Renderer.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/Vulkan/VulkanRenderer.h"

namespace Grafix
{
    static RendererAPI* s_RendererAPI = nullptr;

    Renderer::SceneData* Renderer::s_SceneData = nullptr;

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
                s_RendererAPI = new VulkanRenderer();
                break;
            }
        }
        
        s_RendererAPI->Init();
        s_SceneData = new SceneData;
    }

    void Renderer::Shutdown()
    {
        delete s_SceneData;
        s_RendererAPI->Shutdown();
    }

    void Renderer::BeginScene(const OrthographicCamera& camera)
    {
        s_RendererAPI->SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        s_RendererAPI->Clear();

        s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Shared<Shader>& shader, const Shared<VertexArray>& vertexArray)
    {
        shader->Bind();
        shader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);

        vertexArray->Bind();
        s_RendererAPI->DrawIndexed(vertexArray);
    }
}


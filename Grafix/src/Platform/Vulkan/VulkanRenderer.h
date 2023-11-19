#pragma once

#include "Grafix/Renderer/RendererAPI.h"

#include "vulkan/vulkan.h"

namespace Grafix
{
    class VulkanRenderer : public RendererAPI
    {
    public:
        virtual void Init() override;
        virtual void Shutdown() override {}

        virtual void SetClearColor(const glm::vec4& color) override {}
        virtual void Clear() override {}

        virtual void DrawIndexed(const Shared<VertexArray>& vertexArray) override {}
    private:
        VkPipeline m_Pipeline;
    };

}


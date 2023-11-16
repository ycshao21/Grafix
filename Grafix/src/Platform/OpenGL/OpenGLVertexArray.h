#pragma once

#include "Grafix/Renderer/VertexArray.h"

namespace Grafix
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray() = default;

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer) override;
        virtual void SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer) override;

        virtual const std::vector<Shared<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
        virtual const Shared<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
    private:
        uint32_t m_RendererID;
        std::vector<Shared<VertexBuffer>> m_VertexBuffers;
        Shared<IndexBuffer> m_IndexBuffer;
    };
}

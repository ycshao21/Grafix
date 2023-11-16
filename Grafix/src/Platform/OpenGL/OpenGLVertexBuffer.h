#pragma once

#include "Grafix/Renderer/VertexBuffer.h"

namespace Grafix
{
    class OpenGLVertexBuffer final : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(float* vertices, uint32_t bytes);
        virtual ~OpenGLVertexBuffer();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual const VertexBufferLayout& GetLayout() const override { return m_Layout; }
        virtual void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }
    private:
        uint32_t m_RendererID;

        VertexBufferLayout m_Layout;
    };
}

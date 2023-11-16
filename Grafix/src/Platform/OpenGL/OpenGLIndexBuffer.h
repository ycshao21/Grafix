#pragma once

#include "Grafix/Renderer/IndexBuffer.h"

namespace Grafix
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual inline uint32_t GetCount() const override { return m_Count; }
    private:
        uint32_t m_RendererID;

        uint32_t m_Count = 0;
    };
}

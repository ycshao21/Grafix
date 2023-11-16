#include "pch.h"
#include "VertexBuffer.h"

#include "Platform/OpenGL/OpenGLVertexBuffer.h"

#include "Grafix/Renderer/RendererAPI.h"

namespace Grafix
{
    Unique<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t bytes)
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:    return nullptr;
            case RendererAPIType::OpenGL:  return CreateUnique<OpenGLVertexBuffer>(vertices, bytes);
            ////case RendererAPIType::Vulkan:  return nullptr;
        }
        return nullptr;
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------
    // Vertex Buffer Layout
    // ------------------------------------------------------------------------------------------------------------------------------------------------

    VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements)
        : m_Elements(elements)
    {
        CalcOffsetsAndStride();
    }

    void VertexBufferLayout::CalcOffsetsAndStride()
    {
        uint32_t offset = 0;
        m_Stride = 0;
        for (auto& element : m_Elements)
        {
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }
}

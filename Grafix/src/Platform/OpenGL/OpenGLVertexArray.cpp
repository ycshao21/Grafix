#include "pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Grafix
{
    static GLenum ShaderDataTypeToOpenGLenumType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:    return GL_FLOAT;
            case ShaderDataType::Float2:   return GL_FLOAT;
            case ShaderDataType::Float3:   return GL_FLOAT;
            case ShaderDataType::Float4:   return GL_FLOAT;
            case ShaderDataType::Mat3:     return GL_FLOAT;
            case ShaderDataType::Mat4:     return GL_FLOAT;
            case ShaderDataType::Int:      return GL_INT;
            case ShaderDataType::Int2:     return GL_INT;
            case ShaderDataType::Int3:     return GL_INT;
            case ShaderDataType::Int4:     return GL_INT;
            case ShaderDataType::Bool:     return GL_BOOL;
        }

        GF_CORE_ASSERT(false, "Unknown shader data type!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind()
    {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind()
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer)
    {
        GF_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size() > 0, "Vertex buffer has no layout!");

        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();

        uint32_t index = 0;
        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element: layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLenumType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.Offset);
            ++index;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer)
    {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }
}


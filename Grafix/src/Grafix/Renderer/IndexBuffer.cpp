#include "pch.h"
#include "IndexBuffer.h"

#include "Platform/OpenGL/OpenGLIndexBuffer.h"

#include "Grafix/Renderer/RendererAPI.h"

namespace Grafix
{
    Unique<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:    return nullptr;
            case RendererAPIType::OpenGL:  return CreateUnique<OpenGLIndexBuffer>(indices, count);
            ////case RendererAPIType::Vulkan:  return nullptr;
        }
        return nullptr;

    }
}

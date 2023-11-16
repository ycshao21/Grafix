#include "pch.h"
#include "IndexBuffer.h"

#include "Platform/OpenGL/OpenGLIndexBuffer.h"

#include "RendererAPI.h"

namespace Grafix
{
    Shared<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:   { GF_CORE_ASSERT(false, "Renderer API has not been selected!"); return nullptr; }
            case RendererAPIType::OpenGL: { return CreateShared<OpenGLIndexBuffer>(indices, count); }
            ////case RendererAPIType::Vulkan: { return nullptr; }
        }
        return nullptr;
    }
}

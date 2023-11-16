#include "pch.h"
#include "VertexArray.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "RendererAPI.h"

namespace Grafix
{
    Shared<VertexArray> VertexArray::Create()
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:   { GF_CORE_ASSERT(false, "RendererAPI has not been selected!"); return nullptr; }
            case RendererAPIType::OpenGL: { return CreateShared<OpenGLVertexArray>(); }
            ////case RendererAPIType::Vulkan:  return nullptr;
        }
        return nullptr;
    }
}


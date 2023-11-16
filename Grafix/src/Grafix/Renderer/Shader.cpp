#include "pch.h"
#include "Shader.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Grafix
{
    Shared<Shader> Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:   { GF_CORE_ASSERT(false, "Renderer API has not been selected!"); return nullptr; }
            case RendererAPIType::OpenGL: { return CreateShared<OpenGLShader>(vertexSrc, fragmentSrc); }
            ////case RendererAPIType::Vulkan: { return nullptr; }
        }
        return nullptr;
    }
}


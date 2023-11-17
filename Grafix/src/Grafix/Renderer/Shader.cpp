#include "pch.h"
#include "Shader.h"

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Grafix
{
    // ------------------------------------------------------------------------------------------------------------------------------------
    // Shader (Base)
    // ------------------------------------------------------------------------------------------------------------------------------------

    Shared<Shader> Shader::Create(const std::string& filePath)
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:   { GF_CORE_ASSERT(false, "Renderer API has not been selected!"); return nullptr; }
            case RendererAPIType::OpenGL: { return CreateShared<OpenGLShader>(filePath); }
            ////case RendererAPIType::Vulkan: { return nullptr; }
        }
        return nullptr;
    }

    Shared<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (RendererAPI::GetType())
        {
            case RendererAPIType::None:   { GF_CORE_ASSERT(false, "Renderer API has not been selected!"); return nullptr; }
            case RendererAPIType::OpenGL: { return CreateShared<OpenGLShader>(name, vertexSrc, fragmentSrc); }
            ////case RendererAPIType::Vulkan: { return nullptr; }
        }
        return nullptr;
    }

    // ------------------------------------------------------------------------------------------------------------------------------------
    // Shader Library
    // ------------------------------------------------------------------------------------------------------------------------------------

    void ShaderLibrary::AddShader(const Shared<Shader>& shader)
    {
        auto& name = shader->GetName();
        AddShader(name, shader);
    }

    void ShaderLibrary::AddShader(const std::string& name, const Shared<Shader>& shader)
    {
        GF_CORE_ASSERT(!NameExists(name), "Shader already exists!");
        m_Shaders[name] = shader;  // WARN: Name is not changed in shader
    }

    Shared<Shader> ShaderLibrary::LoadShader(const std::string& filePath)
    {
        auto shader = Shader::Create(filePath);
        AddShader(shader);
        return shader;
    }

    Shared<Shader> ShaderLibrary::LoadShader(const std::string& name, const std::string& filePath)
    {
        auto shader = Shader::Create(filePath);
        AddShader(name, shader);
        return shader;
    }

    Shared<Shader> ShaderLibrary::GetShader(const std::string& name)
    {
        GF_CORE_ASSERT(NameExists(name), "Shader not found!");
        return m_Shaders[name];
    }
}


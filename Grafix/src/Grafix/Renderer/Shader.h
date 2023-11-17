#pragma once

#include <Grafix/Core/Base.h>
#include <string>
#include <glm/glm.hpp>

namespace Grafix
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual const std::string& GetName() const = 0;

        static Shared<Shader> Create(const std::string& filePath);
        static Shared<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    };

    class ShaderLibrary
    {
    public:
        void AddShader(const Shared<Shader>& shader);
        void AddShader(const std::string& name, const Shared<Shader>& shader);
        Shared<Shader> LoadShader(const std::string& filePath);
        Shared<Shader> LoadShader(const std::string& name, const std::string& filePath);

        Shared<Shader> GetShader(const std::string& name);
    private:
        inline bool NameExists(const std::string& name) const
        {
            return m_Shaders.find(name) != m_Shaders.end();
        }
    private:
        std::unordered_map<std::string, Shared<Shader>> m_Shaders;
    };
}

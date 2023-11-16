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

        virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;

        static Shared<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
    };
}

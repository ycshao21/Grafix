#pragma once

#include "Grafix/Renderer/Shader.h"

namespace Grafix
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;
    private:
        uint32_t m_RendererID = 0;
    };
}


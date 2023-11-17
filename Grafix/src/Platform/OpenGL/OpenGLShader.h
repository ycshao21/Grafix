#pragma once

#include "Grafix/Renderer/Shader.h"
#include <glad/glad.h>

namespace Grafix
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& filePath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual const std::string& GetName() const override { return m_Name; }

        // Uniforms

        virtual void UploadUniformInt(const std::string& name, int value);

        virtual void UploadUniformFloat(const std::string& name, float value);
        virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

        virtual void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
    private:
        std::string ReadFile(const std::string& filePath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& src);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSrcs);
    private:
        std::string m_Name;
        uint32_t m_RendererID = 0;
    };
}


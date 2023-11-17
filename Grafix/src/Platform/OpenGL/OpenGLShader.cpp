#include "pch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Grafix
{
    static GLenum ShaderTypeFromString(const std::string& type)
    {
        if(type == "vertex") return GL_VERTEX_SHADER;
        if(type == "fragment") return GL_FRAGMENT_SHADER;

        GF_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& filePath)
    {
        std::string shaderSrc = ReadFile(filePath);
        auto shaderSources = PreProcess(shaderSrc);
        Compile(shaderSources);

        // Extract name from filepath
        size_t lastSlash = filePath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        size_t lastDot = filePath.rfind('.');
        size_t count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filePath.substr(lastSlash, count);

        GF_CORE_INFO("Shader loaded -- Name: {0}, Path: {1}", m_Name, filePath);
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name)
    {
        std::unordered_map<GLenum, std::string> shaderSrcs;
        shaderSrcs[GL_VERTEX_SHADER] = vertexSrc;
        shaderSrcs[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(shaderSrcs);
    }
    
    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(m_RendererID);
    }
   
    void OpenGLShader::Bind()
    {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind()
    {
        glUseProgram(0);
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, int value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    std::string OpenGLShader::ReadFile(const std::string& filePath)
    {
        std::string result;
        std::ifstream in(filePath, std::ios::in | std::ios::binary);
        if (!in)
            GF_CORE_ERROR("Could not open file \"{0}\"", filePath);

        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();

        return result;
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& src)
    {
        std::unordered_map<GLenum, std::string> shaderSrcs;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = src.find(typeToken, 0);  // Start of shader type declaration line
        while (pos != std::string::npos)
        {
            size_t eol = src.find_first_of("\r\n", pos);  // End of line
            GF_CORE_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1;  // Start of shader type name (after "#type " keyword)
            std::string type = src.substr(begin, eol - begin);
            GF_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type");

            size_t nextLinePos = src.find_first_not_of("\r\n", eol);  // Start of shader code after shader type declaration line
            pos = src.find(typeToken, nextLinePos);  // Start of next line
            shaderSrcs[ShaderTypeFromString(type)]
                = (pos == std::string::npos) ? src.substr(nextLinePos) : src.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSrcs;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSrcs)
    {
        m_RendererID = glCreateProgram();
        GF_CORE_ASSERT(shaderSrcs.size() <= 2, "We only support 2 shaders for now!");

        std::array<GLenum, 2> glShaderIDs;
        int glShaderIDIndex = 0;

        for (auto& [type, src] : shaderSrcs)
        {
            GLuint shader = glCreateShader(type);

            const GLchar* sourceCStr = (const GLchar*)src.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);

            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(shader);

                GF_CORE_ERROR("{0}", infoLog.data());
                GF_CORE_ASSERT(false, "Shader compilation failure!");
                break;
            }

            glAttachShader(m_RendererID, shader);
            glShaderIDs[glShaderIDIndex++] = shader;
        }

        glLinkProgram(m_RendererID);

        GLint isLinked = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);
            
            glDeleteProgram(m_RendererID);

            for(auto id : glShaderIDs)
                glDeleteShader(id);

            GF_CORE_ERROR("{0}", infoLog.data());
            GF_CORE_ASSERT(false, "Shader link failure!");
            return;
        }

        for(auto id : glShaderIDs)
            glDetachShader(m_RendererID, id);
    }
}


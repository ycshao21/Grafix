#pragma once

#include <string>

namespace Grafix
{
    class Shader
    {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~Shader();

        void Bind();
        void Unbind();
    private:
        uint32_t m_RendererID = 0;
    };
}

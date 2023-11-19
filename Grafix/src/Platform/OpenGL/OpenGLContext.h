#pragma once

#include "Grafix/Renderer/RendererContext.h"

struct GLFWwindow;

namespace Grafix
{
    class OpenGLContext : public RendererContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);
        virtual ~OpenGLContext() = default;

        virtual void Init() override;
        virtual void SwapBuffers() override;
        virtual void Destroy() override;
    private:
        GLFWwindow* m_WindowHandle;
    };
}

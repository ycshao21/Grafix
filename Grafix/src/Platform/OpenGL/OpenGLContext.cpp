#include "pch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>

namespace Grafix
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);

        // Initialize Glad
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        GF_CORE_ASSERT(status, "Failed to initialize Glad!");
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}
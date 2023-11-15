#include "pch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
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

        GF_CORE_INFO("Physical device: {0}", (const char*)glGetString(GL_RENDERER));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}

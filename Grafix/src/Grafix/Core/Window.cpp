#include "pch.h"
#include "Window.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Grafix/Renderer/RendererAPI.h"

#include <glad/glad.h>

namespace Grafix
{
    static bool s_GLFWInitialized = false;

    Window::Window(const WindowSpecification& spec)
    {
        m_Data.Title = spec.Title;
        m_Data.Width = spec.Width;
        m_Data.Height = spec.Height;
        m_Data.VSync = spec.VSync;

        switch (RendererAPI::GetType())
        {
            case RendererAPIType::OpenGL: { m_Data.Title += " | OpenGL"; break; }
            case RendererAPIType::Vulkan: { m_Data.Title += " | Vulkan"; break; }
        }
    }

    Window::~Window()
    {
        Shutdown();
    }

    void Window::OnUpdate()
    {
        // TEMP
        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glfwPollEvents();
        m_Context->SwapBuffers();
    }

    inline void Window::SetVSync(bool enabled)
    {
        glfwSwapInterval(enabled ? 1 : 0);
        m_Data.VSync = enabled;
    }

    Unique<Window> Window::Create(const WindowSpecification& spec)
    {
        return CreateUnique<Window>(spec);
    }

    void Window::Init()
    {
        // ------------------------------------------------------------------------------------------------------------------------------------------
        // Initialize GLFW
        // ------------------------------------------------------------------------------------------------------------------------------------------
        if (!s_GLFWInitialized)
        {
            int success = glfwInit();
            GF_CORE_ASSERT(success, "Failed to initialize GLFW!");
            s_GLFWInitialized = true;

            glfwSetErrorCallback([](int error, const char* description)
            {
                GF_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
            });
        }

        // ------------------------------------------------------------------------------------------------------------------------------------------
        // Create GLFW Window
        // ------------------------------------------------------------------------------------------------------------------------------------------
        {
            GF_CORE_INFO("Creating window: {0}({1} ¡Á {2})", m_Data.Title, m_Data.Width, m_Data.Height);
            m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
        }

        // ------------------------------------------------------------------------------------------------------------------------------------------
        // Initialize Renderer Context
        // ------------------------------------------------------------------------------------------------------------------------------------------
        m_Context = RendererContext::Create(m_Window);
        m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(m_Data.VSync);

        // ------------------------------------------------------------------------------------------------------------------------------------------
        // Set GLFW Callbacks
        // ------------------------------------------------------------------------------------------------------------------------------------------
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            data.Width = width;
            data.Height = height;
            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event((Key)key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event((Key)key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event((Key)key, 1);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            KeyTypedEvent event((Key)keycode);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event((MouseButton)button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event((MouseButton)button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }

    void Window::Shutdown()
    {
        // Destroy the GLFW window
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;

        // Terminate GLFW
        glfwTerminate();
        s_GLFWInitialized = false;
    }
}

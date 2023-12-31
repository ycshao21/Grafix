#include "pch.h"
#include "Window.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Grafix/Renderer/RendererAPI.h"

#include <GLFW/glfw3.h>
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
        glfwPollEvents();
        m_Context->SwapBuffers();
    }

    inline void Window::SetVSync(bool enabled)
    {
        m_Data.VSync = enabled;

        switch (RendererAPI::GetType())
        {
            case RendererAPIType::OpenGL:
            {
                glfwSwapInterval(enabled ? 1 : 0);
                break;
            }
            case RendererAPIType::Vulkan:
            {
                // TODO: swapchain->SetVSync(enabled);
                break;
            }
        }
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
            if (RendererAPI::GetType() == RendererAPIType::Vulkan)
            {
                // [Vulkan] Since we are using Vulkan, we don't need GLFW to create an OpenGL context.
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

                // [Vulkan] Window resizing will break the swapchain, so we disable it here.
                //          When resizing is supported by the swapchain, this can be removed.
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            }

            // If we need fullscreen, we need to set the 4th parameter to glfwGetPrimaryMonitor()
            m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
            GF_CORE_INFO("Creating window: {0} ({1} �� {2})", m_Data.Title, m_Data.Width, m_Data.Height);
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

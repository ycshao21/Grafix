#include "pch.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Grafix/Renderer/Renderer.h"
#include "Grafix/Renderer/RendererAPI.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace Grafix
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& spec)
    {
        s_Instance = this;

        WindowSpecification windowSpec;
        windowSpec.Title = spec.Name;
        windowSpec.Width = spec.WindowWidth;
        windowSpec.Height = spec.WindowHeight;
        windowSpec.VSync = spec.VSync;

        m_Window = Window::Create(windowSpec);
        m_Window->Init();
        m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        Renderer::Init();
    }

    Application::~Application()
    {
        Renderer::Shutdown();
        // Note: Layers are destroyed before the window.
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(e); });
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) { return OnWindowResize(e); });

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    void Application::Run()
    {
        while(m_IsRunning)
        {
            // TODO: Make glfwGetTime() disappear
            float time = (float)glfwGetTime();
            float ts = time - m_LastFrameTime;
            m_LastFrameTime = time;

            m_Window->PollEvents();

            if (!m_Minimized)
            {
                m_Window->BeginFrame();

                for(Layer* layer : m_LayerStack)
                    layer->OnUpdate(ts);

                m_Window->SwapBuffers();
            }
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_IsRunning = false;
        return false;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            GF_CORE_WARN("Window minizied.");
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;

        if (RendererAPI::GetType() == RendererAPIType::Vulkan)
        {
            auto swapchain = std::dynamic_pointer_cast<VulkanContext>(m_Window->GetContext())->GetSwapchain();
            swapchain->Resize(e.GetWidth(), e.GetHeight());
        }

        return false;
    }
}
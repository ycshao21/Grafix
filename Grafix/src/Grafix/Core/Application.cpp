#include "pch.h"
#include "Application.h"

#include "Grafix/Renderer/Renderer.h"

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
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(e); });

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
            {
                break;
            }
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
            
            for(Layer* layer : m_LayerStack)
                layer->OnUpdate(ts);

            m_Window->OnUpdate();
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
}
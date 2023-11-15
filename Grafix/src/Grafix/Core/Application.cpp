#include "pch.h"
#include "Application.h"

#include "Base.h"

namespace Grafix
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& spec)
    {
        s_Instance = this;
        Logging::Init();

        m_Window = Window::Create(WindowSpecification(spec.Name, spec.Width, spec.Height));
        m_Window->Init();
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
    }

    Application::~Application()
    {
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

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
        return true;
    }
}
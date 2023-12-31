#pragma once

#include "Window.h"
#include "LayerStack.h"

#include "Events/ApplicationEvent.h"

namespace Grafix
{
    struct ApplicationSpecification final
    {
        std::string Name = "Grafix App";
        uint32_t WindowWidth = 1280;
        uint32_t WindowHeight = 720;
        bool VSync = true;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& spec = {});
        virtual ~Application();

        static inline Application& Get() { return *s_Instance; }
        inline Window& GetWindow() { return *m_Window; }

        void OnEvent(Event& e);

        void Run();

        void PushLayer(Layer* layer);
    private:
        bool OnWindowClose(WindowCloseEvent& e);
    private:
        bool m_IsRunning = true;

        Unique<Window> m_Window = nullptr;
        LayerStack m_LayerStack;

        float m_LastFrameTime = 0.0f;
        static Application* s_Instance;
    };

    // Define the function in any client application
    Application* CreateApplication();
}

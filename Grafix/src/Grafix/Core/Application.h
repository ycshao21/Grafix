#pragma once

#include "Window.h"
#include "LayerStack.h"

#include "Events/ApplicationEvent.h"

#include "Grafix/Renderer/Shader.h"

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

        // TEMP
        uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;

        Unique<Shader> m_Shader = nullptr;
    };

    // Define the function in any client application
    Application* CreateApplication();
}

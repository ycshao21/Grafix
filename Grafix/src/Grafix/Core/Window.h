#pragma once
#include "Grafix/Core/Window.h"

#include "Base.h"
#include "Events/Event.h"

#include <string>

// NOTE: "#include <GLFW/glfw3.h>" will cause compile error, so we declare "struct GLFWwindow" here.
struct GLFWwindow;

namespace Grafix
{
    struct WindowSpecification final
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
        bool VSync;
    };

    class Window final
    {
        using EventCallbackFn = std::function<void(Event&)>;
    public:
        Window(const WindowSpecification& spec);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
    public:
        void Init();
        void Shutdown();

        void OnUpdate();

        const std::string& GetTitle() const { return m_Data.Title; }
        uint32_t GetWidth() const { return m_Data.Width; }
        uint32_t GetHeight() const { return m_Data.Height; }
        std::pair<uint32_t, uint32_t> GetSize() const { return { m_Data.Width, m_Data.Height }; }

        inline void* GetNative() const { return m_Window; }

        inline void SetVSync(bool enabled);
        inline bool IsVSync() const { return m_Data.VSync; }

        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

        static Unique<Window> Create(const WindowSpecification& spec);
    private:
        WindowSpecification m_Spec;

        struct WindowData
        {
            std::string Title;
            uint32_t Width;
            uint32_t Height;

            bool VSync;
            EventCallbackFn EventCallback;
        };
        WindowData m_Data;

        GLFWwindow* m_Window = nullptr;
    };
}

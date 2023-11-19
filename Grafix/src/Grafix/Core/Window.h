#pragma once
#include "Grafix/Core/Window.h"

#include "Grafix/Renderer/RendererContext.h"

#include "Base.h"
#include "Events/Event.h"

#include <string>

struct GLFWwindow;

namespace Grafix
{
    struct WindowSpecification final
    {
        std::string Title = "Grafix App";
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool VSync = true;
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

        void BeginFrame();
        void SwapBuffers();

        void PollEvents();

        const std::string& GetTitle() const { return m_Data.Title; }
        uint32_t GetWidth() const { return m_Data.Width; }
        uint32_t GetHeight() const { return m_Data.Height; }
        std::pair<uint32_t, uint32_t> GetSize() const { return { m_Data.Width, m_Data.Height }; }

        inline void* GetNative() const { return m_Window; }
        Shared<RendererContext> GetContext() const { return m_Context; }

        inline void SetVSync(bool enabled);
        inline bool IsVSync() const { return m_Data.VSync; }

        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

        static Unique<Window> Create(const WindowSpecification& spec);
    private:
        void Shutdown();
    private:
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
        Shared<RendererContext> m_Context = nullptr;

        friend class VulkanContext;
    };
}

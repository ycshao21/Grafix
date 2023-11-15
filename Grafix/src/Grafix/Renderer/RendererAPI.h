#pragma once

#include <numeric>

namespace Grafix
{
    enum class RendererAPIType : uint8_t
    {
        None = 0,
        OpenGL, Vulkan
    };

    class RendererAPI
    {
    public:
        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        static RendererAPIType GetType() { return s_APIType; }
        // NOTE: You cannot set API type at runtime. You need to set it in RendererAPI.cpp.
        static void SetType(RendererAPIType type) { s_APIType = type; }
    private:
        static RendererAPIType s_APIType;
    };
}
#pragma once

namespace Grafix
{
    class RendererContext
    {
    public:
        virtual ~RendererContext() = default;

        // When a renderer context is created, this function must be called manually.
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static Shared<RendererContext> Create(void* windowHandle);
    };
}

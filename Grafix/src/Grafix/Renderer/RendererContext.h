#pragma once

namespace Grafix
{
    class RendererContext
    {
    public:
        virtual void Init() = 0;

        virtual void SwapBuffers() = 0;

        static Shared<RendererContext> Create(void* windowHandle);
    };
}

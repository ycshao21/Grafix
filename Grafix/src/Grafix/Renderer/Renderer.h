#pragma once

#include "VertexArray.h"

namespace Grafix
{
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene();
        static void EndScene();

        static void Submit(const Shared<VertexArray>& vertexArray);
    };
}

#pragma once

#include <numeric>
#include <glm/glm.hpp>

#include "VertexArray.h"

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

        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;

        virtual void DrawIndexed(const Shared<VertexArray>& vertexArray) = 0;

        // NOTE: You cannot set API type at runtime. You need to set it in RendererAPI.cpp.
        static void SetType(RendererAPIType type) { s_APIType = type; }
        static RendererAPIType GetType() { return s_APIType; }
    private:
        static RendererAPIType s_APIType;
    };
}

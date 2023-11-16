#pragma once

#include "OrthographicCamera.h"
#include "VertexArray.h"
#include "Shader.h"

namespace Grafix
{
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();

        static void Submit(const Shared<Shader>& shader, const Shared<VertexArray>& vertexArray);
    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static SceneData* s_SceneData;
    };
}

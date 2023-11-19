#pragma once

#include "Grafix.h"

#include <vulkan/vulkan.h>

class TestLayer : public Grafix::Layer
{
public:
    TestLayer();
    virtual ~TestLayer();

    virtual void OnUpdate(float ts) override;
    virtual void OnEvent(Grafix::Event& e) override;
private:
    void PipelineSetup();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
private:
    ///////////////////////////////////////////////////////
    // Vulkan
    ///////////////////////////////////////////////////////

    VkShaderModule m_VertexShaderModule;
    VkShaderModule m_FragmentShaderModule;

    VkPipelineLayout m_PipelineLayout;
    VkRenderPass m_RenderPass;

    VkPipeline m_GraphicsPipeline;

    std::vector<VkFramebuffer> m_SwapchainFramebuffers;

    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores, m_RenderFinishedSemaphores;
    const uint32_t m_MaxFramesInFlight = 2;
    std::vector<VkFence> m_InFlightFences;

    uint32_t m_CurrentFrameIndex = 0;

    ///////////////////////////////////////////////////////
    // OpenGL
    ///////////////////////////////////////////////////////

    ////// Shader
    ////Grafix::ShaderLibrary m_ShaderLibrary;

    ////Grafix::Shared<Grafix::Shader> m_SquareShader = nullptr;
    ////Grafix::Shared<Grafix::VertexArray> m_SquareVA= nullptr;
    ////glm::vec4 m_SquareColor = glm::vec4(0.3f);

    ////Grafix::Shared<Grafix::Shader> m_Shader = nullptr;
    ////Grafix::Shared<Grafix::VertexArray> m_VertexArray = nullptr;

    // Camara
    Grafix::OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition = glm::vec3(0.0f);
    float m_CameraMoveSpeed = 1.0f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 85.0f;

    glm::vec3 m_TrianglePosition = glm::vec3(0.0f);
    float m_TriangleMoveSpeed = 0.5f;
    float m_TriangleRotation = 0.0f;
    float m_TriangleRotationSpeed = 105.0f;
};

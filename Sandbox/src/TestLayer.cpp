#include "TestLayer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Platform/OpenGL/OpenGLShader.h"

#include "Platform/Vulkan/VulkanContext.h"
#include <fstream>

static std::vector<char> ReadFile(const std::string& filePath)
{
    std::ifstream in(filePath, std::ios::in | std::ios::binary);
    if (!in)
        GF_CORE_ERROR("Could not open file \"{0}\"", filePath);

    in.seekg(0, std::ios::end);
    std::vector<char> result(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&result[0], result.size());
    in.close();

    return result;
}

TestLayer::TestLayer()
    : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
{
    ////{
    ////    m_VertexArray = Grafix::VertexArray::Create();

    ////    // Vertex Buffer
    ////    float vertices[3 * 7] = {
    ////        -0.5f, -0.5f, 0.0f, 0.85f, 0.0f,  0.85f, 1.0f,
    ////         0.5f, -0.5f, 0.0f, 0.0f,  0.85f, 0.85f, 1.0f,
    ////         0.0f,  0.5f, 0.0f, 0.85f, 0.85f, 0.0f,  1.0f
    ////    };
    ////    auto vertexBuffer = Grafix::VertexBuffer::Create(vertices, sizeof(vertices));

    ////    Grafix::VertexBufferLayout layout = {
    ////        { Grafix::ShaderDataType::Float3, "a_Position" },
    ////        { Grafix::ShaderDataType::Float4, "a_Color" }
    ////    };
    ////    vertexBuffer->SetLayout(layout);
    ////    m_VertexArray->AddVertexBuffer(vertexBuffer);

    ////    // Index Buffer
    ////    uint32_t indices[3] = { 0, 1, 2 };
    ////    auto indexBuffer = Grafix::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
    ////    m_VertexArray->SetIndexBuffer(indexBuffer);

    ////    // Shader
    ////    m_Shader = m_ShaderLibrary.LoadShader("assets/shaders/Colorful.glsl");
    ////}

    ////{
    ////    m_SquareVA = Grafix::VertexArray::Create();

    ////    // Vertex Buffer
    ////    float vertices[4 * 3] = {
    ////         0.5f,  0.5f, 0.0f,
    ////        -0.5f,  0.5f, 0.0f,
    ////        -0.5f, -0.5f, 0.0f,
    ////         0.5f, -0.5f, 0.0f
    ////    };
    ////    auto squareVB = Grafix::VertexBuffer::Create(vertices, sizeof(vertices));

    ////    squareVB->SetLayout({
    ////        { Grafix::ShaderDataType::Float3, "a_Position" }
    ////    });
    ////    m_SquareVA->AddVertexBuffer(squareVB);

    ////    uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
    ////    auto squareIB = Grafix::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
    ////    m_SquareVA->SetIndexBuffer(squareIB);
    ////    
    ////    m_ShaderLibrary.LoadShader("assets/shaders/FlatColor.glsl");
    ////    m_SquareShader = m_ShaderLibrary.GetShader("FlatColor");
    ////}

    PipelineSetup();
}

TestLayer::~TestLayer()
{
    auto device = Grafix::VulkanContext::Get().GetLogicalDevice()->GetVkDevice();

    vkDeviceWaitIdle(device);

    vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);

    // In fact, when the pipeline is created, the shader modules can be destroyed.
    vkDestroyShaderModule(device, m_VertexShaderModule, nullptr);
    vkDestroyShaderModule(device, m_FragmentShaderModule, nullptr);
}

void TestLayer::OnUpdate(float ts)
{
    // Camera movement
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(m_CameraRotation), glm::vec3(0, 0, 1));
    if (Grafix::Input::IsKeyPressed(Grafix::Key::A))
        m_CameraPosition -= m_CameraMoveSpeed * glm::vec3(rotate[0].x, rotate[0].y, rotate[0].z) * ts;
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::D))
        m_CameraPosition += m_CameraMoveSpeed * glm::vec3(rotate[0].x, rotate[0].y, rotate[0].z) * ts;

    if (Grafix::Input::IsKeyPressed(Grafix::Key::W))
        m_CameraPosition += m_CameraMoveSpeed * glm::vec3(rotate[1].x, rotate[1].y, rotate[1].z) * ts;
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::S))
        m_CameraPosition -= m_CameraMoveSpeed * glm::vec3(rotate[1].x, rotate[1].y, rotate[1].z) * ts;

    // Camera rotation
    if (Grafix::Input::IsKeyPressed(Grafix::Key::Left))
        m_CameraRotation += m_CameraRotationSpeed * ts;
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::Right))
        m_CameraRotation -= m_CameraRotationSpeed * ts;

    // Triangle movement
    if (Grafix::Input::IsKeyPressed(Grafix::Key::J))
        m_TrianglePosition.x -= m_TriangleMoveSpeed * ts;
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::L))
        m_TrianglePosition.x += m_TriangleMoveSpeed * ts;

    if (Grafix::Input::IsKeyPressed(Grafix::Key::I))
        m_TrianglePosition.y += m_TriangleMoveSpeed * ts;
    else if (Grafix::Input::IsKeyPressed(Grafix::Key::K))
        m_TrianglePosition.y -= m_TriangleMoveSpeed * ts;

    m_TriangleRotation += m_TriangleRotationSpeed * ts;


    // Reset camera
    if(Grafix::Input::IsKeyPressed(Grafix::Key::R))
    {
        m_CameraPosition = glm::vec3(0.0f);
        m_CameraRotation = 0.0f;
    }

    m_Camera.SetPosition(m_CameraPosition);
    m_Camera.SetRotation(m_CameraRotation);

    Grafix::Renderer::BeginScene(m_Camera);

    ////std::dynamic_pointer_cast<Grafix::OpenGLShader>(m_SquareShader)->Bind();
    ////std::dynamic_pointer_cast<Grafix::OpenGLShader>(m_SquareShader)->UploadUniformFloat4("u_Color", m_SquareColor);

    ////{
    ////    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_TrianglePosition) 
    ////        * glm::rotate(glm::mat4(1.0f), glm::radians(m_TriangleRotation), glm::vec3(0.0f, 0.0f, 1.0f))
    ////        * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    ////    Grafix::Renderer::Submit(m_Shader, m_VertexArray, transform);
    ////}

    ////for (int y = 0; y < 15; ++y)
    ////{
    ////    for (int x = 0; x < 15; ++x)
    ////    {
    ////        glm::vec3 position = glm::vec3(x * 0.11f, y * 0.11f, 0.0f);
    ////        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    ////        Grafix::Renderer::Submit(m_SquareShader, m_SquareVA, transform);
    ////    }
    ////}

    auto device = Grafix::VulkanContext::Get().GetLogicalDevice()->GetVkDevice();
    auto swapchain = Grafix::VulkanContext::Get().GetSwapchain();
    auto commandBuffer = swapchain->GetCurrentCommandBuffer();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Begin Recording Command Buffers (requires command pool)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr;
    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Begin Render Pass (requires swapchain and render pass)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkRenderPassBeginInfo renderPassBI{};
    renderPassBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBI.renderPass = swapchain->GetVkRenderPass();
    renderPassBI.framebuffer = swapchain->GetCurrentFramebuffer();

    renderPassBI.renderArea.offset = { 0, 0 };
    renderPassBI.renderArea.extent = swapchain->GetExtent();

    VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
    renderPassBI.clearValueCount = 1;
    renderPassBI.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBI, VK_SUBPASS_CONTENTS_INLINE);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Draw commands (requires command buffer & swapchain & pipeline)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

    // Viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;

    auto& extent = swapchain->GetExtent();
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;

    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Scissor
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // Draw a triangle
    // vertexCount = 3: 3 vertices make up a triangle
    // instanceCount = 1: draw one triangle
    // firstVertex = 0: start at the first vertex in the vertex buffer
    // firstInstance = 0: the first instance ID of the vertex shader
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // End Render Pass (requires command buffer)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    vkCmdEndRenderPass(commandBuffer);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // End Recording Command Buffers
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    Grafix::Renderer::EndScene();
}

void TestLayer::OnEvent(Grafix::Event& e)
{
}

void TestLayer::PipelineSetup()
{
    auto device = Grafix::VulkanContext::Get().GetLogicalDevice()->GetVkDevice();
    auto swapchain = Grafix::VulkanContext::Get().GetSwapchain();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Shader
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string vertexFilePath = "assets/shaders/VertexShader.spv";
    std::string fragmentFilePath = "assets/shaders/FragmentShader.spv";

    auto vertexSrc = ReadFile(vertexFilePath);
    auto fragmentSrc = ReadFile(fragmentFilePath);

    // Vertex shader
    VkShaderModuleCreateInfo vertShaderCI{};
    vertShaderCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertShaderCI.codeSize = vertexSrc.size();
    vertShaderCI.pCode = (uint32_t*)vertexSrc.data();
    VK_CHECK(vkCreateShaderModule(device, &vertShaderCI, nullptr, &m_VertexShaderModule));

    // Fragment shader
    VkShaderModuleCreateInfo fragShaderCI{};
    fragShaderCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragShaderCI.codeSize = fragmentSrc.size();
    fragShaderCI.pCode = (uint32_t*)fragmentSrc.data();
    VK_CHECK(vkCreateShaderModule(device, &fragShaderCI, nullptr, &m_FragmentShaderModule));

    VkPipelineShaderStageCreateInfo vertStageCI{};
    vertStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageCI.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageCI.module = m_VertexShaderModule;
    vertStageCI.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageCI{};
    fragStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageCI.module = m_FragmentShaderModule;
    fragStageCI.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertStageCI, fragStageCI };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Pipeline (requires device & swapchain)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputCI{};
    vertexInputCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCI.vertexBindingDescriptionCount = 0;
    vertexInputCI.pVertexBindingDescriptions = nullptr;
    vertexInputCI.vertexAttributeDescriptionCount = 0;
    vertexInputCI.pVertexAttributeDescriptions = nullptr;

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI{};
    inputAssemblyCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCI.primitiveRestartEnable = VK_FALSE;
    
    VkPipelineViewportStateCreateInfo viewportCI{};
    viewportCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // Viewport and scissors will be specified dynamically, so we do not specify them here.
    // Instead, we will specify them in the command buffer.
    viewportCI.viewportCount = 1;
    viewportCI.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizerCI{};
    rasterizerCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCI.depthClampEnable = VK_FALSE;
    rasterizerCI.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCI.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerCI.lineWidth = 1.0f;  // 1.0f is the maximum line width
    rasterizerCI.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerCI.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerCI.depthBiasEnable = VK_FALSE;
    rasterizerCI.depthBiasConstantFactor = 0.0f;
    rasterizerCI.depthBiasClamp = 0.0f;
    rasterizerCI.depthBiasSlopeFactor = 0.0f;

    // Multisampling
    // This is a way to perform anti-aliasing.
    VkPipelineMultisampleStateCreateInfo multisampleCI{};
    multisampleCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCI.sampleShadingEnable = VK_FALSE;
    multisampleCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleCI.minSampleShading = 1.0f;
    multisampleCI.pSampleMask = nullptr;
    multisampleCI.alphaToCoverageEnable = VK_FALSE;
    multisampleCI.alphaToOneEnable = VK_FALSE;

    // Depth and stencil testing
    // NOTE: We are not using this for now.

    // Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                              | VK_COLOR_COMPONENT_G_BIT
                              | VK_COLOR_COMPONENT_B_BIT
                              | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendCI{};
    colorBlendCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCI.logicOpEnable = VK_FALSE;
    colorBlendCI.logicOp = VK_LOGIC_OP_COPY;
    colorBlendCI.attachmentCount = 1;
    colorBlendCI.pAttachments = &colorBlendAttachment;
    colorBlendCI.blendConstants[0] = 0.0f;
    colorBlendCI.blendConstants[1] = 0.0f;
    colorBlendCI.blendConstants[2] = 0.0f;
    colorBlendCI.blendConstants[3] = 0.0f;

    // Dynamic states
    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateCI{};
    dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCI.dynamicStateCount = (uint32_t)dynamicStates.size();
    dynamicStateCI.pDynamicStates = dynamicStates.data();

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutCI{};
    pipelineLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCI.setLayoutCount = 0;
    pipelineLayoutCI.pSetLayouts = nullptr;
    pipelineLayoutCI.pushConstantRangeCount = 0;
    pipelineLayoutCI.pPushConstantRanges = nullptr;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &m_PipelineLayout));

    // Creation of pipeline (requires shader, render pass, and pipeline layout)

    VkGraphicsPipelineCreateInfo pipelineCI{};
    pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineCI.stageCount = (uint32_t)shaderStages.size();
    pipelineCI.pStages = shaderStages.data();
    pipelineCI.pVertexInputState = &vertexInputCI;
    pipelineCI.pInputAssemblyState = &inputAssemblyCI;
    pipelineCI.pViewportState = &viewportCI;
    pipelineCI.pRasterizationState = &rasterizerCI;
    pipelineCI.pMultisampleState = &multisampleCI;
    pipelineCI.pDepthStencilState = nullptr;
    pipelineCI.pColorBlendState = &colorBlendCI;
    pipelineCI.pDynamicState = &dynamicStateCI;

    pipelineCI.layout = m_PipelineLayout;

    pipelineCI.renderPass = swapchain->GetVkRenderPass();
    pipelineCI.subpass = 0;

    pipelineCI.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCI.basePipelineIndex = -1;

    // Pipeline cache -- store and reuse data to accelerate pipeline creation
    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &m_GraphicsPipeline));
}

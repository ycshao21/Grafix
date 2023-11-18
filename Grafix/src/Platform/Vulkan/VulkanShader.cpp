#include "pch.h"
#include "VulkanShader.h"

#include "VulkanContext.h";

namespace Grafix
{
    VulkanShader::VulkanShader(const std::string& name, const std::string& vertexFilePath, const std::string& fragmentFilePath)
        : m_Name(name)
    {
        // Binary code
        const std::string vertexSrc = ReadFile(vertexFilePath);
        const std::string fragmentSrc = ReadFile(fragmentFilePath);

        // Vertex shader
        VkShaderModuleCreateInfo vertShaderCI{};
        vertShaderCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertShaderCI.codeSize = vertexSrc.size();
        vertShaderCI.pCode = (uint32_t*)vertexSrc.data();
        VK_CHECK(vkCreateShaderModule(nullptr, &vertShaderCI, nullptr, &m_VertexShaderModule));

        // Fragment shader
        VkShaderModuleCreateInfo fragShaderCI{};
        fragShaderCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragShaderCI.codeSize = fragmentSrc.size();
        fragShaderCI.pCode = (uint32_t*)fragmentSrc.data();
        VK_CHECK(vkCreateShaderModule(nullptr, &fragShaderCI, nullptr, &m_FragmentShaderModule));

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

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageCI, fragStageCI };

        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateCI{};
        dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCI.pDynamicStates = dynamicStates.data();
        dynamicStateCI.dynamicStateCount = (uint32_t)dynamicStates.size();

        VkPipelineVertexInputStateCreateInfo vertexInputCI{};
        vertexInputCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputCI.vertexBindingDescriptionCount = 0;
        vertexInputCI.pVertexBindingDescriptions = nullptr;
        vertexInputCI.vertexAttributeDescriptionCount = 0;
        vertexInputCI.pVertexAttributeDescriptions = nullptr;
    }

    ////VulkanShader::VulkanShader(const std::string& filePath)
    ////{
    ////}

    ////VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    ////{
    ////}

    VulkanShader::~VulkanShader()
    {
        auto device = VulkanContext::Get().GetLogicalDevice()->GetVkDevice();

        vkDestroyShaderModule(device, m_VertexShaderModule, nullptr);
        vkDestroyShaderModule(device, m_FragmentShaderModule, nullptr);
    }

    std::string VulkanShader::ReadFile(const std::string& filePath)
    {
        std::string result;
        std::ifstream in(filePath, std::ios::in | std::ios::binary);
        if (!in)
            GF_CORE_ERROR("Could not open file \"{0}\"", filePath);

        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();

        return result;
    }
}

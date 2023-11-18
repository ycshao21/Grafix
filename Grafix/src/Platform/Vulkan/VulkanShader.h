#pragma once

#include "Grafix/Renderer/Shader.h"
#include <vulkan/vulkan.h>

namespace Grafix
{
    class VulkanShader : public Shader
    {
    public:
        // TODO: Implement compile, so that the two functions below can be called and this can be deleted.
        VulkanShader(const std::string& name, const std::string& vertexFilePath, const std::string& fragmentFilePath);

        // These two functions cannot be called now.
        ////VulkanShader(const std::string& filePath);
        ////VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~VulkanShader();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual const std::string& GetName() const override { return m_Name; }
    private:
        std::string ReadFile(const std::string& filePath);
    private:
        uint32_t m_RendererID = 0;

        std::string m_Name;
        VkShaderModule m_VertexShaderModule;
        VkShaderModule m_FragmentShaderModule;
    };
}


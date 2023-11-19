#pragma once

#include "vulkan/vulkan.h"

namespace Grafix
{
    class VulkanPipeline
    {
    public:
        VulkanPipeline();
        ~VulkanPipeline();
    private:
        VkPipeline m_Pipeline;
    };
}

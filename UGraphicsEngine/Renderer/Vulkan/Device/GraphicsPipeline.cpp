
#include "GraphicsPipeline.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/Shader.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "GlslShaderCompiler.h"
#include <array>


namespace uncanny::vulkan
{


FGraphicsPipeline::~FGraphicsPipeline()
{
  Destroy();
}


void FGraphicsPipeline::Create(const FGraphicsPipelineSpecification& specification)
{
  m_Device = specification.vkDevice;

  CreateCache();
  CreatePipeline(specification);
}


void FGraphicsPipeline::Destroy()
{
  if (m_PipelineCache != VK_NULL_HANDLE)
  {
    vkDestroyPipelineCache(m_Device, m_PipelineCache, nullptr);
    m_PipelineCache = VK_NULL_HANDLE;
  }

  if (m_Pipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
    m_Pipeline = VK_NULL_HANDLE;
  }
}


void FGraphicsPipeline::CreateCache()
{
  VkPipelineCacheCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .initialDataSize = 0,
    .pInitialData = nullptr
  };
  VkResult result = vkCreatePipelineCache(m_Device, &createInfo, nullptr, &m_PipelineCache);
  AssertVkAndThrow(result);
}


void FGraphicsPipeline::CreatePipeline(const FGraphicsPipelineSpecification& specification)
{
  FGLSLShaderCompiler glslCompiler{};
  glslCompiler.Initialize(specification.targetVulkanVersion);

  FShader vertexShader{};
  FShader::ParseAndCreateModule(vertexShader, specification.vertexShader, EShaderCompilerStage::VERTEX,
                                &glslCompiler, m_Device);
  FShader fragmentShader{};
  FShader::ParseAndCreateModule(fragmentShader, specification.fragmentShader, EShaderCompilerStage::FRAGMENT,
                                &glslCompiler, m_Device);

  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageCreateInfos{
    VkPipelineShaderStageCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertexShader.GetHandle(),
      .pName = "main",
      .pSpecializationInfo = nullptr
    },
    VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragmentShader.GetHandle(),
        .pName = "main",
        .pSpecializationInfo = nullptr
    }
  };

  VkPipelineVertexInputStateCreateInfo vertexInputState{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .vertexBindingDescriptionCount = (u32)specification.vertexInputBindingDescriptions.size(),
    .pVertexBindingDescriptions = specification.vertexInputBindingDescriptions.data(),
    .vertexAttributeDescriptionCount = (u32)specification.vertexInputAttributeDescriptions.size(),
    .pVertexAttributeDescriptions = specification.vertexInputAttributeDescriptions.data()
  };

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE
  };

  VkPipelineViewportStateCreateInfo viewportState{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .viewportCount = 1,
    .pViewports = nullptr,
    .scissorCount = 1,
    .pScissors = nullptr
  };

  VkPipelineRasterizationStateCreateInfo rasterizationState{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_NONE,
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0.f,
    .depthBiasClamp = 0.f,
    .depthBiasSlopeFactor = 0.f,
    .lineWidth = 1.f
  };

  VkPipelineMultisampleStateCreateInfo multisampleState{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 0.f,
    .pSampleMask = nullptr,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable = VK_FALSE
  };

  VkPipelineDepthStencilStateCreateInfo depthStencilState{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .depthTestEnable = VK_FALSE,
    .depthWriteEnable = VK_FALSE,
    .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    .depthBoundsTestEnable = VK_FALSE,
    .stencilTestEnable = VK_FALSE,
    .front = {},
    .back = { .compareOp = VK_COMPARE_OP_ALWAYS },
    .minDepthBounds = 0.f,
    .maxDepthBounds = 0.f
  };

  std::array<VkPipelineColorBlendAttachmentState, 1> blendAttachmentStates{
    VkPipelineColorBlendAttachmentState{
      .blendEnable = VK_TRUE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
          | VK_COLOR_COMPONENT_B_BIT |VK_COLOR_COMPONENT_A_BIT
    }
  };

  VkPipelineColorBlendStateCreateInfo colorBlendState{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_CLEAR,
    .attachmentCount = blendAttachmentStates.size(),
    .pAttachments = blendAttachmentStates.data(),
    .blendConstants = {}
  };

  std::array<VkDynamicState, 2> dynamicStates{
      VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamicState{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .dynamicStateCount = dynamicStates.size(),
    .pDynamicStates = dynamicStates.data()
  };

  VkGraphicsPipelineCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .stageCount = shaderStageCreateInfos.size(),
    .pStages = shaderStageCreateInfos.data(),
    .pVertexInputState = &vertexInputState,
    .pInputAssemblyState = &inputAssemblyState,
    .pTessellationState = nullptr,
    .pViewportState = &viewportState,
    .pRasterizationState = &rasterizationState,
    .pMultisampleState = &multisampleState,
    .pDepthStencilState = &depthStencilState,
    .pColorBlendState = &colorBlendState,
    .pDynamicState = &dynamicState,
    .layout = specification.pipelineLayout,
    .renderPass = specification.renderPass,
    .subpass = 0,
    .basePipelineHandle = VK_NULL_HANDLE,
    .basePipelineIndex = 0
  };

  VkResult result = vkCreateGraphicsPipelines(m_Device, m_PipelineCache, 1, &createInfo, nullptr, &m_Pipeline);
  AssertVkAndThrow(result);
}


}

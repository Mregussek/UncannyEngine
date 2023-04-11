
#include "RayTracingPipeline.h"
#include "UTools/Filesystem/File.h"
#include "Shader.h"
#include "GlslShaderCompiler.h"
#include "PipelineLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FRayTracingPipeline::FRayTracingPipeline(VkDevice vkDevice)
  : m_Device(vkDevice)
{
}


void FRayTracingPipeline::Create(const FRayTracingPipelineSpecification& specification)
{
  auto CompileLoadAndCreateModule =
      [pCompiler = specification.pGlslCompiler](FShader& shaderModule, const FPath& path, EShaderCompilerStage stage)
  {
    std::vector<char> glslSource = FFile::Read(path.GetString().c_str());
    std::vector<u32> spvSource = pCompiler->Compile(glslSource.data(), stage);
    shaderModule.Create(spvSource);
  };

  FShader closestHitModule(m_Device);
  CompileLoadAndCreateModule(closestHitModule, specification.rayClosestHitPath, EShaderCompilerStage::CLOSESTHIT);
  FShader generationModule(m_Device);
  CompileLoadAndCreateModule(generationModule, specification.rayGenerationPath, EShaderCompilerStage::RAYGEN);
  FShader missModule(m_Device);
  CompileLoadAndCreateModule(missModule, specification.rayMissPath, EShaderCompilerStage::MISS);

  VkPipelineShaderStageCreateInfo rayGenStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
      .module = generationModule.GetHandle(),
      .pName = "main",
      .pSpecializationInfo = nullptr
  };
  VkPipelineShaderStageCreateInfo rayMissStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_MISS_BIT_KHR,
      .module = missModule.GetHandle(),
      .pName = "main",
      .pSpecializationInfo = nullptr
  };
  VkPipelineShaderStageCreateInfo closestHitStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
      .module = closestHitModule.GetHandle(),
      .pName = "main",
      .pSpecializationInfo = nullptr
  };
  VkPipelineShaderStageCreateInfo shaderStageCreateInfos[]{ rayGenStageInfo, rayMissStageInfo, closestHitStageInfo };

  VkRayTracingShaderGroupCreateInfoKHR rayGenGroupInfo{
    .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
    .pNext = nullptr,
    .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
    .generalShader = 0,
    .closestHitShader = VK_SHADER_UNUSED_KHR,
    .anyHitShader = VK_SHADER_UNUSED_KHR,
    .intersectionShader = VK_SHADER_UNUSED_KHR,
    .pShaderGroupCaptureReplayHandle = nullptr
  };
  VkRayTracingShaderGroupCreateInfoKHR rayMissGroupInfo{
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .pNext = nullptr,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
      .generalShader = 1,
      .closestHitShader = VK_SHADER_UNUSED_KHR,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = VK_SHADER_UNUSED_KHR,
      .pShaderGroupCaptureReplayHandle = nullptr
  };
  VkRayTracingShaderGroupCreateInfoKHR rayHitGroupInfo{
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .pNext = nullptr,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR,
      .generalShader = VK_SHADER_UNUSED_KHR,
      .closestHitShader = 2,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = VK_SHADER_UNUSED_KHR,
      .pShaderGroupCaptureReplayHandle = nullptr
  };
  VkRayTracingShaderGroupCreateInfoKHR shaderGroupCreateInfos[]{ rayGenGroupInfo, rayMissGroupInfo, rayHitGroupInfo };

  VkRayTracingPipelineCreateInfoKHR createInfo{
    .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
    .stageCount = sizeof(shaderStageCreateInfos) / sizeof(shaderStageCreateInfos[0]),
    .pStages = shaderStageCreateInfos,
    .groupCount = sizeof(shaderGroupCreateInfos) / sizeof(shaderGroupCreateInfos[0]),
    .pGroups = shaderGroupCreateInfos,
    .maxPipelineRayRecursionDepth = 1,
    .pLibraryInfo = nullptr,
    .pLibraryInterface = nullptr,
    .pDynamicState = nullptr,
    .layout = specification.pPipelineLayout->GetHandle(),
    .basePipelineHandle = VK_NULL_HANDLE,
    .basePipelineIndex = 0
  };

  VkResult result = vkCreateRayTracingPipelinesKHR(m_Device, nullptr, nullptr, 1, &createInfo,
                                                   nullptr, &m_Pipeline);
  AssertVkAndThrow(result);
}


void FRayTracingPipeline::Destroy()
{
  if (m_Pipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
  }
}


}


#include "RayTracingPipeline.h"
#include "UTools/Filesystem/File.h"
#include "Shader.h"
#include "GlslShaderCompiler.h"
#include "PipelineLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UGraphicsEngine/Renderer/Vulkan/RenderDeviceFactory.h"


namespace uncanny::vulkan
{


FRayTracingPipeline::FRayTracingPipeline(const FRenderDeviceFactory* pFactory,
                                         VkDevice vkDevice,
                                         const VkPhysicalDeviceRayTracingPipelinePropertiesKHR& properties)
  : m_Device(vkDevice),
    m_Properties(properties),
    m_pFactory(pFactory)
{
}


void FRayTracingPipeline::Create(const FRayTracingPipelineSpecification& specification)
{
  CreatePipeline(specification);
  CreateShaderBindingTable();
}


void FRayTracingPipeline::CreatePipeline(const FRayTracingPipelineSpecification& specification)
{
  auto CompileLoadAndCreateModule =
      [pCompiler = specification.pGlslCompiler](FShader& shaderModule, const FPath& path, EShaderCompilerStage stage)
  {
    std::vector<char> glslSource = FFile::Read(path.GetString().c_str());
    std::vector<u32> spvSource = pCompiler->Compile(glslSource.data(), stage);
    shaderModule.Create(spvSource);
  };

  FShader rayGenerationModule(m_Device);
  CompileLoadAndCreateModule(rayGenerationModule, specification.rayGenerationPath, EShaderCompilerStage::RAYGEN);
  FShader rayMissModule(m_Device);
  CompileLoadAndCreateModule(rayMissModule, specification.rayMissPath, EShaderCompilerStage::MISS);
  FShader rayClosestHitModule(m_Device);
  CompileLoadAndCreateModule(rayClosestHitModule, specification.rayClosestHitPath, EShaderCompilerStage::CLOSESTHIT);

  VkPipelineShaderStageCreateInfo rayGenStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
      .module = rayGenerationModule.GetHandle(),
      .pName = "main",
      .pSpecializationInfo = nullptr
  };
  VkPipelineShaderStageCreateInfo rayMissStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_MISS_BIT_KHR,
      .module = rayMissModule.GetHandle(),
      .pName = "main",
      .pSpecializationInfo = nullptr
  };
  VkPipelineShaderStageCreateInfo closestHitStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
      .module = rayClosestHitModule.GetHandle(),
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
  m_ShaderGroupCount = sizeof(shaderGroupCreateInfos) / sizeof(shaderGroupCreateInfos[0]);

  VkRayTracingPipelineCreateInfoKHR createInfo{
    .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
    .stageCount = sizeof(shaderStageCreateInfos) / sizeof(shaderStageCreateInfos[0]),
    .pStages = shaderStageCreateInfos,
    .groupCount = m_ShaderGroupCount,
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


void FRayTracingPipeline::CreateShaderBindingTable()
{
  auto alignTo = [](u32 value, u32 alignment) -> u32
  {
    return (value + alignment - 1) & ~(alignment - 1);
  };

  u32 sbtHandleSize = m_Properties.shaderGroupHandleSize;
  u32 sbtHandleAlignment = m_Properties.shaderGroupHandleAlignment;
  u32 sbtHandleSizeAligned = alignTo(sbtHandleSize, sbtHandleAlignment);
  u32 sbtSize = m_ShaderGroupCount * sbtHandleSizeAligned;

  std::vector<u8> sbtResults(sbtSize);
  VkResult result = vkGetRayTracingShaderGroupHandlesKHR(m_Device, m_Pipeline, 0, m_ShaderGroupCount, sbtSize,
                                                         sbtResults.data());
  AssertVkAndThrow(result);

  VkBufferUsageFlags bufferUsageFlags =
      VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  m_RayGenBuffer = m_pFactory->CreateBuffer();
  m_RayGenBuffer.Allocate(sbtHandleSize, bufferUsageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  m_RayGenBuffer.Fill(sbtResults.data(), sizeof(u8), sbtHandleSize);

  m_RayMissBuffer = m_pFactory->CreateBuffer();
  m_RayMissBuffer.Allocate(sbtHandleSize, bufferUsageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  m_RayMissBuffer.Fill(sbtResults.data() + sbtHandleSizeAligned, sizeof(u8), sbtHandleSize);

  m_RayClosestHitBuffer = m_pFactory->CreateBuffer();
  m_RayClosestHitBuffer.Allocate(sbtHandleSize, bufferUsageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  m_RayClosestHitBuffer.Fill(sbtResults.data() + sbtHandleSizeAligned * 2, sizeof(u8), sbtHandleSize);
}


void FRayTracingPipeline::Destroy()
{
  if (m_Pipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
  }

  m_RayGenBuffer.Free();
  m_RayMissBuffer.Free();
  m_RayClosestHitBuffer.Free();
}


}


#include "RayTracingPipeline.h"
#include "UTools/Filesystem/File.h"
#include "UTools/Logger/Log.h"
#include "Shader.h"
#include "GlslShaderCompiler.h"
#include "PipelineLayout.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"


namespace uncanny::vulkan
{


FRayTracingPipeline::~FRayTracingPipeline()
{
  Destroy();
}


void FRayTracingPipeline::Create(const FRayTracingPipelineSpecification& specification)
{
  m_Device = specification.vkDevice;
  m_pPhysicalDeviceAttributes = specification.pPhysicalDeviceAttributes;

  CreatePipeline(specification);
  CreateShaderBindingTable(specification.pProperties);
}


void FRayTracingPipeline::CreatePipeline(const FRayTracingPipelineSpecification& specification)
{
  auto CompileLoadAndCreateModule =
      [pCompiler = specification.pGlslCompiler, vkDevice = specification.vkDevice]
      (FShader& shaderModule, const FPath& path, EShaderCompilerStage stage)
      {
        const char* shaderPath = path.GetString().c_str();
        UDEBUG("Loading shader: {}", shaderPath);

        if (FPath::HasExtension(path, ".spv"))
        {
          std::vector<char> spvSource;
          spvSource = FFile::ReadBinary(shaderPath);
          shaderModule.Create(reinterpret_cast<const u32*>(spvSource.data()), spvSource.size(), vkDevice);
        }
        else
        {
          std::vector<u32> spvSource;
          std::vector<char> glslSource = FFile::Read(shaderPath);
          spvSource = pCompiler->Compile(glslSource.data(), stage);
          shaderModule.Create(spvSource.data(), sizeof(u32) * spvSource.size(), vkDevice);
        }
      };

  FShader rayGenerationModule{};
  CompileLoadAndCreateModule(rayGenerationModule, specification.rayGenerationPath, EShaderCompilerStage::RAYGEN);
  FShader rayMissModule{};
  CompileLoadAndCreateModule(rayMissModule, specification.rayMissPath, EShaderCompilerStage::MISS);
  FShader rayShadowMissModule{};
  CompileLoadAndCreateModule(rayShadowMissModule, specification.rayShadowMissPath, EShaderCompilerStage::MISS);
  FShader rayClosestHitModule{};
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
  VkPipelineShaderStageCreateInfo rayShadowMissStageInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_MISS_BIT_KHR,
      .module = rayShadowMissModule.GetHandle(),
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
  VkPipelineShaderStageCreateInfo shaderStageCreateInfos[]{ rayGenStageInfo, rayMissStageInfo,
                                                            rayShadowMissStageInfo, closestHitStageInfo };

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
  VkRayTracingShaderGroupCreateInfoKHR rayShadowMissGroupInfo{
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR,
      .pNext = nullptr,
      .type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
      .generalShader = 2,
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
      .closestHitShader = 3,
      .anyHitShader = VK_SHADER_UNUSED_KHR,
      .intersectionShader = VK_SHADER_UNUSED_KHR,
      .pShaderGroupCaptureReplayHandle = nullptr
  };
  VkRayTracingShaderGroupCreateInfoKHR shaderGroupCreateInfos[]{ rayGenGroupInfo, rayMissGroupInfo,
                                                                 rayShadowMissGroupInfo, rayHitGroupInfo };
  m_ShaderGroupCount = sizeof(shaderGroupCreateInfos) / sizeof(shaderGroupCreateInfos[0]);

  VkRayTracingPipelineCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR,
      .pNext = nullptr,
      .flags = 0,
      .stageCount = sizeof(shaderStageCreateInfos) / sizeof(shaderStageCreateInfos[0]),
      .pStages = shaderStageCreateInfos,
      .groupCount = m_ShaderGroupCount,
      .pGroups = shaderGroupCreateInfos,
      .maxPipelineRayRecursionDepth = 2,
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


void FRayTracingPipeline::CreateShaderBindingTable(const VkPhysicalDeviceRayTracingPipelinePropertiesKHR* pProperties)
{
  /*
    SBT Layout used in this sample:
    /-----------\
    | raygen    |
    |-----------|
    | miss      |
    |-----------|
    | hit       |
    \-----------/
   */
  auto alignTo = [](u32 value, u32 alignment) -> u32
  {
    return (value + alignment - 1) & ~(alignment - 1);
  };

  u32 sbtHandleSize = pProperties->shaderGroupHandleSize;
  u32 sbtHandleSizeAligned = alignTo(sbtHandleSize, pProperties->shaderGroupHandleAlignment);
  u32 sbtSize = m_ShaderGroupCount * sbtHandleSizeAligned;

  std::vector<u8> sbtResults(sbtSize);
  VkResult result = vkGetRayTracingShaderGroupHandlesKHR(m_Device, m_Pipeline, 0, m_ShaderGroupCount, sbtSize,
                                                         sbtResults.data());
  AssertVkAndThrow(result);

  VkBufferUsageFlags bufferUsageFlags =
      VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

  m_RayGenBuffer = vulkan::FBuffer(m_Device, m_pPhysicalDeviceAttributes);
  m_RayGenBuffer.Allocate(sbtHandleSize, bufferUsageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  m_RayGenBuffer.Fill(sbtResults.data(), sizeof(u8), sbtHandleSize);

  m_RayMissBuffer = vulkan::FBuffer(m_Device, m_pPhysicalDeviceAttributes);
  m_RayMissBuffer.Allocate(sbtHandleSize * 2, bufferUsageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  m_RayMissBuffer.Fill(sbtResults.data() + sbtHandleSizeAligned, sizeof(u8), sbtHandleSize * 2);

  m_RayClosestHitBuffer = vulkan::FBuffer(m_Device, m_pPhysicalDeviceAttributes);
  m_RayClosestHitBuffer.Allocate(sbtHandleSize, bufferUsageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  m_RayClosestHitBuffer.Fill(sbtResults.data() + sbtHandleSizeAligned * 3, sizeof(u8), sbtHandleSize);
}


void FRayTracingPipeline::Destroy()
{
  if (m_Pipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
    m_Pipeline = VK_NULL_HANDLE;
  }

  m_RayGenBuffer.Free();
  m_RayMissBuffer.Free();
  m_RayClosestHitBuffer.Free();
}


}

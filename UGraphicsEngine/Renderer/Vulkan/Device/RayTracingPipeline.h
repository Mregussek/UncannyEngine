
#ifndef UNCANNYENGINE_FRAYTRACINGSHADOWPIPELINE_H
#define UNCANNYENGINE_FRAYTRACINGSHADOWPIPELINE_H


#include <volk.h>
#include "UTools/Filesystem/Path.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"


namespace uncanny::vulkan
{


class FGLSLShaderCompiler;
class FPipelineLayout;


struct FRayTracingPipelineSpecification
{
  FPath rayClosestHitPath{ UEMPTY_PATH };
  FPath rayGenerationPath{ UEMPTY_PATH };
  FPath rayMissPath{ UEMPTY_PATH };
  FPath rayShadowMissPath{ UEMPTY_PATH };
  FPipelineLayout* pPipelineLayout{ nullptr };
  const VkPhysicalDeviceRayTracingPipelinePropertiesKHR* pProperties{ nullptr };
  VkDevice vkDevice{ VK_NULL_HANDLE };
  const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes{ nullptr };
  u32 targetVulkanVersion{ UUNUSED };
};


class FRayTracingPipeline
{
public:

  FRayTracingPipeline() = default;

  FRayTracingPipeline(const FRayTracingPipeline& other) = default;
  FRayTracingPipeline(FRayTracingPipeline&& other) = default;

  FRayTracingPipeline& operator=(const FRayTracingPipeline& other) = delete;
  FRayTracingPipeline& operator=(FRayTracingPipeline&& other) = delete;

  ~FRayTracingPipeline();

public:

  void Create(const FRayTracingPipelineSpecification& specification);

  void Destroy();

// Getters
public:

  [[nodiscard]] VkPipeline GetHandle() const { return m_Pipeline; }
  [[nodiscard]] const FBuffer& GetRayGenBuffer() const { return m_RayGenBuffer; }
  [[nodiscard]] const FBuffer& GetRayClosestHitBuffer() const { return m_RayClosestHitBuffer; }
  [[nodiscard]] const FBuffer& GetRayMissBuffer() const { return m_RayMissBuffer; }

private:

  void CreatePipeline(const FRayTracingPipelineSpecification& specification);
  void CreateShaderBindingTable(const VkPhysicalDeviceRayTracingPipelinePropertiesKHR* pProperties);

private:

  FBuffer m_RayGenBuffer{};
  FBuffer m_RayMissBuffer{};
  FBuffer m_RayClosestHitBuffer{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkPipeline m_Pipeline{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  u32 m_ShaderGroupCount{ 0 };

};


}


#endif //UNCANNYENGINE_FRAYTRACINGSHADOWPIPELINE_H

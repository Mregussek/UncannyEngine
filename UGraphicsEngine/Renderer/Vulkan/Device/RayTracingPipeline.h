
#ifndef UNCANNYENGINE_FRAYTRACINGSHADOWPIPELINE_H
#define UNCANNYENGINE_FRAYTRACINGSHADOWPIPELINE_H


#include <volk.h>
#include "UTools/Filesystem/Path.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"


namespace uncanny::vulkan
{


class FGLSLShaderCompiler;
class FPipelineLayout;


struct FRayTracingShadowPipelineSpecification
{
  FPath rayClosestHitPath{ "" };
  FPath rayGenerationPath{ "" };
  FPath rayMissPath{ "" };
  FPath rayShadowMissPath{ "" };
  FGLSLShaderCompiler* pGlslCompiler{ nullptr };
  FPipelineLayout* pPipelineLayout{ nullptr };
  const VkPhysicalDeviceRayTracingPipelinePropertiesKHR* pProperties{ nullptr };
  VkDevice vkDevice{ VK_NULL_HANDLE };
  const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes{ nullptr };
};


class FRayTracingPipeline
{
public:

  ~FRayTracingPipeline();

  void Create(const FRayTracingShadowPipelineSpecification& specification);

  void Destroy();

  [[nodiscard]] VkPipeline GetHandle() const { return m_Pipeline; }
  [[nodiscard]] const FBuffer& GetRayGenBuffer() const { return m_RayGenBuffer; }
  [[nodiscard]] const FBuffer& GetRayClosestHitBuffer() const { return m_RayClosestHitBuffer; }
  [[nodiscard]] const FBuffer& GetRayMissBuffer() const { return m_RayMissBuffer; }

private:

  void CreatePipeline(const FRayTracingShadowPipelineSpecification& specification);
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

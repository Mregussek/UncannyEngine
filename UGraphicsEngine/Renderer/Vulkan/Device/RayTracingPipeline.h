
#ifndef UNCANNYENGINE_RAYTRACINGPIPELINE_H
#define UNCANNYENGINE_RAYTRACINGPIPELINE_H


#include <volk.h>
#include "UTools/Filesystem/Path.h"
#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"


namespace uncanny::vulkan
{


class FRenderDeviceFactory;
class FGLSLShaderCompiler;
class FPipelineLayout;


struct FRayTracingPipelineSpecification
{
  FPath rayClosestHitPath{ "" };
  FPath rayGenerationPath{ "" };
  FPath rayMissPath{ "" };
  FGLSLShaderCompiler* pGlslCompiler{ nullptr };
  FPipelineLayout* pPipelineLayout{};
};


class FRayTracingPipeline
{

  friend class FRenderDeviceFactory;

public:

  FRayTracingPipeline() = default;

  void Create(const FRayTracingPipelineSpecification& specification);

  void Destroy();

  [[nodiscard]] VkPipeline GetHandle() const { return m_Pipeline; }
  [[nodiscard]] const FBuffer& GetRayGenBuffer() const { return m_RayGenBuffer; }
  [[nodiscard]] const FBuffer& GetRayClosestHitBuffer() const { return m_RayClosestHitBuffer; }
  [[nodiscard]] const FBuffer& GetRayMissBuffer() const { return m_RayMissBuffer; }

private:

  FRayTracingPipeline(const FRenderDeviceFactory* pFactory, VkDevice vkDevice,
                      const VkPhysicalDeviceRayTracingPipelinePropertiesKHR& properties);

  void CreatePipeline(const FRayTracingPipelineSpecification& specification);
  void CreateShaderBindingTable();


  VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_Properties{};
  FBuffer m_RayGenBuffer{};
  FBuffer m_RayMissBuffer{};
  FBuffer m_RayClosestHitBuffer{};
  VkPipeline m_Pipeline{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  const FRenderDeviceFactory* m_pFactory{ nullptr };
  u32 m_ShaderGroupCount{ 0 };

};


}


#endif //UNCANNYENGINE_RAYTRACINGPIPELINE_H

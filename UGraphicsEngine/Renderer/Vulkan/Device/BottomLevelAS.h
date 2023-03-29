
#ifndef UNCANNYENGINE_BOTTOMLEVELAS_H
#define UNCANNYENGINE_BOTTOMLEVELAS_H


#include "UGraphicsEngine/Renderer/Vulkan/Resources/Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Queue.h"
#include "Mesh.h"
#include <span>


namespace uncanny::vulkan
{


class FBottomLevelAS
{

  friend class FRenderDeviceFactory;

public:

  FBottomLevelAS() = default;

  void Build(std::span<FVertex> vertices, std::span<u32> indices, const FCommandPool& commandPool,
             const FQueue& queue);

  void Destroy();

private:

  FBottomLevelAS(const FRenderDeviceFactory* pFactory, VkDevice vkDevice);


  VkAccelerationStructureKHR m_AccelerationStructure{ VK_NULL_HANDLE };
  u64 m_DeviceAddress{ UUNUSED };
  VkDevice m_Device{ VK_NULL_HANDLE };
  const FRenderDeviceFactory* m_pRenderDeviceFactory{};

};


}


#endif //UNCANNYENGINE_BOTTOMLEVELAS_H

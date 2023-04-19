
#ifndef UNCANNYENGINE_ACCELERATIONTOPLEVELSTRUCTURE_H
#define UNCANNYENGINE_ACCELERATIONTOPLEVELSTRUCTURE_H


#include "BottomLevelAS.h"


namespace uncanny::vulkan
{


class FRenderDeviceFactory;


class FTopLevelAS
{

  friend class FRenderDeviceFactory;

public:

  FTopLevelAS() = default;

  void Build(const FBottomLevelAS& bottomLevelAS, const FCommandPool& commandPool,
             const FQueue& queue);

  void Destroy();

  [[nodiscard]] VkAccelerationStructureKHR GetHandle() const { return m_AccelerationStructure; }

private:

  FTopLevelAS(const FRenderDeviceFactory* pFactory, VkDevice vkDevice);


  FBuffer m_AccelerationMemoryBuffer{};
  VkAccelerationStructureKHR m_AccelerationStructure{ VK_NULL_HANDLE };
  u64 m_DeviceAddress{ UUNUSED };
  VkDevice m_Device{ VK_NULL_HANDLE };
  const FRenderDeviceFactory* m_pRenderDeviceFactory{};

};


}


#endif //UNCANNYENGINE_ACCELERATIONTOPLEVELSTRUCTURE_H

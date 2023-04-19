
#ifndef UNCANNYENGINE_ACCELERATIONSTRUCTURE_H
#define UNCANNYENGINE_ACCELERATIONSTRUCTURE_H


#include "Buffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Commands/CommandPool.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Queue.h"
#include "UGraphicsEngine/Renderer/RenderMesh.h"
#include <span>


namespace uncanny::vulkan
{


class FPhysicalDeviceAttributes;


class FAccelerationStructure
{
public:

  FAccelerationStructure() = default;
  FAccelerationStructure(VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);

  void AcquireSizeForBuild(VkAccelerationStructureTypeKHR type, u32 trianglesCount,
                           const VkAccelerationStructureGeometryKHR* pGeometry);

  void Create(VkAccelerationStructureTypeKHR type);

  void Build(VkAccelerationStructureTypeKHR type,
             const VkAccelerationStructureGeometryKHR* pGeometry,
             u32 primitiveCount, const FCommandPool& commandPool, const FQueue& queue);

  void Destroy();

  [[nodiscard]] VkAccelerationStructureKHR GetHandle() const { return m_AccelerationStructure; }
  [[nodiscard]] u64 GetDeviceAddress() const { return m_DeviceAddress; }

protected:

  VkDevice m_Device{ VK_NULL_HANDLE };
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };

private:

  FBuffer m_AccelerationMemoryBuffer{};
  VkAccelerationStructureKHR m_AccelerationStructure{ VK_NULL_HANDLE };
  u64 m_DeviceAddress{ UUNUSED };
  VkDeviceSize m_Size{ UUNUSED };
  VkDeviceSize m_ScratchSize{ UUNUSED };

};


}


#endif //UNCANNYENGINE_ACCELERATIONSTRUCTURE_H

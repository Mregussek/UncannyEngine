
#ifndef UNCANNYENGINE_IMAGE_H
#define UNCANNYENGINE_IMAGE_H


#include <span>
#include <vector>
#include "Memory.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/Typedefs.h"
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FPhysicalDeviceAttributes;


class FImage
{

  friend class FRenderDeviceFactory;

public:

  FImage() = default;
  ~FImage();

  void Allocate(VkFormat format, VkExtent2D extent2D, VkImageUsageFlags usage, VkImageLayout initialLayout,
                VkMemoryPropertyFlags memoryFlags, std::span<FQueueFamilyIndex> queueFamilies);
  void Free();

  void Recreate(VkExtent2D extent);

  [[nodiscard]] VkImage GetHandle() const { return m_Image; }

private:

  FImage(const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, VkDevice vkDevice);

  void ActualAllocate();


  FMemory m_Memory{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkImage m_Image{ VK_NULL_HANDLE };
  VkImageCreateInfo m_CreateInfo{};
  VkMemoryPropertyFlags m_MemoryFlags{ 0 };
  std::vector<FQueueFamilyIndex> m_QueueFamilies{};
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_IMAGE_H

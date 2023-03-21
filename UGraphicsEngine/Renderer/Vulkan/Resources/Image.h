
#ifndef UNCANNYENGINE_IMAGE_H
#define UNCANNYENGINE_IMAGE_H


#include "Memory.h"
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
                VkMemoryPropertyFlags memoryFlags);
  void Free();

  void Recreate(VkExtent2D extent);

private:

  FImage(const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes, VkDevice vkDevice);

  void ActualAllocate();


  FMemory m_Memory{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkImage m_Image{ VK_NULL_HANDLE };
  VkImageCreateInfo m_CreateInfo{};
  VkMemoryPropertyFlags m_MemoryFlags{ 0 };
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_IMAGE_H

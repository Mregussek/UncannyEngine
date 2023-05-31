
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
public:

  ~FImage();

  void Allocate(VkFormat format, VkExtent2D extent2D, VkImageUsageFlags usage, VkImageLayout initialLayout,
                VkMemoryPropertyFlags memoryFlags, std::span<FQueueFamilyIndex> queueFamilies,
                VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);
  void Free();

  void CreateView();

  void Recreate(VkExtent2D extent);

  [[nodiscard]] VkImage GetHandle() const { return m_Image; }
  [[nodiscard]] VkImageView GetHandleView() const { return m_ImageView; }
  [[nodiscard]] VkExtent3D GetExtent3D() const { return m_CreateInfo.extent; }

private:

  void ActualAllocate();

private:

  FMemory m_Memory{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkImage m_Image{ VK_NULL_HANDLE };
  VkImageView m_ImageView{ VK_NULL_HANDLE };
  VkImageCreateInfo m_CreateInfo{};
  VkMemoryPropertyFlags m_MemoryFlags{ 0 };
  std::vector<FQueueFamilyIndex> m_QueueFamilies{};
  b8 m_UsingView{ UFALSE };

};


}


#endif //UNCANNYENGINE_IMAGE_H

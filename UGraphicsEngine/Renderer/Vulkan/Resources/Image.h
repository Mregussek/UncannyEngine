
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


/// @brief FImage is a wrapper for VkImage. It also is responsible for memory management for the buffer.
/// @details This class is also owner of VkImageView for VkImage.
class FImage
{
public:

  FImage() = default;

  FImage(const FImage& other) = delete;
  FImage(FImage&& other) = default;

  FImage& operator=(const FImage& other) = delete;
  FImage& operator=(FImage&& other) = delete;

  ~FImage();

public:

  /// @brief Creates a VkImage with specified parameters and allocates memory for it.
  void Allocate(VkFormat format, VkExtent2D extent2D, VkImageUsageFlags usage, VkImageLayout initialLayout,
                VkMemoryPropertyFlags memoryFlags, std::span<FQueueFamilyIndex> queueFamilies,
                VkDevice vkDevice, const FPhysicalDeviceAttributes* pPhysicalDeviceAttributes);
  void Free();

  /// @brief Creates VkImageView for VkImage, remember to call after Allocate().
  void CreateView();

  /// @brief Recreates VkImage and VkImageView if it was created before.
  void Recreate(VkExtent2D extent);

// Getters
public:

  [[nodiscard]] VkImage GetHandle() const { return m_Image; }
  [[nodiscard]] VkImageView GetHandleView() const { return m_ImageView; }
  [[nodiscard]] VkExtent3D GetExtent3D() const { return m_CreateInfo.extent; }

// Private methods
private:

  void ActualAllocate();

// Members
private:

  VkImageCreateInfo m_CreateInfo{};
  std::vector<FQueueFamilyIndex> m_QueueFamilies{};
  FMemory m_Memory{};
  const FPhysicalDeviceAttributes* m_pPhysicalDeviceAttributes{ nullptr };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkImage m_Image{ VK_NULL_HANDLE };
  VkImageView m_ImageView{ VK_NULL_HANDLE };
  VkMemoryPropertyFlags m_MemoryFlags{ 0 };
  b8 m_UsingView{ UFALSE };

};


}


#endif //UNCANNYENGINE_IMAGE_H

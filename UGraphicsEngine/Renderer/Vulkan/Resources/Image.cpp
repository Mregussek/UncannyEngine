
#include "Image.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/PhysicalDeviceAttributes.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


FImage::FImage(const FPhysicalDeviceAttributes *pPhysicalDeviceAttributes, VkDevice vkDevice)
  : m_pPhysicalDeviceAttributes(pPhysicalDeviceAttributes),
  m_Device(vkDevice)
{
}


FImage::~FImage()
{
  Free();
}


void FImage::Allocate(VkFormat format, VkExtent2D extent2D, VkImageUsageFlags usage, VkImageLayout initialLayout,
                      VkMemoryPropertyFlags memoryFlags, std::span<FQueueFamilyIndex> queueFamilies)
{
  m_MemoryFlags = memoryFlags;

  VkImageCreateInfo createInfo{
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = format,
    .extent = { .width = extent2D.width, .height = extent2D.height, .depth = 1 },
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr,
    .initialLayout = initialLayout
  };

  if (queueFamilies.size() > 1)
  {
    m_QueueFamilies.clear();
    m_QueueFamilies.assign(queueFamilies.begin(), queueFamilies.end());
    createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = m_QueueFamilies.size();
    createInfo.pQueueFamilyIndices = m_QueueFamilies.data();
  }

  m_CreateInfo = createInfo;

  ActualAllocate();
}


void FImage::ActualAllocate()
{
  VkResult result = vkCreateImage(m_Device, &m_CreateInfo, nullptr, &m_Image);
  AssertVkAndThrow(result);

  VkMemoryRequirements memoryRequirements{};
  vkGetImageMemoryRequirements(m_Device, m_Image, &memoryRequirements);

  m_Memory.Allocate(m_Device, m_pPhysicalDeviceAttributes->GetMemoryProperties(), memoryRequirements, m_MemoryFlags,
                    UFALSE);

  VkDeviceSize memoryOffset{ 0 };
  vkBindImageMemory(m_Device, m_Image, m_Memory.GetHandle(), memoryOffset);

  m_Freed = UFALSE;
}


void FImage::Free()
{
  if (m_Freed)
  {
    return;
  }
  if (m_Image != VK_NULL_HANDLE)
  {
    vkDestroyImage(m_Device, m_Image, nullptr);
  }
  m_Memory.Free();
  m_Freed = UTRUE;
}


void FImage::Recreate(VkExtent2D extent)
{
  if (extent.width == 0 or extent.height == 0)
  {
    UWARN("Cannot recreate image, as passed extent is ({}, {})", extent.width, extent.height);
    return;
  }

  m_CreateInfo.extent = { .width = extent.width, .height = extent.height, .depth = 1 };
  Free();
  ActualAllocate();
}


}

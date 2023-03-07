
#include "Swapchain.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/WindowSurface.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"


namespace uncanny::vulkan
{


struct FSwapchainCreateAttributes
{
  std::array<VkImageUsageFlags, 2> imageUsageFlags
  {
      VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
  };
  std::array<VkFormatFeatureFlags, 2> imageFormatFeatureFlags
  {
      VK_FORMAT_FEATURE_TRANSFER_DST_BIT,
      VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
  };
  VkSurfaceFormatKHR surfaceFormat{ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
  u32 minImageCount{ 2 };
  VkSurfaceTransformFlagBitsKHR preTransform{ VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };
  VkPresentModeKHR presentMode{ VK_PRESENT_MODE_MAILBOX_KHR };
};


/*
 * @brief Function checks all attributes and replaces them if they aren't supported.
 * @return Status of replace, if everything went fine UTRUE is returned, UFALSE otherwise
 */
static b8 ReplaceRequestedAttributesWithSupportedIfNeeded(FSwapchainCreateAttributes& ca,
                                                          const FWindowSurface* pWindowSurface);


static VkImageUsageFlags CreateOneFlagFromVector(std::span<VkImageUsageFlags> vec);



void FSwapchain::Create(u32 backBufferCount, VkDevice vkDevice, const FQueue* pQueue,
                        const FWindowSurface* pWindowSurface)
{
  // Make sure that we won't wait for the GPU to complete any operations -> one additional image
  m_BackBufferCount = backBufferCount + 1;
  m_Device = vkDevice;
  m_pPresentQueue = pQueue;
  m_pWindowSurface = pWindowSurface;

  CreateOnlySwapchain(VK_NULL_HANDLE);

  m_Fences.resize(m_BackBufferCount);
  m_ImageAvailableSemaphores.resize(m_BackBufferCount);
  m_PresentableImagesReadySemaphores.resize(m_BackBufferCount);
  for(u32 i = 0; i < m_BackBufferCount; i++)
  {
    m_Fences[i].Create(m_Device);
    m_ImageAvailableSemaphores[i].Create(m_Device);
    m_PresentableImagesReadySemaphores[i].Create(m_Device);
  }
}


void FSwapchain::CreateOnlySwapchain(VkSwapchainKHR oldSwapchain)
{
  FSwapchainCreateAttributes createAttributes{};
  createAttributes.minImageCount = m_BackBufferCount;
  b8 replaced = ReplaceRequestedAttributesWithSupportedIfNeeded(createAttributes, m_pWindowSurface);
  if (not replaced)
  {
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Create attributes for swapchain are not supported!");
  }

  VkImageUsageFlags usageFlags{ CreateOneFlagFromVector(createAttributes.imageUsageFlags) };

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.surface = m_pWindowSurface->GetHandle();
  createInfo.minImageCount = createAttributes.minImageCount;
  createInfo.imageFormat = createAttributes.surfaceFormat.format;
  createInfo.imageColorSpace = createAttributes.surfaceFormat.colorSpace;
  createInfo.imageExtent = m_pWindowSurface->GetCapabilities().currentExtent;
  createInfo.imageArrayLayers = 1; // non-stereoscopic-3D app
  createInfo.imageUsage = usageFlags;
  createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // images are exclusive to queue family
  createInfo.queueFamilyIndexCount = 0;                    // for exclusive sharing mode, param is ignored
  createInfo.pQueueFamilyIndices = nullptr;               // for exclusive sharing mode, param is ignored
  createInfo.preTransform = createAttributes.preTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // no transparency with OS
  createInfo.presentMode = createAttributes.presentMode;
  createInfo.clipped = VK_TRUE; // clipping world that is beyond presented surface (not visible)
  createInfo.oldSwapchain = oldSwapchain;

  VkResult result = vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain);
  AssertVkAndThrow(result);

  u32 count{ 0 };
  result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &count, nullptr);
  AssertVkAndThrow(result);
  m_Images.resize(count);
  result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &count, m_Images.data());
  AssertVkAndThrow(result);

  m_CurrentFrame = 0;
  m_ImageIndex = 0;
  m_OutOfDate = UFALSE;
}


void FSwapchain::Destroy()
{
  if (m_Swapchain != VK_NULL_HANDLE)
  {
    vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
  }
  m_Images.clear();
  m_Images.shrink_to_fit();
  for(u32 i = 0; i < m_BackBufferCount; i++)
  {
    m_Fences[i].Destroy();
    m_ImageAvailableSemaphores[i].Destroy();
    m_PresentableImagesReadySemaphores[i].Destroy();
  }
  m_Fences.clear();
  m_Fences.shrink_to_fit();
  m_ImageAvailableSemaphores.clear();
  m_ImageAvailableSemaphores.shrink_to_fit();
  m_PresentableImagesReadySemaphores.clear();
  m_PresentableImagesReadySemaphores.shrink_to_fit();
}


void FSwapchain::Recreate()
{
  m_Images.clear();
  VkSwapchainKHR oldSwapchain = m_Swapchain;
  m_Swapchain = VK_NULL_HANDLE;

  CreateOnlySwapchain(oldSwapchain);

  vkDestroySwapchainKHR(m_Device, oldSwapchain, nullptr);
}


void FSwapchain::WaitForNextImage()
{
  u64 timeout = std::numeric_limits<u64>::max();
  VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain, timeout,
                                          m_ImageAvailableSemaphores[m_CurrentFrame].GetHandle(),
                                          VK_NULL_HANDLE, &m_ImageIndex);
  switch(result)
  {
    case VK_SUCCESS:
      break;
    case VK_SUBOPTIMAL_KHR:
      [[fallthrough]];
    case VK_ERROR_OUT_OF_DATE_KHR:
      m_OutOfDate = UTRUE;
      break;
    default:
      AssertVkAndThrow(result);
  }

  m_Fences[m_CurrentFrame].WaitAndReset();
}


void FSwapchain::Present()
{
  VkSemaphore waitSemaphores[]{ GetPresentableImageReadySemaphore() };
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = nullptr;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = waitSemaphores;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &m_Swapchain;
  presentInfo.pImageIndices = &m_ImageIndex;
  presentInfo.pResults = nullptr;

  VkResult result = vkQueuePresentKHR(m_pPresentQueue->GetHandle(), &presentInfo);
  switch(result)
  {
    case VK_SUCCESS:
      break;
    case VK_SUBOPTIMAL_KHR:
      [[fallthrough]];
    case VK_ERROR_OUT_OF_DATE_KHR:
      m_OutOfDate = UTRUE;
      break;
    default:
      AssertVkAndThrow(result);
  }

  m_CurrentFrame++;
  if (m_CurrentFrame >= m_BackBufferCount)
  {
    m_CurrentFrame = 0;
  }
}



b8 ReplaceRequestedAttributesWithSupportedIfNeeded(FSwapchainCreateAttributes& ca,
                                                   const FWindowSurface* pWindowSurface)
{
  const VkSurfaceCapabilitiesKHR& surfaceCaps = pWindowSurface->GetCapabilities();

  // Validate surface capabilities...
  // Validating min image count...
  if (not (surfaceCaps.minImageCount <= ca.minImageCount and ca.minImageCount <= surfaceCaps.maxImageCount))
  {
    UERROR("Not supported min image count for surface capabilities!");
    return UFALSE;
  }
  // Validating pre transform...
  if (not (surfaceCaps.supportedTransforms & ca.preTransform))
  {
    UERROR("Not supported pre transform for surface capabilities!");
    return UFALSE;
  }
  // Validating if all requested image usage flags are supported...
  if (std::ranges::find_if(ca.imageUsageFlags,[supportedFlags = surfaceCaps.supportedUsageFlags](auto flag)->bool
  {
    return supportedFlags & flag;
  }) == ca.imageUsageFlags.end())
  {
    UERROR("Not supported image usage flag!");
    return UFALSE;
  }
  // ... surface capabilities validated

  // Validate surface format...
  // Validating if requested surface format is supported...
  auto formats = pWindowSurface->GetFormats();
  if (std::ranges::find_if(formats, [expectedFormat = ca.surfaceFormat](auto surfaceFormat)->bool
  {
    return expectedFormat.format == surfaceFormat.format and expectedFormat.colorSpace == surfaceFormat.colorSpace;
  }) == formats.end())
  {
    UWARN("Using 0-indexed surface format as requested one is not supported!");
    ca.surfaceFormat = formats[0];
  }
  // Validating if requested surface format features are supported...
  VkFormatProperties formatProperties = pWindowSurface->GetFormatProperties(ca.surfaceFormat.format);
  VkFormatFeatureFlags actualFormatFeatureFlags;
  if (ca.imageTiling == VK_IMAGE_TILING_OPTIMAL)
  {
    actualFormatFeatureFlags = formatProperties.optimalTilingFeatures;
  }
  else
  {
    actualFormatFeatureFlags = formatProperties.linearTilingFeatures;
  }
  if (std::ranges::find_if(ca.imageFormatFeatureFlags, [actualFormatFeatureFlags](VkFormatFeatureFlags flag)->bool
  {
    return actualFormatFeatureFlags & flag;
  }) == ca.imageFormatFeatureFlags.end())
  {
    UERROR("Not supported format feature flag!");
    return UFALSE;
  }
  // ... surface format validated

  // Validate present mode...
  auto presentModes = pWindowSurface->GetPresentModes();
  if (std::ranges::find(presentModes, ca.presentMode) == presentModes.end())
  {
    UWARN("Using default FIFO present mode as required one is not supported!");
    ca.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  }
  // ... present mode validated

  return UTRUE;
}


VkImageUsageFlags CreateOneFlagFromVector(std::span<VkImageUsageFlags> vec)
{
  VkImageUsageFlags rtn{ 0 };
  std::ranges::for_each(vec, [&rtn](auto flag)
  {
    rtn |= flag;
  });
  return rtn;
}


}

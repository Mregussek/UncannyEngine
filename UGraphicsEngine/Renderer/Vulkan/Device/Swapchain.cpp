
#include "Swapchain.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/LogicalDevice.h"
#include "UGraphicsEngine/Renderer/Vulkan/Context/WindowSurface.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UTools/Logger/Log.h"
// remove stupid MSVC min/max macro definitions
#ifdef WIN32
  #undef min
  #undef max
#endif


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
  VkSurfaceFormatKHR surfaceFormat{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
  u32 minImageCount{ 2 };
  VkSurfaceTransformFlagBitsKHR preTransform{ VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR };
  VkImageTiling imageTiling{ VK_IMAGE_TILING_OPTIMAL };
  VkPresentModeKHR presentMode{ VK_PRESENT_MODE_MAILBOX_KHR };
  VkExtent2D extent{ .width = 0, .height = 0 };
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
  m_CurrentExtent = m_pWindowSurface->QueryCapabilities().currentExtent;

  CreateOnlySwapchain(VK_NULL_HANDLE);

  m_Fences.resize(m_BackBufferCount);
  m_ImageAvailableSemaphores.resize(m_BackBufferCount);
  m_PresentableImagesReadySemaphores.resize(m_BackBufferCount);
  for(u32 i = 0; i < m_BackBufferCount; i++)
  {
    m_Fences[i].Create(m_Device, VK_FENCE_CREATE_SIGNALED_BIT);
    m_ImageAvailableSemaphores[i].Create(m_Device);
    m_PresentableImagesReadySemaphores[i].Create(m_Device);
  }
}


void FSwapchain::CreateOnlySwapchain(VkSwapchainKHR oldSwapchain)
{
  FSwapchainCreateAttributes createAttributes{};
  createAttributes.minImageCount = m_BackBufferCount;
  createAttributes.extent = m_CurrentExtent;
  b8 replaced = ReplaceRequestedAttributesWithSupportedIfNeeded(createAttributes, m_pWindowSurface);
  if (not replaced)
  {
    AssertVkAndThrow(VK_ERROR_INITIALIZATION_FAILED, "Create attributes for swapchain are not supported!");
  }

  VkImageUsageFlags usageFlags{ CreateOneFlagFromVector(createAttributes.imageUsageFlags) };

  VkSwapchainCreateInfoKHR createInfo{
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
    .surface = m_pWindowSurface->GetHandle(),
    .minImageCount = createAttributes.minImageCount,
    .imageFormat = createAttributes.surfaceFormat.format,
    .imageColorSpace = createAttributes.surfaceFormat.colorSpace,
    .imageExtent = createAttributes.extent,
    .imageArrayLayers = 1, // non-stereoscopic-3D app
    .imageUsage = usageFlags,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, // images are exclusive to queue family
    .queueFamilyIndexCount = 0,                    // for exclusive sharing mode, param is ignored
    .pQueueFamilyIndices = nullptr,                // for exclusive sharing mode, param is ignored
    .preTransform = createAttributes.preTransform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, // no transparency with OS
    .presentMode = createAttributes.presentMode,
    .clipped = VK_TRUE, // clipping world that is beyond presented surface (not visible)
    .oldSwapchain = oldSwapchain
  };
  VkResult result = vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain);
  AssertVkAndThrow(result);

  m_Format = createInfo.imageFormat;
  RetrieveNewlyCreatedImages();

  m_CurrentFrame = 0;
  m_ImageIndex = 0;
  m_OutOfDate = UFALSE;
}


void FSwapchain::RetrieveNewlyCreatedImages()
{
  u32 count{ 0 };
  VkResult result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &count, nullptr);
  AssertVkAndThrow(result);
  m_Images.resize(count);
  result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &count, m_Images.data());
  AssertVkAndThrow(result);
}


void FSwapchain::CreateImageViews()
{
  m_ImageViews.reserve(m_Images.size());

  for (VkImage vkImage : m_Images)
  {
    VkImageViewCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .image = vkImage,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = GetFormat(),
      .components = VkComponentMapping{
        .r = VK_COMPONENT_SWIZZLE_R,
        .g = VK_COMPONENT_SWIZZLE_G,
        .b = VK_COMPONENT_SWIZZLE_B,
        .a = VK_COMPONENT_SWIZZLE_A
      },
      .subresourceRange = VkImageSubresourceRange{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
      }
    };

    VkImageView imageView{ VK_NULL_HANDLE };
    VkResult result = vkCreateImageView(m_Device, &createInfo, nullptr, &imageView);
    AssertVkAndThrow(result);

    m_ImageViews.push_back(imageView);
  }
}


void FSwapchain::CreateFramebuffers(VkRenderPass renderPass, VkImageView depthView)
{
  // Depth/Stencil attachment is the same for all framebuffers
  std::array<VkImageView, 2> attachments{ VK_NULL_HANDLE, depthView };

  m_Framebuffers.reserve(m_ImageViews.size());

  for (VkImageView vkImageView : m_ImageViews)
  {
    attachments[0] = vkImageView;

    VkFramebufferCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .renderPass = renderPass,
      .attachmentCount = attachments.size(),
      .pAttachments = attachments.data(),
      .width = GetCurrentExtent().width,
      .height = GetCurrentExtent().height,
      .layers = 1
    };

    VkFramebuffer framebuffer{ VK_NULL_HANDLE };
    VkResult result = vkCreateFramebuffer(m_Device, &createInfo, nullptr, &framebuffer);
    AssertVkAndThrow(result);

    m_Framebuffers.push_back(framebuffer);
  }
}


void FSwapchain::Destroy()
{
  DestroyFramebuffers();
  DestroyViews();

  if (m_Swapchain != VK_NULL_HANDLE)
  {
    vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
  }
  for(u32 i = 0; i < m_BackBufferCount; i++)
  {
    m_Fences[i].Destroy();
    m_ImageAvailableSemaphores[i].Destroy();
    m_PresentableImagesReadySemaphores[i].Destroy();
  }
}


void FSwapchain::DestroyFramebuffers()
{
  for (VkFramebuffer vkFramebuffer : m_Framebuffers)
  {
    if (vkFramebuffer != VK_NULL_HANDLE)
    {
      vkDestroyFramebuffer(m_Device, vkFramebuffer, nullptr);
    }
  }

  m_Framebuffers.clear();
}


void FSwapchain::DestroyViews()
{
  for (VkImageView vkImageView : m_ImageViews)
  {
    if (vkImageView != VK_NULL_HANDLE)
    {
      vkDestroyImageView(m_Device, vkImageView, nullptr);
    }
  }

  m_ImageViews.clear();
}


b8 FSwapchain::IsRecreatePossible() const
{
  VkExtent2D currentExtent = m_pWindowSurface->QueryCapabilities().currentExtent;
  if (currentExtent.width == 0 or currentExtent.height == 0)
  {
    UWARN("Cannot recreate swapchain, as current extent is ({}, {})", currentExtent.width, currentExtent.height);
    return UFALSE;
  }
  return UTRUE;
}


void FSwapchain::Recreate()
{
  DestroyFramebuffers();
  DestroyViews();

  m_Images.clear();

  VkSwapchainKHR oldSwapchain = m_Swapchain;
  m_Swapchain = VK_NULL_HANDLE;

  CreateOnlySwapchain(oldSwapchain);

  vkDestroySwapchainKHR(m_Device, oldSwapchain, nullptr);
}


void FSwapchain::WaitForNextImage()
{
  GetCurrentFence().WaitAndReset();

  constexpr u64 timeout = std::numeric_limits<u64>::max();
  VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain, timeout,
                                          GetCurrentImageAvailableSemaphore().GetHandle(),
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
}


void FSwapchain::Present()
{
  VkSemaphore waitSemaphores[]{ GetCurrentPresentableImageReadySemaphore().GetHandle()};
  VkPresentInfoKHR presentInfo{
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = nullptr,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = waitSemaphores,
    .swapchainCount = 1,
    .pSwapchains = &m_Swapchain,
    .pImageIndices = &m_ImageIndex,
    .pResults = nullptr
  };
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
  m_CurrentExtent = m_pWindowSurface->QueryCapabilities().currentExtent;
}


u32 FSwapchain::GetCurrentFrameIndex() const
{
  return m_CurrentFrame;
}


VkExtent2D FSwapchain::GetCurrentExtent() const
{
  return m_CurrentExtent;
}


f32 FSwapchain::GetCurrentAspectRatio() const
{
  return (f32)m_CurrentExtent.width / (f32)m_CurrentExtent.height;
}


const FFence& FSwapchain::GetCurrentFence() const
{
  return m_Fences[m_CurrentFrame];
}


const FSemaphore& FSwapchain::GetCurrentImageAvailableSemaphore() const
{
  return m_ImageAvailableSemaphores[m_CurrentFrame];
}


const FSemaphore& FSwapchain::GetCurrentPresentableImageReadySemaphore() const
{
  return m_PresentableImagesReadySemaphores[m_CurrentFrame];
}



b8 ReplaceRequestedAttributesWithSupportedIfNeeded(FSwapchainCreateAttributes& ca,
                                                   const FWindowSurface* pWindowSurface)
{
  const VkSurfaceCapabilitiesKHR& surfaceCaps = pWindowSurface->QueryCapabilities();

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
  // validating extent...
  if (surfaceCaps.currentExtent.width != ca.extent.width or surfaceCaps.currentExtent.height != ca.extent.height)
  {
    ca.extent = surfaceCaps.currentExtent;
  }
  if (ca.extent.width == 0 or ca.extent.height == 0)
  {
    UERROR("Extent is ({}, {}), surface is minimized!", ca.extent.width, ca.extent.height);
    return UFALSE;
  }
  // ... surface capabilities validated

  // Validate surface format...
  // Validating if requested surface format is supported...
  auto formats = pWindowSurface->QueryFormats();
  if (std::ranges::find_if(formats, [expectedFormat = ca.surfaceFormat](auto surfaceFormat)->bool
  {
    return expectedFormat.format == surfaceFormat.format and expectedFormat.colorSpace == surfaceFormat.colorSpace;
  }) == formats.end())
  {
    UWARN("Using 0-indexed surface format as requested one is not supported!");
    ca.surfaceFormat = formats[0];
  }
  // Validating if requested surface format features are supported...
  VkFormatProperties formatProperties = pWindowSurface->QueryFormatProperties(ca.surfaceFormat.format);
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
  auto presentModes = pWindowSurface->QueryPresentModes();
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

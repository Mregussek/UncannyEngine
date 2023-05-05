
#include "CommandBuffer.h"
#include "UGraphicsEngine/Renderer/Vulkan/Utilities.h"
#include "UGraphicsEngine/Renderer/Vulkan/Device/RayTracingPipeline.h"
#include "UTools/Logger/Log.h"
#undef MemoryBarrier


namespace uncanny::vulkan
{


FCommandBuffer::FCommandBuffer(VkDevice vkDevice, VkCommandPool vkCommandPool, VkCommandBuffer vkCommandBuffer)
  : m_CommandBuffer(vkCommandBuffer),
  m_Device(vkDevice),
  m_CommandPool(vkCommandPool)
{
}


FCommandBuffer::~FCommandBuffer()
{
  Free();
}


void FCommandBuffer::Free()
{
  if (m_Freed)
  {
    return;
  }

  if (m_CommandBuffer != VK_NULL_HANDLE)
  {
    vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &m_CommandBuffer);
    m_Freed = UTRUE;
  }
}


void FCommandBuffer::BeginRecording()
{
  if (m_Recording)
  {
    UWARN("Command buffer is during recording commands, returning...");
    return;
  }
  VkCommandBufferBeginInfo beginInfo{
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = 0,
    .pInheritanceInfo = nullptr
  };
  VkResult result = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
  AssertVkAndThrow(result);
  m_Recording = UTRUE;
}


void FCommandBuffer::BeginOneTimeRecording()
{
  if (m_Recording)
  {
    UWARN("Command buffer is during recording commands, returning...");
    return;
  }
  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr
  };
  VkResult result = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
  AssertVkAndThrow(result);
  m_Recording = UTRUE;
}


void FCommandBuffer::EndRecording()
{
  if (not m_Recording)
  {
    UWARN("Command buffer is not during recording, returing...");
    return;
  }
  VkResult result = vkEndCommandBuffer(m_CommandBuffer);
  AssertVkAndThrow(result);
  m_Recording = UFALSE;
}


void FCommandBuffer::MemoryBarrier(VkAccessFlags srcAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcStage,
                                   VkPipelineStageFlags dstStage)
{
  VkMemoryBarrier memoryBarrier{
    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
    .pNext = nullptr,
    .srcAccessMask = srcAccess,
    .dstAccessMask = dstAccess
  };

  vkCmdPipelineBarrier(m_CommandBuffer, srcStage, dstStage, VkDependencyFlags{ 0 },
                       1, &memoryBarrier,  0, nullptr, 0, nullptr);

  m_LastWaitStageFlag = dstStage;
}


void FCommandBuffer::ImageMemoryBarrier(VkImage image, VkAccessFlags srcFlags, VkAccessFlags dstFlags,
                                        VkImageLayout oldLayout, VkImageLayout newLayout,
                                        VkImageSubresourceRange subresourceRange,
                                        VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage)
{
  VkImageMemoryBarrier barrier{
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = nullptr,
    .srcAccessMask = srcFlags,
    .dstAccessMask = dstFlags,
    .oldLayout = oldLayout,
    .newLayout = newLayout,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = image,
    .subresourceRange = subresourceRange
  };

  vkCmdPipelineBarrier(m_CommandBuffer, srcStage, dstStage, VkDependencyFlags{ 0 },
                       0, nullptr, 0, nullptr, 1, &barrier);

  m_LastWaitStageFlag = dstStage;
}


void FCommandBuffer::ClearColorImage(VkImage image, VkClearColorValue clearValue,
                                     VkImageSubresourceRange subresourceRange)
{
  vkCmdClearColorImage(m_CommandBuffer,
                       image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       &clearValue,
                       1, &subresourceRange);

  m_LastWaitStageFlag = VK_PIPELINE_STAGE_TRANSFER_BIT;
}


void FCommandBuffer::CopyImage(VkImage srcImage, VkImage dstImage, VkImageSubresourceLayers subresourceLayers,
                               VkExtent2D extent)
{
  VkImageCopy copyRegion{
    .srcSubresource = subresourceLayers,
    .srcOffset = { .x = 0, .y = 0, .z = 0 },
    .dstSubresource = subresourceLayers,
    .dstOffset = { .x = 0, .y = 0, .z = 0 },
    .extent = { .width = extent.width, .height = extent.height, .depth = 1 }
  };

  vkCmdCopyImage(m_CommandBuffer,
                 srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                 dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                 1, &copyRegion);

  m_LastWaitStageFlag = VK_PIPELINE_STAGE_TRANSFER_BIT;
}


void FCommandBuffer::BuildAccelerationStructure(const VkAccelerationStructureBuildGeometryInfoKHR* pBuildGeometryInfo,
                                                const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
  vkCmdBuildAccelerationStructuresKHR(m_CommandBuffer, 1, pBuildGeometryInfo, ppBuildRangeInfos);
}


void FCommandBuffer::BindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline)
{
  vkCmdBindPipeline(m_CommandBuffer, bindPoint, pipeline);
}


void FCommandBuffer::BindDescriptorSet(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout,
                                       VkDescriptorSet descriptorSet)
{
  std::span<VkDescriptorSet> span(&descriptorSet, 1);
  BindDescriptorSets(bindPoint, pipelineLayout, span);
}


void FCommandBuffer::BindDescriptorSets(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout,
                                        std::span<VkDescriptorSet> descriptorSets)
{
  constexpr u32 firstSet = 0;
  constexpr u32 dynamicOffsetCount = 0;
  constexpr u32* pDynamicOffsets = nullptr;
  vkCmdBindDescriptorSets(m_CommandBuffer, bindPoint, pipelineLayout,
                          firstSet, descriptorSets.size(), descriptorSets.data(),
                          dynamicOffsetCount, pDynamicOffsets);
}


void FCommandBuffer::TraceRays(const FRayTracingShadowPipeline* pRayTracingShadowPipeline, VkExtent3D extent3D)
{
  const FBuffer& rayGenBuffer = pRayTracingShadowPipeline->GetRayGenBuffer();
  VkStridedDeviceAddressRegionKHR rayGenSBT{
      .deviceAddress = rayGenBuffer.GetDeviceAddress(),
      .stride = rayGenBuffer.GetFilledElementsCount(),
      .size = rayGenBuffer.GetFilledElementsCount()
  };

  const FBuffer& rayMissBuffer = pRayTracingShadowPipeline->GetRayMissBuffer();
  VkStridedDeviceAddressRegionKHR rayMissSBT{
      .deviceAddress = rayMissBuffer.GetDeviceAddress(),
      .stride = rayMissBuffer.GetFilledElementsCount() / 2,
      .size = rayMissBuffer.GetFilledElementsCount()
  };

  const FBuffer& rayClosestHitBuffer = pRayTracingShadowPipeline->GetRayClosestHitBuffer();
  VkStridedDeviceAddressRegionKHR rayHitSBT{
      .deviceAddress = rayClosestHitBuffer.GetDeviceAddress(),
      .stride = rayClosestHitBuffer.GetFilledElementsCount(),
      .size = rayClosestHitBuffer.GetFilledElementsCount()
  };

  VkStridedDeviceAddressRegionKHR rayCallableSBT{};

  vkCmdTraceRaysKHR(m_CommandBuffer, &rayGenSBT, &rayMissSBT, &rayHitSBT, &rayCallableSBT,
                    extent3D.width, extent3D.height, extent3D.depth);
}


}

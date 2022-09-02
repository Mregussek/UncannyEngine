
#include "RendererVulkan.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createUniformBuffers() {
  UTRACE("Creating uniform buffers...");

  mUniformBufferCameraVector.resize(mMaxFramesInFlight);
  VkDeviceSize bufferSize{ sizeof(FCameraUBO) };

  UDEBUG("Created uniform buffers!");
  return UTRUE;
}


b32 FRendererVulkan::closeUniformBuffers() {
  UTRACE("Closing uniform buffers...");

  UDEBUG("Closed uniform buffers!");
  return UTRUE;
}


}

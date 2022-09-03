
#include "RendererVulkan.h"
#include "VulkanBuffers.h"
#include <renderer/Camera.h>
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


static void moveCameraDataToStructUBO(FCamera* pCamera, FCameraUBO* pOutUBO);


b32 FRendererVulkan::createUniformBuffers(const FRenderSceneConfiguration& sceneConfiguration) {
  UTRACE("Creating uniform buffers...");

  VkDeviceSize bufferSize{ sizeof(FCameraUBO) };
  VkMemoryPropertyFlags memoryPropertyFlags{
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
  FCameraUBO cameraUbo{};
  moveCameraDataToStructUBO(sceneConfiguration.pCamera, &cameraUbo);
  void* pData{ &cameraUbo };

  createBuffer(mContextPtr->Device(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferSize,
               &mUniformBufferCamera.buffer.handle, "camera UBO");
  allocateAndBindBufferMemory(mContextPtr->PhysicalDevice(), mContextPtr->Device(),
                              mUniformBufferCamera.buffer.handle,
                              memoryPropertyFlags, &mUniformBufferCamera.buffer.deviceMemory,
                              "camera UBO");
  copyDataFromHostToBuffer(mContextPtr->Device(), mUniformBufferCamera.buffer.deviceMemory,
                           bufferSize, pData, "camera UBO");

  VkDescriptorBufferInfo descriptorCameraBufferInfo{};
  descriptorCameraBufferInfo.buffer = mUniformBufferCamera.buffer.handle;
  descriptorCameraBufferInfo.offset = 0;
  descriptorCameraBufferInfo.range = bufferSize;

  mUniformBufferCamera.descriptorInfo = descriptorCameraBufferInfo;

  UDEBUG("Created uniform buffers!");
  return UTRUE;
}


b32 FRendererVulkan::closeUniformBuffers() {
  UTRACE("Closing uniform buffers...");

  closeBuffer(mContextPtr->Device(), &mUniformBufferCamera.buffer, "camera ubo");
  mUniformBufferCamera.descriptorInfo = {};

  UDEBUG("Closed uniform buffers!");
  return UTRUE;
}


void moveCameraDataToStructUBO(FCamera* pCamera, FCameraUBO* pOutUBO) {
  UTRACE("Moving camera data to UBO struct...");
  pOutUBO->matrixModelViewProjection = pCamera->mMatrixMVP;
}


}

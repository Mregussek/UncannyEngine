
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

  mUniformBufferCameraVector.resize(mMaxFramesInFlight);
  VkDeviceSize bufferSize{ sizeof(FCameraUBO) };
  VkMemoryPropertyFlags memoryPropertyFlags{
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
  FCameraUBO cameraUbo{};
  moveCameraDataToStructUBO(sceneConfiguration.pCamera, &cameraUbo);
  void* pData{ &cameraUbo };

  for (FBufferVulkan& cameraUBO : mUniformBufferCameraVector) {
    createBuffer(mContextPtr->Device(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferSize,
                 &cameraUBO.handle, "camera UBO");
    allocateAndBindBufferMemory(mContextPtr->PhysicalDevice(), mContextPtr->Device(),
                                cameraUBO.handle,
                                memoryPropertyFlags, &cameraUBO.deviceMemory,
                                "camera UBO");
    copyDataFromHostToBuffer(mContextPtr->Device(), cameraUBO.deviceMemory,
                             bufferSize, pData, "camera UBO");
  }

  UDEBUG("Created uniform buffers!");
  return UTRUE;
}


b32 FRendererVulkan::closeUniformBuffers() {
  UTRACE("Closing uniform buffers...");

  if (mUniformBufferCameraVector.empty()) {
    UWARN("Cannot empty camera UBO as it is empty!");
  }
  for (FBufferVulkan& ubo : mUniformBufferCameraVector) {
    closeBuffer(mContextPtr->Device(), &ubo, "camera ubo");
  }
  mUniformBufferCameraVector.clear();

  UDEBUG("Closed uniform buffers!");
  return UTRUE;
}


void moveCameraDataToStructUBO(FCamera* pCamera, FCameraUBO* pOutUBO) {
  UTRACE("Moving camera data to UBO struct...");
  pOutUBO->matrixModelViewProjection = pCamera->mMatrixMVP;
}


}

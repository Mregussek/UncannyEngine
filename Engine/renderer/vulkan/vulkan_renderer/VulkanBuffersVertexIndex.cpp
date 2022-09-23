
#include "RendererVulkan.h"
#include <renderer/vulkan/vulkan_resources/BufferVulkan.h>
#include <renderer/vulkan/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createVertexIndexBuffersForMesh(FMesh* pMesh, FBufferVulkan* pOutVertex,
                                                     FBufferVulkan* pOutIndex) const {
  UTRACE("Creating buffers for mesh...");

  FBufferCreateStagingDependenciesVulkan stagingDeps{};
  stagingDeps.transferQueue = mContextPtr->QueueCopy();
  stagingDeps.transferCommandPool = mVkTransferCommandPool;

  FBufferCreateDependenciesVulkan createDeps{};
  createDeps.pNext = &stagingDeps;
  createDeps.physicalDevice = mContextPtr->PhysicalDevice();
  createDeps.device = mContextPtr->Device();
  createDeps.size = sizeof(pMesh->vertices[0]) * pMesh->vertices.size();
  createDeps.elemCount = pMesh->vertices.size();
  createDeps.pData = pMesh->vertices.data();
  createDeps.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  createDeps.type = EBufferType::DEVICE_WITH_STAGING;
  createDeps.logInfo = "mesh vertex";

  b32 createdVertex{ pOutVertex->create(createDeps) };
  if (not createdVertex) {
    UERROR("Could not create {} buffer!");
    return UFALSE;
  }

  createDeps.size = sizeof(pMesh->indices[0]) * pMesh->indices.size();
  createDeps.elemCount = pMesh->indices.size();
  createDeps.pData = pMesh->indices.data();
  createDeps.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  createDeps.logInfo = "mesh index";

  b32 createdIndex{ pOutIndex->create(createDeps) };
  if (not createdIndex) {
    UERROR("Could not create {} buffer!");
    return UFALSE;
  }

  return UTRUE;
}


b32 FRendererVulkan::closeVertexIndexBuffersForMesh(FBufferVulkan* pVertex,
                                                    FBufferVulkan* pIndex) const {
  UTRACE("Closing buffers for mesh...");

  pVertex->close(mContextPtr->Device());
  pIndex->close(mContextPtr->Device());

  UDEBUG("Closed buffers for mesh!");
  return UTRUE;
}


}

#include "RendererVulkan.h"
#include <renderer/vulkan/vulkan_resources/BufferVulkan.h>
#include <utilities/Logger.h>


namespace uncanny
{


b32 FRendererVulkan::createVertexIndexBuffersForMesh(FMesh* pMesh, FBufferVulkan* pOutVertex,
                                                     FBufferVulkan* pOutIndex) const {
  UTRACE("Creating buffers for mesh...");

  FBufferStagingDependenciesVulkan stagingDeps{};
  stagingDeps.transferQueue = m_Queues.QueueTransfer();
  stagingDeps.transferCommandPool = mVkTransferCommandPool;

  FBufferCreateDependenciesVulkan createDeps{};
  createDeps.pNext = &stagingDeps;
  createDeps.physicalDevice = m_PhysicalDevice.Handle();
  createDeps.device = m_LogicalDevice.Handle();
  createDeps.size = pMesh->getVerticesSizeof();
  createDeps.elemCount = pMesh->getVerticesCount();
  createDeps.pData = pMesh->getVerticesData();
  createDeps.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  createDeps.type = EBufferType::DEVICE_WITH_STAGING;
  createDeps.logInfo = "mesh vertex";

  b32 createdVertex{ pOutVertex->create(createDeps) };
  if (not createdVertex) {
    UERROR("Could not create {} buffer!");
    return UFALSE;
  }

  createDeps.size = pMesh->getIndicesSizeof();
  createDeps.elemCount = pMesh->getIndicesCount();
  createDeps.pData = pMesh->getIndicesData();
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

  pVertex->close(m_LogicalDevice.Handle());
  pIndex->close(m_LogicalDevice.Handle());

  UDEBUG("Closed buffers for mesh!");
  return UTRUE;
}


}
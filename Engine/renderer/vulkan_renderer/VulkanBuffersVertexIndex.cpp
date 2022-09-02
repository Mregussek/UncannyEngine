
#include "RendererVulkan.h"
#include "VulkanBuffers.h"
#include <renderer/vulkan_context/ContextVulkan.h>
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>



namespace uncanny
{


b32 FRendererVulkan::createVertexIndexBuffersForMesh(FMesh* pMesh, FBufferVulkan* pOutVertex,
                                                     FBufferVulkan* pOutIndex) {
  UTRACE("Creating buffers for mesh...");

  pOutVertex->elemCount = pMesh->vertices.size();
  VkDeviceSize vertexBufferSize{ sizeof(pMesh->vertices[0]) * pMesh->vertices.size() };

  createAllocateAndCopyMeshDataToBuffer(
      mContextPtr->PhysicalDevice(), mContextPtr->Device(), mVkTransferCommandPool,
      mContextPtr->QueueCopy(), vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      pMesh->vertices.data(), pOutVertex, "vertex");
  UDEBUG("Created vertex buffer for mesh!");

  pOutIndex->elemCount = pMesh->indices.size();
  VkDeviceSize indicesSize{ sizeof(pMesh->indices[0]) * pMesh->indices.size() };

  createAllocateAndCopyMeshDataToBuffer(
      mContextPtr->PhysicalDevice(), mContextPtr->Device(), mVkTransferCommandPool,
      mContextPtr->QueueCopy(), indicesSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      pMesh->indices.data(), pOutIndex, "index");
  UDEBUG("Created index buffer for mesh!");
  return UTRUE;
}


b32 FRendererVulkan::closeVertexIndexBuffersForMesh(FBufferVulkan* pVertex, FBufferVulkan* pIndex) {
  UTRACE("Closing buffers for mesh...");

  closeBuffer(mContextPtr->Device(), pVertex, "vertex");
  closeBuffer(mContextPtr->Device(), pIndex, "index");

  UDEBUG("Closed buffers for mesh!");
  return UTRUE;
}


}
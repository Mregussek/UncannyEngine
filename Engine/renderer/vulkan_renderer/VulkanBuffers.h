
#ifndef UNCANNYENGINE_VULKANBUFFERS_H
#define UNCANNYENGINE_VULKANBUFFERS_H


#include <utilities/Variables.h>
#include <volk.h>


namespace uncanny
{


b32 createBuffer(VkDevice device, VkBufferUsageFlags usage, VkDeviceSize size,
                 VkBuffer* pOutHandle, const char* logInfo);


b32 allocateAndBindBufferMemory(VkPhysicalDevice physicalDevice, VkDevice device,
                                VkBuffer bufferHandle, VkMemoryPropertyFlags memoryPropertyFlags,
                                VkDeviceMemory* pOutDeviceMemoryHandle, const char* logInfo);


b32 copyDataFromHostToBuffer(VkDevice device, VkDeviceMemory deviceMemory, VkDeviceSize size,
                             void* pData, const char* logInfo);


b32 copyDataFromDeviceBufferToBuffer(VkDevice device, VkCommandPool transferCommandPool,
                                     VkQueue transferQueue, VkDeviceSize size,
                                     VkBuffer srcBuffer, VkBuffer dstBuffer,
                                     const char* srcLogInfo, const char* dstLogInfo);


b32 createAllocateAndCopyMeshDataToBuffer(
    VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool transferCommandPool,
    VkQueue transferQueue, VkDeviceSize bufferSize, VkBufferUsageFlags usage, void* pData,
    FBufferVulkan* pOutBuffer, const char* logInfo);


b32 closeBuffer(VkDevice device, FBufferVulkan* pBuffer, const char* logInfo);


}


#endif //UNCANNYENGINE_VULKANBUFFERS_H

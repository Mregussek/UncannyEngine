
#include "BufferVulkan.h"
#include "Memory.h"
#include <renderer/vulkan_context/VulkanUtilities.h>
#include <utilities/Logger.h>


namespace uncanny
{


template<typename T> struct is_vk_device_memory : std::false_type { };
template<> struct is_vk_device_memory<VkDeviceMemory> : std::true_type { };

template<typename T> struct is_vk_buffer : std::false_type { };
template<> struct is_vk_buffer<VkBuffer> : std::true_type { };


static b32 createBufferHandle(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage,
                              VkBuffer* pOutHandle);


static b32 createHostVisibleBuffer(const FBufferCreateDependenciesVulkan& deps,
                                   VkBufferUsageFlags usageFlags,
                                   FBufferDataVulkan* pOutBuffer);


static b32 createDeviceLocalBuffer(const FBufferCreateDependenciesVulkan& deps,
                                   VkBuffer* pStagingBuffer, VkDeviceMemory* pStagingMemory,
                                   FBufferDataVulkan* pOutBuffer);


template<typename T> static b32 closeHandle(VkDevice device, T* pHandle, const char* logInfo);


b32 FBufferVulkan::create(const FBufferCreateDependenciesVulkan& deps) {
  UTRACE("Creating buffer {}...", deps.logInfo);
  mData.type = deps.type;
  mData.size = deps.size;
  mData.elemCount = deps.elemCount;
  mData.logInfo = deps.logInfo;

  if (mData.type == EBufferType::HOST_VISIBLE) {
    UTRACE("Creating only host visible buffer {}...", deps.logInfo);
    b32 created{ createHostVisibleBuffer(deps, deps.usage, &mData) };
    if (not created) {
      UERROR("Could not create host visible buffer!");
      return UFALSE;
    }
  }
  else if (mData.type == EBufferType::DEVICE_WITH_STAGING) {
    UTRACE("Creating device local buffer {} using staging one...", deps.logInfo);
    b32 created{ createDeviceLocalBuffer(deps, &mBufferStaging, &mMemoryStaging, &mData) };
    if (not created) {
      UERROR("Could not create device local buffer!");
      return UFALSE;
    }
  }
  else {
    UERROR("Wrong {} buffer create info given!", deps.logInfo);
    return UFALSE;
  }

  UDEBUG("Created buffer {}!", deps.logInfo);
  return UTRUE;
}


b32 FBufferVulkan::close(VkDevice device) {
  UTRACE("Closing buffer...");

  closeHandle<VkBuffer>(device, &mData.handle, mData.logInfo);
  closeHandle<VkDeviceMemory>(device, &mData.deviceMemory, mData.logInfo);
  closeHandle<VkBuffer>(device, &mBufferStaging, "staging");
  closeHandle<VkDeviceMemory>(device, &mMemoryStaging, "staging");
  mData.elemCount = 0;
  mData.size = 0;
  mData.logInfo = "";

  UDEBUG("Closed buffer!");
  return UTRUE;
}


VkDescriptorBufferInfo FBufferVulkan::getDescriptorBufferInfo() const {
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = mData.handle;
  bufferInfo.offset = 0;
  bufferInfo.range = mData.size;
  return bufferInfo;
}


b32 createBufferHandle(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage,
                       VkBuffer* pOutHandle, const char* logInfo) {
  VkBufferCreateInfo createInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.size = size;
  createInfo.usage = usage;
  createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  createInfo.queueFamilyIndexCount = 0;
  createInfo.pQueueFamilyIndices = nullptr;

  U_VK_ASSERT( vkCreateBuffer(device, &createInfo, nullptr, pOutHandle) );

  UTRACE("Created buffer handle {}!", logInfo);
  return UTRUE;
}


b32 createHostVisibleBuffer(const FBufferCreateDependenciesVulkan& deps,
                            VkBufferUsageFlags usageFlags, FBufferDataVulkan* pOutBuffer) {
  VkMemoryPropertyFlags hostVisiblePropertyFlags{
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };

  b32 createdHandle{ createBufferHandle(deps.device, pOutBuffer->size, usageFlags,
                                        &(pOutBuffer->handle), deps.logInfo) };
  if (not createdHandle) {
    UERROR("Could not create vulkan buffer {} handle!", deps.logInfo);
    return UFALSE;
  }

  FMemoryAllocationDependenciesVulkan allocationDeps{};
  allocationDeps.physicalDevice = deps.physicalDevice;
  allocationDeps.device = deps.device;
  allocationDeps.buffer = pOutBuffer->handle;
  allocationDeps.propertyFlags = hostVisiblePropertyFlags;
  allocationDeps.logInfo = deps.logInfo;

  b32 allocated{ FMemoryVulkan::allocate(allocationDeps, &(pOutBuffer->deviceMemory)) };
  if (not allocated) {
    UERROR("Could not allocate buffer {}!", deps.logInfo);
    return UFALSE;
  }

  FMemoryCopyDependenciesVulkan copyDeps{};
  copyDeps.pNext = nullptr;
  copyDeps.device = deps.device;
  copyDeps.deviceMemory = pOutBuffer->deviceMemory;
  copyDeps.size = pOutBuffer->size;
  copyDeps.pData = deps.pData;
  copyDeps.copyType = EMemoryCopyType::USE_HOST_MEMCPY;
  copyDeps.logInfo = deps.logInfo;

  b32 copied{ FMemoryVulkan::copy(copyDeps) };
  if (not copied) {
    UERROR("Could not copy data to buffer {}!", deps.logInfo);
    return UFALSE;
  }

  UTRACE("Created host visible buffer {}!", deps.logInfo);
  return UTRUE;
}


b32 createDeviceLocalBuffer(const FBufferCreateDependenciesVulkan& deps,
                            VkBuffer* pStagingBuffer, VkDeviceMemory* pStagingMemory,
                            FBufferDataVulkan* pOutBuffer) {
  UTRACE("Creating device local buffer {} ...", deps.logInfo);
  VkMemoryPropertyFlags hostVisiblePropertyFlags{
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };
  VkBufferUsageFlags stagingUsageFlags{ VK_BUFFER_USAGE_TRANSFER_SRC_BIT };
  VkMemoryPropertyFlags deviceLocalPropertyFlags{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
  VkBufferUsageFlags deviceLocalUsageFlags{ VK_BUFFER_USAGE_TRANSFER_DST_BIT | deps.usage };
  auto stagingDeps{reinterpret_cast<const FBufferCreateStagingDependenciesVulkan*>(deps.pNext)};

  b32 createdStagingHandle{ createBufferHandle(deps.device, deps.size, stagingUsageFlags,
                                               pStagingBuffer, "staging") };
  b32 createdHandle{ createBufferHandle(deps.device, deps.size, deviceLocalUsageFlags,
                                        &(pOutBuffer->handle), deps.logInfo) };
  if (not createdStagingHandle or not createdHandle) {
    UERROR("Could not create staging handle or device local handle buffer {}!", deps.logInfo);
    return UFALSE;
  }

  FMemoryAllocationDependenciesVulkan allocationStagingDeps{};
  allocationStagingDeps.physicalDevice = deps.physicalDevice;
  allocationStagingDeps.device = deps.device;
  allocationStagingDeps.buffer = *pStagingBuffer;
  allocationStagingDeps.propertyFlags = hostVisiblePropertyFlags;
  allocationStagingDeps.logInfo = "staging";

  b32 allocatedStaging{ FMemoryVulkan::allocate(allocationStagingDeps, pStagingMemory) };

  FMemoryAllocationDependenciesVulkan allocationDeviceDeps{};
  allocationDeviceDeps.physicalDevice = deps.physicalDevice;
  allocationDeviceDeps.device = deps.device;
  allocationDeviceDeps.buffer = pOutBuffer->handle;
  allocationDeviceDeps.propertyFlags = deviceLocalPropertyFlags;
  allocationDeviceDeps.logInfo = deps.logInfo;

  b32 allocatedDevice{ FMemoryVulkan::allocate(allocationDeviceDeps, &(pOutBuffer->deviceMemory)) };

  if (not allocatedStaging or not allocatedDevice) {
    UERROR("Could not allocate staging buffer or device buffer {}!", deps.logInfo);
    return UFALSE;
  }

  FMemoryCopyDependenciesVulkan copyHostVisibleDeps{};
  copyHostVisibleDeps.pNext = nullptr;
  copyHostVisibleDeps.device = deps.device;
  copyHostVisibleDeps.deviceMemory = *pStagingMemory;
  copyHostVisibleDeps.size = pOutBuffer->size;
  copyHostVisibleDeps.pData = deps.pData;
  copyHostVisibleDeps.copyType = EMemoryCopyType::USE_HOST_MEMCPY;
  copyHostVisibleDeps.logInfo = deps.logInfo;

  b32 copiedToStagingHostVisibleMemory{ FMemoryVulkan::copy(copyHostVisibleDeps) };
  if (not copiedToStagingHostVisibleMemory) {
    UERROR("Could not copy data to host visible staging buffer {}!", deps.logInfo);
    return UFALSE;
  }

  FMemoryCopyBuffersUsingStagingBufferDependenciesVulkan copyStagingDeps{};
  copyStagingDeps.srcBuffer = *pStagingBuffer;
  copyStagingDeps.dstBuffer = pOutBuffer->handle;
  copyStagingDeps.transferCommandPool = stagingDeps->transferCommandPool;
  copyStagingDeps.transferQueue = stagingDeps->transferQueue;
  copyStagingDeps.srcLogInfo = "staging";
  copyStagingDeps.dstLogInfo = deps.logInfo;

  FMemoryCopyDependenciesVulkan copyDeviceLocalDeps{};
  copyDeviceLocalDeps.pNext = reinterpret_cast<void*>(&copyStagingDeps);
  copyDeviceLocalDeps.device = deps.device;
  copyDeviceLocalDeps.deviceMemory = pOutBuffer->deviceMemory;
  copyDeviceLocalDeps.size = pOutBuffer->size;
  copyDeviceLocalDeps.pData = nullptr; // as it is staged copy, pData is not needed
  copyDeviceLocalDeps.copyType = EMemoryCopyType::USE_COMMAND_BUFFER;
  copyDeviceLocalDeps.logInfo = deps.logInfo;

  b32 copied{ FMemoryVulkan::copy(copyDeviceLocalDeps) };
  if (not copied) {
    UERROR("Could not copy data to device local buffer {}!", deps.logInfo);
  }

  UTRACE("Device local buffer {} with staging one!", deps.logInfo);
  return UTRUE;
}


template<typename T> b32 closeHandle(VkDevice device, T* pHandle, const char* logInfo) {
  UTRACE("Closing handle {} {}...", typeid(T).name(), logInfo);

  if (*pHandle != VK_NULL_HANDLE) {
    UTRACE("Destroying {} {}...", typeid(T).name(), logInfo);
    if constexpr (is_vk_buffer<T>::value) {
      vkDestroyBuffer(device, *pHandle, nullptr);
    }
    else if constexpr (is_vk_device_memory<T>::value) {
      vkFreeMemory(device, *pHandle, nullptr);
    }
    *pHandle = VK_NULL_HANDLE;
  }
  else {
    UWARN("As {} {} is not created, it is not destroyed!", typeid(T).name(), logInfo);
  }

  UTRACE("Closed handle {} {}!", typeid(T).name(), logInfo);
  return UTRUE;
}


}

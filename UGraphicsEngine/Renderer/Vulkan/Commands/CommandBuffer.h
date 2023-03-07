
#ifndef UNCANNYENGINE_COMMANDBUFFER_H
#define UNCANNYENGINE_COMMANDBUFFER_H


#include <volk.h>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


class FCommandBuffer
{
public:

  FCommandBuffer() = default;
  FCommandBuffer(VkDevice vkDevice, VkCommandPool vkCommandPool, VkCommandBuffer vkCommandBuffer);
  ~FCommandBuffer();

  void Free();

  void BeginRecording();
  void EndRecording();

  void ImageMemoryBarrierToStartTransfer(VkImage image) const;
  void ImageMemoryBarrierToFinishTransferAndStartPresentation(VkImage image) const;

  [[nodiscard]] VkCommandBuffer GetHandle() const { return m_CommandBuffer; }

private:

  VkCommandPool m_CommandPool{ VK_NULL_HANDLE };
  VkDevice m_Device{ VK_NULL_HANDLE };
  VkCommandBuffer m_CommandBuffer{ VK_NULL_HANDLE };
  b8 m_Recording{ UFALSE };
  b8 m_Freed{ UFALSE };

};


}


#endif //UNCANNYENGINE_COMMANDBUFFER_H


#ifndef UNCANNYENGINE_LOGICALDEVICE_H
#define UNCANNYENGINE_LOGICALDEVICE_H


#include <volk.h>
#include "LogicalDeviceAttributes.h"
#include "Queue.h"


namespace uncanny {
class FRenderContextVulkan;
}
namespace uncanny::vulkan {


class FLogicalDevice {

  // I want only RenderContext to access Create() and Destroy() methods, not more! Only reason for friendship
  friend class ::uncanny::FRenderContextVulkan;

public:

  void Wait() const;

  [[nodiscard]] FQueueFamilyIndex GetGraphicsQueueFamilyIndex() const { return m_Attributes.GetGraphicsQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetPresentQueueFamilyIndex() const { return m_Attributes.GetPresentQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetTransferQueueFamilyIndex() const { return m_Attributes.GetTransferQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetComputeQueueFamilyIndex() const { return m_Attributes.GetComputeQueueFamilyIndex(); }

  [[nodiscard]] const FQueue& GetGraphicsQueue() const { return m_GraphicsQueue; }
  [[nodiscard]] const FQueue& GetPresentQueue() const { return m_PresentQueue; }
  [[nodiscard]] const FQueue& GetTransferQueue() const { return m_TransferQueue; }
  [[nodiscard]] const FQueue& GetComputeQueue() const { return m_ComputeQueue; }

  [[nodiscard]] VkDevice GetHandle() const { return m_Device; }

  [[nodiscard]] b32 IsValid() const noexcept { return m_Device != VK_NULL_HANDLE; }

private:

  void Create(const FLogicalDeviceAttributes& attributes, VkPhysicalDevice vkPhysicalDevice);
  void Destroy();
  void InitializeQueues();


  FLogicalDeviceAttributes m_Attributes{};
  FQueue m_GraphicsQueue{};
  FQueue m_PresentQueue{};
  FQueue m_TransferQueue{};
  FQueue m_ComputeQueue{};
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICE_H

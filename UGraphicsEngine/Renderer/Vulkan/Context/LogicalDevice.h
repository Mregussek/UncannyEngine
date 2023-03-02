
#ifndef UNCANNYENGINE_LOGICALDEVICE_H
#define UNCANNYENGINE_LOGICALDEVICE_H


#include <volk.h>
#include "LogicalDeviceAttributes.h"
#include "Queue.h"


namespace uncanny::vulkan
{


/*
 * @brief FLogicalDevice is general wrapper class for VkDevice handle. As this logical device travels across
 * several classes in RenderDevice I decided to hide Create() and Destroy() methods from the end user and
 * make friendship with RenderContext so that only RenderContext can manage its lifetime.
 */
class FLogicalDevice
{

  // I want only RenderContext to access Create() and Destroy() methods, not more! Only reason for friendship
  friend class FRenderContext;

public:

  void Wait() const;

  [[nodiscard]] const FQueue& GetGraphicsQueue() const { return m_GraphicsQueue; }
  [[nodiscard]] const FQueue& GetPresentQueue() const { return m_PresentQueue; }
  [[nodiscard]] const FQueue& GetTransferQueue() const { return m_TransferQueue; }
  [[nodiscard]] const FQueue& GetComputeQueue() const { return m_ComputeQueue; }

  [[nodiscard]] VkDevice GetHandle() const { return m_Device; }

  [[nodiscard]] b32 IsValid() const noexcept { return m_Device != VK_NULL_HANDLE; }

  [[nodiscard]] FQueueFamilyIndex GetGraphicsFamilyIndex() const { return m_Attributes.GetGraphicsQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetPresentFamilyIndex() const { return m_Attributes.GetPresentQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetTransferFamilyIndex() const { return m_Attributes.GetTransferQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetComputeFamilyIndex() const { return m_Attributes.GetComputeQueueFamilyIndex(); }

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

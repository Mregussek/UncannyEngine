
#ifndef UNCANNYENGINE_LOGICALDEVICE_H
#define UNCANNYENGINE_LOGICALDEVICE_H


#include <volk.h>
#include "LogicalDeviceAttributes.h"
#include "Queue.h"


namespace uncanny::vulkan
{


/// @brief FLogicalDevice is general wrapper class for VkDevice handle.
/// @details This class should expose Queues, Queue Families to the end user as VkDevice is actually a owner
/// of those. Even I don't like those GetHandle() methods I have decided to put it there, so that every
/// VkDevice-dependent wrapper class can store and use it for its creation / destruction. Other operations
/// must not be allowed! Remember about single responsibility rule!
/// @friend As this logical device travels across several classes in RenderDevice I decided to hide Create()
/// and Destroy() methods from the end user and make friendship with RenderContext so that only RenderContext
/// can manage its lifetime.
class FLogicalDevice
{

  // I want only RenderContext to access Create() and Destroy() methods, not more! Only reason for friendship
  friend class FRenderContext;

public:

  FLogicalDevice() = default;

  FLogicalDevice(const FLogicalDevice& other) = delete;
  FLogicalDevice(FLogicalDevice&& other) = delete;

  FLogicalDevice& operator=(const FLogicalDevice& other) = delete;
  FLogicalDevice& operator=(FLogicalDevice&& other) = delete;

public:

  /// @brief Waits for a device to become idle
  void WaitIdle() const;

  /// @brief Validates if logical device is valid (if it is created)
  /// @returns boolean information, true for valid logical device, false otherwise
  [[nodiscard]] b32 IsValid() const noexcept { return m_Device != VK_NULL_HANDLE; }

// Getters
public:

  [[nodiscard]] VkDevice GetHandle() const { return m_Device; }
  [[nodiscard]] const FLogicalDeviceAttributes& GetAttributes() const { return m_Attributes; }

  [[nodiscard]] const FQueue& GetGraphicsQueue() const { return m_GraphicsQueue; }
  [[nodiscard]] const FQueue& GetPresentQueue() const { return m_PresentQueue; }
  [[nodiscard]] const FQueue& GetTransferQueue() const { return m_TransferQueue; }
  [[nodiscard]] const FQueue& GetComputeQueue() const { return m_ComputeQueue; }

  [[nodiscard]] FQueueFamilyIndex GetGraphicsFamilyIndex() const { return m_Attributes.GetGraphicsFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetPresentFamilyIndex() const { return m_Attributes.GetPresentFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetTransferFamilyIndex() const { return m_Attributes.GetTransferFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetComputeFamilyIndex() const { return m_Attributes.GetComputeFamilyIndex(); }

// Private methods
private:

  void Create(const FLogicalDeviceAttributes& attributes, VkPhysicalDevice vkPhysicalDevice);
  void Destroy();
  void InitializeQueues();

// Members
private:

  FLogicalDeviceAttributes m_Attributes{};
  FQueue m_GraphicsQueue{};
  FQueue m_PresentQueue{};
  FQueue m_TransferQueue{};
  FQueue m_ComputeQueue{};
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICE_H

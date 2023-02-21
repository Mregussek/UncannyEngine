
#ifndef UNCANNYENGINE_LOGICALDEVICE_H
#define UNCANNYENGINE_LOGICALDEVICE_H


#include "UGraphicsEngine/3rdparty/volk/volk.h"
#include "LogicalDeviceAttributes.h"
#include "LogicalDeviceFactory.h"
#include "Queue.h"


namespace uncanny::vulkan {


class FLogicalDevice {
public:

  void Create(const FLogicalDeviceAttributes& attributes, VkPhysicalDevice vkPhysicalDevice);
  void Destroy();

  void Wait() const;

  [[nodiscard]] FQueueFamilyIndex GetGraphicsQueueFamilyIndex() const { return m_Attributes.GetGraphicsQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetPresentQueueFamilyIndex() const { return m_Attributes.GetPresentQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetTransferQueueFamilyIndex() const { return m_Attributes.GetTransferQueueFamilyIndex(); }
  [[nodiscard]] FQueueFamilyIndex GetComputeQueueFamilyIndex() const { return m_Attributes.GetComputeQueueFamilyIndex(); }

  [[nodiscard]] const FLogicalDeviceFactory& GetFactory() const { return m_Factory; }

private:

  void InitializeQueues();


  FLogicalDeviceAttributes m_Attributes{};
  FLogicalDeviceFactory m_Factory{};
  FQueue m_GraphicsQueue{};
  FQueue m_PresentQueue{};
  FQueue m_TransferQueue{};
  FQueue m_ComputeQueue{};
  VkDevice m_Device{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICE_H

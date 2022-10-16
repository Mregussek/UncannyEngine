
#ifndef UNCANNYENGINE_PHYSICALDEVICE_H
#define UNCANNYENGINE_PHYSICALDEVICE_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf
{


struct FPhysicalDeviceInitDependenciesVulkan {
  VkInstance instance{ VK_NULL_HANDLE };
};


class FPhysicalDeviceVulkan {
public:

  b32 init(const FPhysicalDeviceInitDependenciesVulkan& deps);
  void terminate();

  [[nodiscard]] VkPhysicalDevice Handle() const { return m_VkPhysicalDevice; }
  [[nodiscard]] VkQueueFamilyProperties QueueFamilyPropertiesGraphics() const { return m_VkQueueFamilyPropertiesGraphics; }
  [[nodiscard]] VkQueueFamilyProperties QueueFamilyPropertiesTransfer() const { return m_VkQueueFamilyPropertiesTransfer; }
  [[nodiscard]] u32 QueueFamilyGraphics() const { return m_QueueFamilyGraphics; }
  [[nodiscard]] u32 QueueFamilyTransfer() const { return m_QueueFamilyTransfer; }

private:

  VkPhysicalDevice m_VkPhysicalDevice{ VK_NULL_HANDLE };
  VkQueueFamilyProperties m_VkQueueFamilyPropertiesGraphics{};
  VkQueueFamilyProperties m_VkQueueFamilyPropertiesTransfer{};
  u32 m_QueueFamilyGraphics{ UUNUSED };
  u32 m_QueueFamilyTransfer{ UUNUSED };

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICE_H

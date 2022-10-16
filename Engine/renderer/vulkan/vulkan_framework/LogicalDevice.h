
#ifndef UNCANNYENGINE_LOGICALDEVICE_H
#define UNCANNYENGINE_LOGICALDEVICE_H


#include <volk.h>
#include <utilities/Variables.h>


namespace uncanny::vkf
{


struct FLogicalDeviceInitDependenciesVulkan {
  VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
  VkQueueFamilyProperties queueFamilyPropertiesGraphics{};
  VkQueueFamilyProperties queueFamilyPropertiesTransfer{};
  u32 queueFamilyIndexGraphics{ UUNUSED };
  u32 queueFamilyIndexTransfer{ UUNUSED };
};


class FLogicalDeviceVulkan {
public:

  b32 init(const FLogicalDeviceInitDependenciesVulkan& deps);
  void terminate();

  void waitIdle() const;

  [[nodiscard]] VkDevice Handle() const { return m_VkDevice; }

private:

  VkDevice m_VkDevice{ VK_NULL_HANDLE };

};


}


#endif //UNCANNYENGINE_LOGICALDEVICE_H

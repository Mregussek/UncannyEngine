
#ifndef UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H
#define UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan {


class FPhysicalDeviceAttributes {
public:

  void Initialize(VkPhysicalDevice physicalDevice);

  b8 IsExtensionPresent(const char* extensionName) const;

  [[nodiscard]] const VkPhysicalDeviceProperties& GetDeviceProperties() const { return m_Properties; }
  [[nodiscard]] const std::vector<VkExtensionProperties>& GetExtensionProperties() const { return m_ExtensionProperties; }
  [[nodiscard]] const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const { return m_QueueFamilyProperties; }

private:

  VkPhysicalDeviceProperties m_Properties{};
  std::vector<VkExtensionProperties> m_ExtensionProperties{};
  std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties{};

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H

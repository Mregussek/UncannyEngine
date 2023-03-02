
#ifndef UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H
#define UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H


#include <volk.h>
#include <vector>
#include "UTools/UTypes.h"


namespace uncanny::vulkan
{


/*
 * @brief FPhysicalDeviceAttributes is a helper class for FPhysicalDevice. Also, it is used in FPhysicalDeviceSelector
 * as it is written in a way that, it only retrieves information about physical device. It is used
 * for retrieving information about device extensions, properties, queue families etc.
 */
class FPhysicalDeviceAttributes
{
public:

  void Initialize(VkPhysicalDevice vkPhysicalDevice);

  b8 IsExtensionPresent(const char* extensionName) const;

  [[nodiscard]] const VkPhysicalDeviceProperties& GetDeviceProperties() const { return m_Properties; }
  [[nodiscard]] const VkPhysicalDeviceFeatures& GetDeviceFeatures() const { return m_Features; }
  [[nodiscard]] const std::vector<VkExtensionProperties>& GetExtensionProperties() const { return m_ExtensionProperties; }
  [[nodiscard]] const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const { return m_QueueFamilyProperties; }

private:

  VkPhysicalDeviceProperties m_Properties{};
  VkPhysicalDeviceFeatures m_Features{};
  std::vector<VkExtensionProperties> m_ExtensionProperties{};
  std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties{};

};


}


#endif //UNCANNYENGINE_PHYSICALDEVICEATTRIBUTES_H
